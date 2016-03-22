#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/view.hpp>

#include <mbgl/platform/log.hpp>

#include <mbgl/renderer/painter.hpp>

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/property_transition.hpp>

#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_store.hpp>

#include <mbgl/gl/gl_object_store.hpp>
#include <mbgl/gl/texture_pool.hpp>

#include <mbgl/util/worker.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/mapbox.hpp>

#include <algorithm>

namespace mbgl {

MapContext::MapContext(View& view_, FileSource& fileSource_, MapMode mode_, GLContextMode contextMode_, const float pixelRatio_)
    : view(view_),
      fileSource(fileSource_),
      dataPtr(std::make_unique<MapData>(mode_, contextMode_, pixelRatio_)),
      data(*dataPtr),
      asyncUpdate([this] { update(); }),
      asyncInvalidate([&view_] { view_.invalidate(); }),
      texturePool(std::make_unique<gl::TexturePool>()) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    view.activate();
}

MapContext::~MapContext() {
    // Make sure we call cleanup() before deleting this object.
    assert(!style);
}

void MapContext::cleanup() {
    view.notify();

    styleRequest = nullptr;

    // Explicit resets currently necessary because these abandon resources that need to be
    // cleaned up by glObjectStore.performCleanup();
    style.reset();
    painter.reset();
    texturePool.reset();
    dataPtr.reset();

    glObjectStore.performCleanup();

    view.deactivate();
}

void MapContext::pause() {
    MBGL_CHECK_ERROR(glFinish());

    view.deactivate();

    std::unique_lock<std::mutex> lockPause(data.mutexPause);
    data.paused = true;
    data.condPause.notify_all();
    data.condPause.wait(lockPause, [&]{ return !data.paused; });

    view.activate();

    asyncInvalidate.send();
}

void MapContext::updateAsync(Update flags) {
    updateFlags |= flags;
    asyncUpdate.send();
}

void MapContext::triggerUpdate(const TransformState& state, Update flags) {
    transformState = state;
    updateAsync(flags);
}

void MapContext::setStyleURL(const std::string& url) {
    if (styleURL == url) {
        return;
    }

    styleRequest = nullptr;
    styleURL = url;
    styleJSON.clear();

    style = std::make_unique<Style>(data, fileSource);

    const size_t pos = styleURL.rfind('/');
    std::string base = "";
    if (pos != std::string::npos) {
        base = styleURL.substr(0, pos + 1);
    }

    styleRequest = fileSource.request(Resource::style(styleURL), [this, base](Response res) {
        if (res.error) {
            if (res.error->reason == Response::Error::Reason::NotFound &&
                util::mapbox::isMapboxURL(styleURL)) {
                Log::Error(Event::Setup, "style %s could not be found or is an incompatible legacy map or style", styleURL.c_str());
            } else {
                Log::Error(Event::Setup, "loading style failed: %s", res.error->message.c_str());
                data.loading = false;
            }
        } else if (res.notModified || res.noContent) {
            return;
        } else {
            loadStyleJSON(*res.data, base);
        }
    });
}

void MapContext::setStyleJSON(const std::string& json, const std::string& base) {
    if (styleJSON == json) {
        return;
    }

    styleURL.clear();
    styleJSON.clear();

    style = std::make_unique<Style>(data, fileSource);

    loadStyleJSON(json, base);
}

void MapContext::loadStyleJSON(const std::string& json, const std::string& base) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    style->setJSON(json, base);
    style->setObserver(this);
    styleJSON = json;

    // force style cascade, causing all pending transitions to complete.
    style->cascade(Clock::now());

    // set loading here so we don't get a false loaded event as soon as map is
    // created but before a style is loaded
    data.loading = true;

    updateAsync(Update::Classes | Update::RecalculateStyle | Update::Annotations);
}

void MapContext::update() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    if (!style) {
        updateFlags = Update::Nothing;
    }

    if (updateFlags == Update::Nothing || (data.mode == MapMode::Still && !callback)) {
        return;
    }

    if (style->loaded && updateFlags & Update::Annotations) {
        data.getAnnotationManager()->updateStyle(*style);
        updateFlags |= Update::Classes;
    }

    if (updateFlags & Update::Classes) {
        style->cascade(frameData.timePoint);
    }

    if (updateFlags & Update::Classes || updateFlags & Update::RecalculateStyle) {
        style->recalculate(transformState.getZoom(), frameData.timePoint);
    }

    style->update(transformState, frameData.timePoint, *texturePool);

    if (data.mode == MapMode::Continuous) {
        asyncInvalidate.send();
    } else {
        // Update time point so style sources can check they are loaded.
        frameData.timePoint = Clock::now();
        if (callback && style->isLoaded()) {
            renderSync(transformState, frameData);
        }
    }

    updateFlags = Update::Nothing;
}

