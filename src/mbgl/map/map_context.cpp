#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/map/annotation.hpp>
#include <mbgl/annotation/sprite_store.hpp>

#include <mbgl/platform/log.hpp>

#include <mbgl/renderer/painter.hpp>

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/geometry/sprite_atlas.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_layer.hpp>

#include <mbgl/util/gl_object_store.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/string.hpp>

#include <algorithm>

namespace mbgl {

MapContext::MapContext(View& view_, FileSource& fileSource, MapData& data_)
    : view(view_),
      data(data_),
      asyncUpdate(std::make_unique<uv::async>(util::RunLoop::getLoop(), [this] { update(); })),
      texturePool(std::make_unique<TexturePool>()),
      viewInvalidated(false) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    util::ThreadContext::setFileSource(&fileSource);
    util::ThreadContext::setGLObjectStore(&glObjectStore);

    asyncUpdate->unref();

    view.activate();
}

MapContext::~MapContext() {
    // Make sure we call cleanup() before deleting this object.
    assert(!style);
}

void MapContext::cleanup() {
    view.notify();

    if (styleRequest) {
        FileSource* fs = util::ThreadContext::getFileSource();
        fs->cancel(styleRequest);
        styleRequest = nullptr;
    }

    // Explicit resets currently necessary because these abandon resources that need to be
    // cleaned up by glObjectStore.performCleanup();
    style.reset();
    painter.reset();
    texturePool.reset();

    glObjectStore.performCleanup();

    view.deactivate();
}

void MapContext::pause() {
    MBGL_CHECK_ERROR(glFinish());

    viewInvalidated = false;

    view.deactivate();

    std::unique_lock<std::mutex> lockPause(data.mutexPause);
    data.condPaused.notify_all();
    data.condResume.wait(lockPause);

    view.activate();

    invalidateView();
}

void MapContext::triggerUpdate(const TransformState& state, const Update flags) {
    transformState = state;
    updateFlags |= flags;

    asyncUpdate->send();
}

void MapContext::setStyleURL(const std::string& url) {
    FileSource* fs = util::ThreadContext::getFileSource();

    if (styleRequest) {
        fs->cancel(styleRequest);
    }

    styleURL = url;
    styleJSON.clear();

    style = std::make_unique<Style>(data);

    const size_t pos = styleURL.rfind('/');
    std::string base = "";
    if (pos != std::string::npos) {
        base = styleURL.substr(0, pos + 1);
    }

    styleRequest = fs->request({ Resource::Kind::Style, styleURL }, util::RunLoop::getLoop(), [this, base](const Response &res) {
        styleRequest = nullptr;

        if (res.status == Response::Successful) {
            loadStyleJSON(res.data, base);
        } else {
            Log::Error(Event::Setup, "loading style failed: %s", res.message.c_str());
        }
    });
}

void MapContext::setStyleJSON(const std::string& json, const std::string& base) {
    styleURL.clear();
    styleJSON = json;

    style = std::make_unique<Style>(data);

    loadStyleJSON(json, base);
}

void MapContext::loadStyleJSON(const std::string& json, const std::string& base) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    style->setJSON(json, base);
    style->setObserver(this);

    // force style cascade, causing all pending transitions to complete.
    style->cascade();

    updateFlags |= Update::DefaultTransition | Update::Classes | Update::Zoom;
    asyncUpdate->send();

    auto staleTiles = data.getAnnotationManager()->resetStaleTiles();
    if (!staleTiles.empty()) {
        updateAnnotationTiles(staleTiles);
    }
}

void MapContext::updateAnnotationTiles(const std::unordered_set<TileID, TileID::Hash>& ids) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    util::exclusive<AnnotationManager> annotationManager = data.getAnnotationManager();
    annotationManager->markStaleTiles(ids);

    if (!style) {
        return;
    }

    // grab existing, single shape annotations source
    const auto& shapeID = AnnotationManager::ShapeLayerID;
    Source* shapeAnnotationSource = style->getSource(shapeID);

    // Style not parsed yet
    if (!shapeAnnotationSource) {
        return;
    }

    shapeAnnotationSource->enabled = true;

    // create (if necessary) layers and buckets for each shape
    for (const auto &shapeAnnotationID : annotationManager->getOrderedShapeAnnotations()) {
        const std::string shapeLayerID = shapeID + "." + util::toString(shapeAnnotationID);

        const auto layer_it = std::find_if(style->layers.begin(), style->layers.end(),
            [&shapeLayerID](util::ptr<StyleLayer> layer) {
            return (layer->id == shapeLayerID);
        });

        if (layer_it == style->layers.end()) {
            // query shape styling
            auto& shapeStyle = annotationManager->getAnnotationStyleProperties(shapeAnnotationID);

            // apply shape paint properties
            ClassProperties paintProperties;

            if (shapeStyle.is<LineProperties>()) {
                // opacity
                PropertyValue lineOpacity = ConstantFunction<float>(shapeStyle.get<LineProperties>().opacity);
                paintProperties.set(PropertyKey::LineOpacity, lineOpacity);

                // line width
                PropertyValue lineWidth = ConstantFunction<float>(shapeStyle.get<LineProperties>().width);
                paintProperties.set(PropertyKey::LineWidth, lineWidth);

                // stroke color
                PropertyValue strokeColor = ConstantFunction<Color>(shapeStyle.get<LineProperties>().color);
                paintProperties.set(PropertyKey::LineColor, strokeColor);
            } else if (shapeStyle.is<FillProperties>()) {
                // opacity
                PropertyValue fillOpacity = ConstantFunction<float>(shapeStyle.get<FillProperties>().opacity);
                paintProperties.set(PropertyKey::FillOpacity, fillOpacity);

                // fill color
                PropertyValue fillColor = ConstantFunction<Color>(shapeStyle.get<FillProperties>().fill_color);
                paintProperties.set(PropertyKey::FillColor, fillColor);

                // stroke color
                PropertyValue strokeColor = ConstantFunction<Color>(shapeStyle.get<FillProperties>().stroke_color);
                paintProperties.set(PropertyKey::FillOutlineColor, strokeColor);
            }

            std::map<ClassID, ClassProperties> shapePaints;
            shapePaints.emplace(ClassID::Default, std::move(paintProperties));

            // create shape layer
            util::ptr<StyleLayer> shapeLayer = std::make_shared<StyleLayer>(shapeLayerID, std::move(shapePaints));
            shapeLayer->type = (shapeStyle.is<LineProperties>() ? StyleLayerType::Line : StyleLayerType::Fill);

            // add to end of other shape layers just before (last) point layer
            style->layers.emplace((style->layers.end() - 1), shapeLayer);

            // create shape bucket & connect to source
            util::ptr<StyleBucket> shapeBucket = std::make_shared<StyleBucket>(shapeLayer->type);
            shapeBucket->name = shapeLayer->id;
            shapeBucket->source = shapeID;
            shapeBucket->source_layer = shapeLayer->id;

            // apply line layout properties to bucket
            if (shapeStyle.is<LineProperties>()) {
                shapeBucket->layout.set(PropertyKey::LineJoin, ConstantFunction<JoinType>(JoinType::Round));
            }

            // connect layer to bucket
            shapeLayer->bucket = shapeBucket;
        }
    }

    // invalidate annotations layer tiles
    for (const auto &source : style->sources) {
        if (source->info.type == SourceType::Annotations) {
            source->invalidateTiles(ids);
        }
    }

    updateFlags |= Update::Classes;
    asyncUpdate->send();

    annotationManager->resetStaleTiles();
}

