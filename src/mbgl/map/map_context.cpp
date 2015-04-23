#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/map/still_image.hpp>

#include <mbgl/platform/log.hpp>

#include <mbgl/renderer/painter.hpp>

#include <mbgl/text/glyph_store.hpp>

#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>

#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/style/style.hpp>

#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/exception.hpp>

namespace mbgl {

MapContext::MapContext(uv_loop_t* loop, Environment& env_, View& view_, MapData& data_, bool startPaused)
    : env(env_),
      view(view_),
      data(data_),
      mapScope(env, ThreadType::Map, "Map"),
      updated(static_cast<UpdateType>(Update::Nothing)),
      asyncUpdate(util::make_unique<uv::async>(loop, [this] { update(); })),
      workers(util::make_unique<Worker>(loop, 4)),
      glyphStore(util::make_unique<GlyphStore>(env)),
      glyphAtlas(util::make_unique<GlyphAtlas>(1024, 1024)),
      spriteAtlas(util::make_unique<SpriteAtlas>(512, 512)),
      lineAtlas(util::make_unique<LineAtlas>(512, 512)),
      texturePool(util::make_unique<TexturePool>()),
      painter(util::make_unique<Painter>(*spriteAtlas, *glyphAtlas, *lineAtlas))
{
    assert(Environment::currentlyOn(ThreadType::Map));

    asyncUpdate->unref();

    view.activate();

    if (startPaused) {
        pause();
    }

    painter->setup();
}

MapContext::~MapContext() {
    view.deactivate();
    view.notify();

    // Explicit resets currently necessary because these abandon resources that need to be
    // cleaned up by env.performCleanup();
    style.reset();
    sprite.reset();
    painter.reset();
    texturePool.reset();
    lineAtlas.reset();
    spriteAtlas.reset();
    glyphAtlas.reset();
    glyphStore.reset();

    env.performCleanup();
}

void MapContext::pause() {
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
    styleURL = mbgl::util::mapbox::normalizeStyleURL(url, data.getAccessToken());
    styleJSON.clear();

    const size_t pos = styleURL.rfind('/');
    std::string base = "";
    if (pos != std::string::npos) {
        base = styleURL.substr(0, pos + 1);
    }

    env.request({ Resource::Kind::JSON, styleURL }, [this, base](const Response &res) {
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

util::ptr<Sprite> MapContext::getSprite() {
    assert(Environment::currentlyOn(ThreadType::Map));
    const float pixelRatio = transformState.getPixelRatio();
    const std::string &sprite_url = style->getSpriteURL();
    if (!sprite || !sprite->hasPixelRatio(pixelRatio)) {
        sprite = std::make_shared<Sprite>(sprite_url, pixelRatio, env, [this] {
            assert(Environment::currentlyOn(ThreadType::Map));
            triggerUpdate();
        });
    }

    return sprite;
}

void MapContext::loadStyleJSON(const std::string& json, const std::string& base) {
    assert(Environment::currentlyOn(ThreadType::Map));

    sprite.reset();
    style = std::make_shared<Style>();
    style->base = base;
    style->loadJSON((const uint8_t *)json.c_str());
    style->cascade(data.getClasses());
    style->setDefaultTransitionDuration(data.getDefaultTransitionDuration());

    const std::string glyphURL = util::mapbox::normalizeGlyphsURL(style->glyph_url, data.getAccessToken());
    glyphStore->setURL(glyphURL);

    for (const auto& source : style->sources) {
        source->load(data.getAccessToken(), env, [this]() {
            assert(Environment::currentlyOn(ThreadType::Map));
            triggerUpdate();
        });
    }

    triggerUpdate(Update::Zoom);
}

void MapContext::updateTiles() {
    assert(Environment::currentlyOn(ThreadType::Map));
    if (!style) return;
    for (const auto& source : style->sources) {
        source->update(data, transformState, *workers, style, *glyphAtlas, *glyphStore, *spriteAtlas,
                       getSprite(), *texturePool, [this]() {
            assert(Environment::currentlyOn(ThreadType::Map));
            triggerUpdate();
        });
    }
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

    auto u = updated.exchange(static_cast<UpdateType>(Update::Nothing)) |
             data.transform.updateTransitions(now);

    transformState = data.transform.currentState();

    if (u & static_cast<UpdateType>(Update::Debug)) {
        assert(painter);
        painter->setDebug(data.getDebug());
    }

    if (style) {
        if (u & static_cast<UpdateType>(Update::DefaultTransitionDuration)) {
            style->setDefaultTransitionDuration(data.getDefaultTransitionDuration());
        }

        if (u & static_cast<UpdateType>(Update::Classes)) {
            style->cascade(data.getClasses());
        }

        if (u & static_cast<UpdateType>(Update::Classes) ||
            u & static_cast<UpdateType>(Update::Zoom)) {
            style->recalculate(transformState.getNormalizedZoom(), now);
        }

        // Allow the sprite atlas to potentially pull new sprite images if needed.
        spriteAtlas->resize(transformState.getPixelRatio());
        spriteAtlas->setSprite(getSprite());

        updateTiles();

        view.invalidate([this] { render(); });
    }
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

    assert(style);
    assert(painter);

    painter->render(*style, transformState, data.getAnimationTime());

    if (data.mode == MapMode::Still && callback && style->isLoaded() && getSprite()->isLoaded()) {
        callback(view.readStillImage());
        callback = nullptr;
    }

    // Schedule another rerender when we definitely need a next frame.
    if (data.transform.needsTransition() || style->hasTransitions()) {
        triggerUpdate();
    }
}

double MapContext::getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol) {
    assert(Environment::currentlyOn(ThreadType::Map));
    assert(sprite);
    const SpritePosition pos = sprite->getSpritePosition(symbol);
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
        env.performCleanup();
    }
}

void MapContext::onLowMemory() {
    assert(Environment::currentlyOn(ThreadType::Map));
    if (!style) return;
    for (const auto &source : style->sources) {
        source->onLowMemory();
    }
    env.performCleanup();
}

}