void MapContext::renderStill(const TransformState& state, const FrameData& frame, Map::StillImageCallback fn) {
    if (!fn) {
        Log::Error(Event::General, "StillImageCallback not set");
        return;
    }

    if (data.mode != MapMode::Still) {
        fn(std::make_exception_ptr(util::MisuseException("Map is not in still image render mode")), {});
        return;
    }

    if (callback) {
        fn(std::make_exception_ptr(util::MisuseException("Map is currently rendering an image")), {});
        return;
    }

    if (!style) {
        fn(std::make_exception_ptr(util::MisuseException("Map doesn't have a style")), {});
        return;
    }

    if (style->getLastError()) {
        fn(style->getLastError(), {});
        return;
    }

    callback = fn;
    transformState = state;
    frameData = frame;

    updateAsync(Update::RenderStill);
}

bool MapContext::renderSync(const TransformState& state, const FrameData& frame) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    // Style was not loaded yet.
    if (!style) {
        return false;
    }

    view.beforeRender();

    transformState = state;
    frameData = frame;

    if (!painter) painter = std::make_unique<Painter>(data, transformState, glObjectStore);
    painter->render(*style, frame, data.getAnnotationManager()->getSpriteAtlas());

    if (data.mode == MapMode::Still) {
        callback(nullptr, view.readStillImage());
        callback = nullptr;
    }

    // Cleanup OpenGL objects that we abandoned since the last render call.
    glObjectStore.performCleanup();

    view.afterRender();

    if (style->hasTransitions()) {
        updateAsync(Update::RecalculateStyle);
    } else if (painter->needsAnimation()) {
        updateAsync(Update::Repaint);
    }

    return isLoaded();
}

bool MapContext::isLoaded() const {
    return style->isLoaded();
}

void MapContext::addAnnotationIcon(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    data.getAnnotationManager()->addIcon(name, sprite);
}
    
void MapContext::removeAnnotationIcon(const std::string& name) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    data.getAnnotationManager()->removeIcon(name);
}

double MapContext::getTopOffsetPixelsForAnnotationIcon(const std::string& name) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    return data.getAnnotationManager()->getTopOffsetPixelsForIcon(name);
}

void MapContext::addLayer(std::unique_ptr<StyleLayer> layer, optional<std::string> after) {
    style->addLayer(std::move(layer), after);
    updateAsync(Update::Classes);
}

void MapContext::removeLayer(const std::string& id) {
    style->removeLayer(id);
    updateAsync(Update::Classes);
}

std::vector<std::string> MapContext::getClasses() const {
    return style->getClasses();
}

bool MapContext::hasClass(const std::string& className) const {
    return style->hasClass(className);
}

void MapContext::addClass(const std::string& className, optional<PropertyTransition> properties) {
    if (style->addClass(className, properties)) updateAsync(Update::Classes);
}

void MapContext::removeClass(const std::string& className, optional<PropertyTransition> properties) {
    if (style->removeClass(className, properties)) updateAsync(Update::Classes);
}

void MapContext::setClasses(const std::vector<std::string>& classNames, optional<PropertyTransition> properties) {
    style->setClasses(classNames, properties);
    updateAsync(Update::Classes);
}

void MapContext::setSourceTileCacheSize(size_t size) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    if (size != sourceCacheSize) {
        sourceCacheSize = size;
        if (!style) return;
        style->setSourceTileCacheSize(size);
        asyncInvalidate.send();
    }
}

void MapContext::onLowMemory() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    if (!style) return;
    style->onLowMemory();
    asyncInvalidate.send();
}

void MapContext::onResourceLoaded() {
    updateAsync(Update::Repaint);
}

void MapContext::onResourceError(std::exception_ptr error) {
    if (data.mode == MapMode::Still && callback) {
        callback(error, {});
        callback = nullptr;
    }
}

void MapContext::dumpDebugLogs() const {
    Log::Info(Event::General, "--------------------------------------------------------------------------------");
    Log::Info(Event::General, "MapContext::styleURL: %s", styleURL.c_str());
    if (style) {
        style->dumpDebugLogs();
    } else {
        Log::Info(Event::General, "no style loaded");
    }
    Log::Info(Event::General, "--------------------------------------------------------------------------------");
}

} // namespace mbgl