void MapContext::update() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    if (!style) {
        updateFlags = Update::Nothing;
    }

    if (updateFlags == Update::Nothing || (data.mode == MapMode::Still && !callback)) {
        return;
    }

    data.setAnimationTime(Clock::now());

    if (updateFlags & Update::Classes) {
        style->cascade();
    }

    if (updateFlags & Update::Classes || updateFlags & Update::Zoom) {
        style->recalculate(transformState.getNormalizedZoom());
    }

    style->update(transformState, *texturePool);

    if (data.mode == MapMode::Continuous) {
        invalidateView();
    } else if (callback && isLoaded()) {
        renderSync(transformState, frameData);
    }

    updateFlags = Update::Nothing;
}

void MapContext::renderStill(const TransformState& state, const FrameData& frame, Map::StillImageCallback fn) {
    if (!fn) {
        Log::Error(Event::General, "StillImageCallback not set");
        return;
    }

    if (data.mode != MapMode::Still) {
        fn(std::make_exception_ptr(util::MisuseException("Map is not in still image render mode")), nullptr);
        return;
    }

    if (callback) {
        fn(std::make_exception_ptr(util::MisuseException("Map is currently rendering an image")), nullptr);
        return;
    }

    if (!style) {
        fn(std::make_exception_ptr(util::MisuseException("Map doesn't have a style")), nullptr);
        return;
    }

    if (style->getLastError()) {
        fn(style->getLastError(), nullptr);
        return;
    }

    callback = fn;
    transformState = state;
    frameData = frame;

    updateFlags |= Update::RenderStill;
    asyncUpdate->send();
}

bool MapContext::renderSync(const TransformState& state, const FrameData& frame) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    // Style was not loaded yet.
    if (!style) {
        return false;
    }

    view.beforeRender();

    transformState = state;

    // Cleanup OpenGL objects that we abandoned since the last render call.
    glObjectStore.performCleanup();

    if (!painter) {
        painter = std::make_unique<Painter>(data);
        painter->setup();
    }

    painter->setDebug(data.getDebug());
    painter->render(*style, transformState, frame, data.getAnimationTime());

    if (data.mode == MapMode::Still) {
        callback(nullptr, view.readStillImage());
        callback = nullptr;
    }

    view.afterRender();
    viewInvalidated = false;
    data.setNeedsRepaint(style->hasTransitions() || painter->needsAnimation());

    return isLoaded();
}

bool MapContext::isLoaded() const {
    return style->isLoaded();
}

double MapContext::getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    auto sprite = style->spriteStore->getSprite(symbol);
    if (sprite) {
        return -sprite->height / 2;
    } else {
        return 0;
    }
}

void MapContext::setSourceTileCacheSize(size_t size) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    if (size != sourceCacheSize) {
        sourceCacheSize = size;
        if (!style) return;
        for (const auto &source : style->sources) {
            source->setCacheSize(sourceCacheSize);
        }
        invalidateView();
    }
}

void MapContext::onLowMemory() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    if (!style) return;
    for (const auto &source : style->sources) {
        source->onLowMemory();
    }
    invalidateView();
}

void MapContext::setSprite(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    if (!style) {
        Log::Info(Event::Sprite, "Ignoring sprite without stylesheet");
        return;
    }

    style->spriteStore->setSprite(name, sprite);

    style->spriteAtlas->updateDirty();
}

void MapContext::onTileDataChanged() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    updateFlags |= Update::Repaint;
    asyncUpdate->send();
}

void MapContext::onResourceLoadingFailed(std::exception_ptr error) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    if (data.mode == MapMode::Still && callback) {
        callback(error, nullptr);
        callback = nullptr;
    }
}

void MapContext::invalidateView() {
    if (!viewInvalidated) {
        viewInvalidated = true;
        view.invalidate();
    }
}

}
