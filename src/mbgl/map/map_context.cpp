#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/still_image.hpp>

#include <mbgl/platform/log.hpp>

#include <mbgl/renderer/painter.hpp>

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/style/style.hpp>

#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/util/exception.hpp>

namespace mbgl {

MapContext::MapContext(uv_loop_t* loop, View& view_, FileSource& fileSource, MapData& data_)
    : view(view_),
      data(data_),
      env(fileSource),
      envScope(env, ThreadType::Map, "Map"),
      updated(static_cast<UpdateType>(Update::Nothing)),
      asyncUpdate(std::make_unique<uv::async>(loop, [this] { update(); })),
      texturePool(std::make_unique<TexturePool>()) {
    assert(Environment::currentlyOn(ThreadType::Map));

    asyncUpdate->unref();

    view.activate();
}

MapContext::~MapContext() {
    view.notify();

    // Explicit resets currently necessary because these abandon resources that need to be
    // cleaned up by env.performCleanup();
    style.reset();
    painter.reset();
    texturePool.reset();

    env.performCleanup();

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
    triggerUpdate();
}

void MapContext::triggerUpdate(const Update u) {
    updated |= static_cast<UpdateType>(u);
    asyncUpdate->send();
}

void MapContext::setStyleURL(const std::string& url) {
    styleURL = url;
    styleJSON.clear();

    const size_t pos = styleURL.rfind('/');
    std::string base = "";
    if (pos != std::string::npos) {
        base = styleURL.substr(0, pos + 1);
    }

    env.request({ Resource::Kind::Style, styleURL }, [this, base](const Response &res) {
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
    assert(Environment::currentlyOn(ThreadType::Map));

    style.reset();
    style = std::make_unique<Style>(json, base, asyncUpdate->get()->loop, env);
    style->cascade(data.getClasses());
    style->setDefaultTransitionDuration(data.getDefaultTransitionDuration());
    style->setObserver(this);

    triggerUpdate(Update::Zoom);
}

void MapContext::updateTiles() {
    assert(Environment::currentlyOn(ThreadType::Map));

    style->update(data, transformState, *texturePool);
}

void MapContext::updateAnnotationTiles(const std::vector<TileID>& ids) {
    assert(Environment::currentlyOn(ThreadType::Map));
    if (!style) return;
    for (const auto &source : style->sources) {
        if (source->info.type == SourceType::Annotations) {
            source->invalidateTiles(ids);
        }
    }
    triggerUpdate();
}

void MapContext::update() {
    assert(Environment::currentlyOn(ThreadType::Map));

    const auto now = Clock::now();
    data.setAnimationTime(now);

    updated |= data.transform.updateTransitions(now);
    transformState = data.transform.currentState();

    if (style) {
        if (updated & static_cast<UpdateType>(Update::DefaultTransitionDuration)) {
            style->setDefaultTransitionDuration(data.getDefaultTransitionDuration());
        }

        if (updated & static_cast<UpdateType>(Update::Classes)) {
            style->cascade(data.getClasses());
        }

        if (updated & static_cast<UpdateType>(Update::Classes) ||
            updated & static_cast<UpdateType>(Update::Zoom)) {
            style->recalculate(transformState.getNormalizedZoom(), now);
        }

        updateTiles();

        if (style->isLoaded()) {
            if (!data.getFullyLoaded()) {
                data.setFullyLoaded(true);
            }
        } else {
            if (data.getFullyLoaded()) {
                data.setFullyLoaded(false);
            }
        }

        view.invalidate([this] { render(); });
    }

    updated = static_cast<UpdateType>(Update::Nothing);
}

void MapContext::renderStill(StillImageCallback fn) {
    if (data.mode != MapMode::Still) {
        throw util::Exception("Map is not in still image render mode");
    }

    if (callback) {
        throw util::Exception("Map is currently rendering an image");
    }

    callback = fn;
    triggerUpdate(Update::RenderStill);
}

void MapContext::render() {
    assert(Environment::currentlyOn(ThreadType::Map));

    // Cleanup OpenGL objects that we abandoned since the last render call.
    env.performCleanup();

    if (data.mode == MapMode::Still && (!callback || !data.getFullyLoaded())) {
        // We are either not waiting for a map to be rendered, or we don't have all resources yet.
        return;
    }

    assert(style);

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

    // Schedule another rerender when we definitely need a next frame.
    if (data.transform.needsTransition() || style->hasTransitions()) {
        triggerUpdate();
    }
}

double MapContext::getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol) {
    assert(Environment::currentlyOn(ThreadType::Map));
    const SpritePosition pos = style->sprite->getSpritePosition(symbol);
    return -pos.height / pos.pixelRatio / 2;
}

void MapContext::setSourceTileCacheSize(size_t size) {
    assert(Environment::currentlyOn(ThreadType::Map));
    if (size != sourceCacheSize) {
        sourceCacheSize = size;
        if (!style) return;
        for (const auto &source : style->sources) {
            source->setCacheSize(sourceCacheSize);
        }
        view.invalidate([this] { render(); });
    }
}

void MapContext::onLowMemory() {
    assert(Environment::currentlyOn(ThreadType::Map));
    if (!style) return;
    for (const auto &source : style->sources) {
        source->onLowMemory();
    }
    view.invalidate([this] { render(); });
}

void MapContext::onTileDataChanged() {
    assert(Environment::currentlyOn(ThreadType::Map));
    triggerUpdate();
}

void MapContext::onResourceLoadingFailed(std::exception_ptr error) {
    assert(Environment::currentlyOn(ThreadType::Map));

    if (data.mode == MapMode::Still && callback) {
        callback(error, nullptr);
        callback = nullptr;
    }
}

}
