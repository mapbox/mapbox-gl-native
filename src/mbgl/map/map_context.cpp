#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/map/annotation.hpp>

#include <mbgl/platform/log.hpp>

#include <mbgl/renderer/painter.hpp>

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_layer.hpp>

#include <mbgl/util/gl_object_store.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/util/exception.hpp>

#include <algorithm>

namespace mbgl {

MapContext::MapContext(uv_loop_t* loop, View& view_, FileSource& fileSource, MapData& data_)
    : view(view_),
      data(data_),
      updated(static_cast<UpdateType>(Update::Nothing)),
      asyncUpdate(std::make_unique<uv::async>(loop, [this] { update(); })),
      texturePool(std::make_unique<TexturePool>()) {
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

    view.deactivate();

    std::unique_lock<std::mutex> lockPause(data.mutexPause);
    data.condPaused.notify_all();
    data.condResume.wait(lockPause);

    view.activate();
}

void MapContext::resize(uint16_t width, uint16_t height, float ratio) {
    view.resize(width, height, ratio);
}

void MapContext::triggerUpdate(const TransformState& state, const Update u) {
    transformState = state;
    updated |= static_cast<UpdateType>(u);

    asyncUpdate->send();
}

void MapContext::setStyleURL(const std::string& url) {
    FileSource* fs = util::ThreadContext::getFileSource();

    if (styleRequest) {
        fs->cancel(styleRequest);
    }

    styleURL = url;
    styleJSON.clear();

    const size_t pos = styleURL.rfind('/');
    std::string base = "";
    if (pos != std::string::npos) {
        base = styleURL.substr(0, pos + 1);
    }

    styleRequest = fs->request({ Resource::Kind::Style, styleURL }, util::RunLoop::current.get()->get(), [this, base](const Response &res) {
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

    loadStyleJSON(json, base);
}

void MapContext::loadStyleJSON(const std::string& json, const std::string& base) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    style.reset();
    style = std::make_unique<Style>(json, base, asyncUpdate->get()->loop);
    style->cascade(data.getClasses());
    style->setDefaultTransitionDuration(data.getDefaultTransitionDuration());
    style->setObserver(this);

    updated |= static_cast<UpdateType>(Update::Zoom);
    asyncUpdate->send();

    auto staleTiles = data.annotationManager.resetStaleTiles();
    if (staleTiles.size()) {
        updateAnnotationTiles(staleTiles);
    }
}

void MapContext::updateAnnotationTiles(const std::unordered_set<TileID, TileID::Hash>& ids) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    data.annotationManager.markStaleTiles(ids);

    if (!style) return;

    // grab existing, single shape annotations source
    const auto& shapeID = AnnotationManager::ShapeLayerID;
    style->getSource(shapeID)->enabled = true;

    // create (if necessary) layers and buckets for each shape
    for (const auto &shapeAnnotationID : data.annotationManager.getOrderedShapeAnnotations()) {
        const std::string shapeLayerID = shapeID + "." + std::to_string(shapeAnnotationID);

        const auto layer_it = std::find_if(style->layers.begin(), style->layers.end(),
            [&shapeLayerID](util::ptr<StyleLayer> layer) {
            return (layer->id == shapeLayerID);
        });

        if (layer_it == style->layers.end()) {
            // query shape styling
            auto& shapeStyle = data.annotationManager.getAnnotationStyleProperties(shapeAnnotationID);

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
            style->layers.emplace((style->layers.end() - 2), shapeLayer);

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

    cascadeClasses();

    updated |= static_cast<UpdateType>(Update::Classes);
    asyncUpdate->send();

    data.annotationManager.resetStaleTiles();
}

void MapContext::cascadeClasses() {
    style->cascade(data.getClasses());
}

void MapContext::update() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    const auto now = Clock::now();
    data.setAnimationTime(now);

    if (style) {
        if (updated & static_cast<UpdateType>(Update::DefaultTransitionDuration)) {
            style->setDefaultTransitionDuration(data.getDefaultTransitionDuration());
        }

        if (updated & static_cast<UpdateType>(Update::Classes)) {
            cascadeClasses();
        }

        if (updated & static_cast<UpdateType>(Update::Classes) ||
            updated & static_cast<UpdateType>(Update::Zoom)) {
            style->recalculate(transformState.getNormalizedZoom(), now);
        }

        style->update(data, transformState, *texturePool);

        if (data.mode == MapMode::Continuous) {
            view.invalidate();
        } else if (callback && style->isLoaded()) {
            renderSync(transformState);
        }
    }

    updated = static_cast<UpdateType>(Update::Nothing);
}

void MapContext::renderStill(const TransformState& state, StillImageCallback fn) {
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

    updated |= static_cast<UpdateType>(Update::RenderStill);
    asyncUpdate->send();
}

MapContext::RenderResult MapContext::renderSync(const TransformState& state) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    // Style was not loaded yet.
    if (!style) {
        return { false, false };
    }

    transformState = state;

    // Cleanup OpenGL objects that we abandoned since the last render call.
    glObjectStore.performCleanup();

    if (!painter) {
        painter = std::make_unique<Painter>();
        painter->setup();
    }

    painter->setDebug(data.getDebug());
    painter->render(*style, transformState, data.getAnimationTime());

    if (data.mode == MapMode::Still) {
        callback(nullptr, view.readStillImage());
        callback = nullptr;
    }

    view.swap();

    return RenderResult {
        style->isLoaded(),
        style->hasTransitions()
    };
}

bool MapContext::isLoaded() const {
    return style->isLoaded();
}

double MapContext::getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    const SpritePosition pos = style->sprite->getSpritePosition(symbol);
    return -pos.height / pos.pixelRatio / 2;
}

void MapContext::setSourceTileCacheSize(size_t size) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    if (size != sourceCacheSize) {
        sourceCacheSize = size;
        if (!style) return;
        for (const auto &source : style->sources) {
            source->setCacheSize(sourceCacheSize);
        }
        view.invalidate();
    }
}

void MapContext::onLowMemory() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    if (!style) return;
    for (const auto &source : style->sources) {
        source->onLowMemory();
    }
    view.invalidate();
}

void MapContext::onTileDataChanged() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    asyncUpdate->send();
}

void MapContext::onResourceLoadingFailed(std::exception_ptr error) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    if (data.mode == MapMode::Still && callback) {
        callback(error, nullptr);
        callback = nullptr;
    }
}

}
