#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/annotation/sprite_store.hpp>

#include <mbgl/platform/log.hpp>

#include <mbgl/renderer/painter.hpp>

#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/geometry/sprite_atlas.hpp>

#include <mbgl/style/style.hpp>

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
      asyncInvalidate(std::make_unique<uv::async>(util::RunLoop::getLoop(), [&view_] { view_.invalidate(); })),
      texturePool(std::make_unique<TexturePool>()) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    util::ThreadContext::setFileSource(&fileSource);
    util::ThreadContext::setGLObjectStore(&glObjectStore);

    asyncUpdate->unref();
    asyncInvalidate->unref();

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

    asyncInvalidate->send();
}

void MapContext::triggerUpdate(const TransformState& state, const Update flags) {
    transformState = state;
    updateFlags |= flags;

    asyncUpdate->send();
}

void MapContext::setStyleURL(const std::string& url) {
    if (styleURL == url) {
        return;
    }

    styleRequest = nullptr;
    styleURL = url;
    styleJSON.clear();

    style = std::make_unique<Style>(data);

    const size_t pos = styleURL.rfind('/');
    std::string base = "";
    if (pos != std::string::npos) {
        base = styleURL.substr(0, pos + 1);
    }

    FileSource* fs = util::ThreadContext::getFileSource();
    styleRequest = fs->request({ Resource::Kind::Style, styleURL }, util::RunLoop::getLoop(), [this, base](const Response &res) {
        if (res.stale) {
            // Only handle fresh responses.
            return;
        }
        styleRequest = nullptr;

        if (res.error) {
            if (res.error->reason == Response::Error::Reason::NotFound && styleURL.find("mapbox://") == 0) {
                Log::Error(Event::Setup, "style %s could not be found or is an incompatible legacy map or style", styleURL.c_str());
            } else {
                Log::Error(Event::Setup, "loading style failed: %s", res.error->message.c_str());
                data.loading = false;
            }
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

    // set loading here so we don't get a false loaded event as soon as map is
    // created but before a style is loaded
    data.loading = true;

    updateFlags |= Update::DefaultTransition | Update::Classes | Update::Zoom | Update::Annotations;
    asyncUpdate->send();
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

    if (style->sprite && updateFlags & Update::Annotations) {
        data.getAnnotationManager()->updateStyle(*style);
        updateFlags |= Update::Classes;
    }

    if (updateFlags & Update::Classes) {
        style->cascade();
    }

    if (updateFlags & Update::Classes || updateFlags & Update::Zoom) {
        style->recalculate(transformState.getNormalizedZoom());
    }

    style->update(transformState, *texturePool);

    if (data.mode == MapMode::Continuous) {
        asyncInvalidate->send();
    } else if (callback && style->isLoaded()) {
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

    if (!painter) painter = std::make_unique<Painter>(data);
    painter->render(*style, transformState, frame);

    if (data.mode == MapMode::Still) {
        callback(nullptr, view.readStillImage());
        callback = nullptr;
    }

    view.afterRender();

    if (style->hasTransitions()) {
        updateFlags |= Update::Classes;
        asyncUpdate->send();
    }

    if (painter->needsAnimation()) {
        updateFlags |= Update::Repaint;
        asyncUpdate->send();
    }

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
        asyncInvalidate->send();
    }
}

void MapContext::onLowMemory() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    if (!style) return;
    for (const auto &source : style->sources) {
        source->onLowMemory();
    }
    asyncInvalidate->send();
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

}
