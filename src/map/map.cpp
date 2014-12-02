#include <mbgl/map/map.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/util/transition.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/clip_ids.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/util/texturepool.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/string.hpp>

#include <algorithm>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include <uv.h>

#ifdef __ANDROID__
    #include <coffeecatch/coffeecatch.h>
#endif

// Check libuv library version.
const static bool uv_version_check = []() {
    const unsigned int version = uv_version();
    const unsigned int major = (version >> 16) & 0xFF;
    const unsigned int minor = (version >> 8) & 0xFF;
    const unsigned int patch = version & 0xFF;

#ifndef UV_VERSION_PATCH
    // 0.10 doesn't have UV_VERSION_PATCH defined, so we "fake" it by using the library patch level.
    const unsigned int UV_VERSION_PATCH = version & 0xFF;
#endif

    if (major != UV_VERSION_MAJOR || minor != UV_VERSION_MINOR || patch != UV_VERSION_PATCH) {
        throw std::runtime_error(mbgl::util::sprintf<96>(
            "libuv version mismatch: headers report %d.%d.%d, but library reports %d.%d.%d", UV_VERSION_MAJOR,
            UV_VERSION_MINOR, UV_VERSION_PATCH, major, minor, patch));
    }
    return true;
}();


#include <zlib.h>
// Check zlib library version.
const static bool zlib_version_check = []() {
    const char *const version = zlibVersion();
    if (version[0] != ZLIB_VERSION[0]) {
        throw std::runtime_error(mbgl::util::sprintf<96>(
            "zlib version mismatch: headers report %s, but library reports %s", ZLIB_VERSION, version));
    }

    return true;
}();


#include <sqlite3.h>
// Check sqlite3 library version.
const static bool sqlite_version_check = []() {
    if (sqlite3_libversion_number() != SQLITE_VERSION_NUMBER) {
        throw std::runtime_error(mbgl::util::sprintf<96>(
            "sqlite3 libversion mismatch: headers report %d, but library reports %d",
            SQLITE_VERSION_NUMBER, sqlite3_libversion_number()));
    }
    if (strcmp(sqlite3_sourceid(), SQLITE_SOURCE_ID) != 0) {
        throw std::runtime_error(mbgl::util::sprintf<256>(
            "sqlite3 sourceid mismatch: headers report \"%s\", but library reports \"%s\"",
            SQLITE_SOURCE_ID, sqlite3_sourceid()));
    }

    return true;
}();


using namespace mbgl;

Map::Map(View& view_)
    : loop(std::make_unique<uv::loop>()),
      thread(std::make_unique<uv::thread>()),
      view(view_),
#ifndef NDEBUG
      main_thread(uv_thread_self()),
#endif
      transform(view_),
      glyphAtlas(1024, 1024),
      spriteAtlas(512, 512),
      texturepool(std::make_shared<Texturepool>()),
      painter(spriteAtlas, glyphAtlas)
{
    view.initialize(this);
    // Make sure that we're doing an initial drawing in all cases.
    is_clean.clear();
    is_rendered.clear();
    is_swapped.test_and_set();
}

Map::~Map() {
    if (async) {
        stop();
    }

    // Explicitly reset all pointers.
    activeSources.clear();
    sprite.reset();
    glyphStore.reset();
    style.reset();
    texturepool.reset();
    fileSource.reset();
    workers.reset();

    uv_run(**loop, UV_RUN_DEFAULT);
}

uv::worker &Map::getWorker() {
    if (!workers) {
        workers = std::make_unique<uv::worker>(**loop, 4, "Tile Worker");
    }
    return *workers;
}

void Map::start(bool start_paused) {
    assert(uv_thread_self() == main_thread);
    assert(!async);

    // When starting map rendering in another thread, we perform async/continuously
    // updated rendering. Only in these cases, we attach the async handlers.
    async = true;

    // Reset the flag.
    is_stopped = false;

    // Setup async notifications
    async_terminate = std::make_unique<uv::async>(**loop, [this]() {
        assert(uv_thread_self() == map_thread);

        // Remove all of these to make sure they are destructed in the correct thread.
        glyphStore.reset();
        fileSource.reset();
        style.reset();
        workers.reset();
        activeSources.clear();

        terminating = true;

        // Closes all open handles on the loop. This means that the loop will automatically terminate.
        async_cleanup.reset();
        async_render.reset();
        async_terminate.reset();
    });

    async_render = std::make_unique<uv::async>(**loop, [this]() {
        assert(uv_thread_self() == map_thread);

        if (state.hasSize()) {
            if (is_rendered.test_and_set() == false) {
                prepare();
                if (is_clean.test_and_set() == false) {
                    render();
                    is_swapped.clear();
                    view.swap();
                } else {
                    // We set the rendered flag in the test above, so we have to reset it
                    // now that we're not actually rendering because the map is clean.
                    is_rendered.clear();
                }
            }
        }
    });

    async_cleanup = std::make_unique<uv::async>(**loop, [this]() {
        painter.cleanup();
    });

    // Do we need to pause first?
    if (start_paused) {
        pause();
    }

    uv_thread_create(*thread, [](void *arg) {
        Map *map = static_cast<Map *>(arg);
#ifndef NDEBUG
        map->map_thread = uv_thread_self();
#endif
#ifdef __APPLE__
        pthread_setname_np("Map");
#endif
        map->run();
#ifndef NDEBUG
        map->map_thread = -1;
#endif

        // Make sure that the stop() function knows when to stop invoking the callback function.
        map->is_stopped = true;
        map->view.notify();
    }, this);
}

void Map::stop(stop_callback cb, void *data) {
    assert(uv_thread_self() == main_thread);
    assert(main_thread != map_thread);
    assert(async);

    async_terminate->send();

    resume();

    if (cb) {
        // Wait until the render thread stopped. We are using this construct instead of plainly
        // relying on the thread_join because the system might need to run things in the current
        // thread that is required for the render thread to terminate correctly. This is for example
        // the case with Cocoa's NSURLRequest. Otherwise, we will eventually deadlock because this
        // thread (== main thread) is blocked. The callback function should use an efficient waiting
        // function to avoid a busy waiting loop.
        while (!is_stopped) {
            cb(data);
        }
    }

    // If a callback function was provided, this should return immediately because the thread has
    // already finished executing.
    uv_thread_join(*thread);

    async = false;
}

void Map::pause(bool wait_for_pause) {
    assert(uv_thread_self() == main_thread);
    assert(async);
    mutex_run.lock();
    pausing = true;
    mutex_run.unlock();

    uv_stop(**loop);
    rerender(); // Needed to ensure uv_stop is seen and uv_run exits, otherwise we deadlock on wait_for_pause

    if (wait_for_pause) {
        std::unique_lock<std::mutex> lock_pause (mutex_pause);
        while (!is_paused) {
            cond_pause.wait(lock_pause);
        }
    }
}

void Map::resume() {
    assert(uv_thread_self() == main_thread);
    assert(async);

    mutex_run.lock();
    pausing = false;
    cond_run.notify_all();
    mutex_run.unlock();
}

void Map::run() {
#ifdef __ANDROID__
    COFFEE_TRY() {
#endif

#ifndef NDEBUG
    if (!async) {
        map_thread = main_thread;
    }
#endif
    assert(uv_thread_self() == map_thread);

    if (async) {
        check_for_pause();
    }

    setup();
    prepare();

    if (async) {
        terminating = false;
        while(!terminating) {
            uv_run(**loop, UV_RUN_DEFAULT);
            check_for_pause();
        }
    } else {
        uv_run(**loop, UV_RUN_DEFAULT);
    }

    // Run the event loop once more to make sure our async delete handlers are called.
    uv_run(**loop, UV_RUN_ONCE);

    // If the map rendering wasn't started asynchronously, we perform one render
    // *after* all events have been processed.
    if (!async) {
        render();
#ifndef NDEBUG
        map_thread = -1;
#endif
    }
#ifdef __ANDROID__
    } COFFEE_CATCH() {
        Log::Error(Event::Crash, "Map::run() crash:\n%s", coffeecatch_get_message());
        abort();
    }
#endif
}

void Map::check_for_pause() {
    std::unique_lock<std::mutex> lock_run (mutex_run);
    while (pausing) {
        view.make_inactive();

        mutex_pause.lock();
        is_paused = true;
        cond_pause.notify_all();
        mutex_pause.unlock();

        cond_run.wait(lock_run);

        view.make_active();
    }

    mutex_pause.lock();
    is_paused = false;
    mutex_pause.unlock();
}

void Map::rerender() {
    // We only send render events if we want to continuously update the map
    // (== async rendering).
    if (async) {
        async_render->send();
    }
}

void Map::update() {
    is_clean.clear();
    rerender();
}

bool Map::needsSwap() {
    return is_swapped.test_and_set() == false;
}

void Map::swapped() {
    is_rendered.clear();
    rerender();
}

void Map::cleanup() {
    if (async_cleanup != nullptr) {
        async_cleanup->send();
    }
}

void Map::terminate() {
    painter.terminate();
}

void Map::setReachability(bool reachable) {
    // Note: This function may be called from *any* thread.
    if (reachable) {
        if (fileSource) {
            fileSource->prepare([&]() {
                fileSource->retryAllPending();
            });
        }
    }
}

#pragma mark - Setup

void Map::setup() {
    assert(uv_thread_self() == map_thread);
    view.make_active();
    painter.setup();
}

void Map::setStyleURL(const std::string &url) {
    // TODO: Make threadsafe.

    // TODO fix this properly
    styleURL = url;
    if (async) {
        stop();
        start();
    }
}


void Map::setStyleJSON(std::string newStyleJSON, const std::string &base) {
    // TODO: Make threadsafe.
    styleJSON.swap(newStyleJSON);
    sprite.reset();
    if (!style) {
        style = std::make_shared<Style>();
    }

    style->loadJSON((const uint8_t *)styleJSON.c_str());
    if (!fileSource) {
        fileSource = std::make_shared<FileSource>(**loop, platform::defaultCacheDatabase());
        glyphStore = std::make_shared<GlyphStore>(*fileSource);
    }
    fileSource->setBase(base);
    glyphStore->setURL(util::mapbox::normalizeGlyphsURL(style->glyph_url, getAccessToken()));

    style->setDefaultTransitionDuration(defaultTransitionDuration);

    // set applied classes if they were set while the style was loading
    appliedClassesMutex.lock();
    util::ptr<std::vector<std::string>> classes = appliedClasses;
    if (appliedClasses) {
        appliedClasses.reset();
    }
    appliedClassesMutex.unlock();
    if (classes) {
        style->setAppliedClasses(*classes);
    }

    update();
}

std::string Map::getStyleJSON() const {
    return styleJSON;
}

void Map::setAccessToken(std::string access_token) {
    // TODO: Make threadsafe.
    accessToken.swap(access_token);
}

std::string Map::getAccessToken() const {
    return accessToken;
}

util::ptr<Sprite> Map::getSprite() {
    const float pixelRatio = state.getPixelRatio();
    const std::string &sprite_url = style->getSpriteURL();
    if (!sprite || sprite->pixelRatio != pixelRatio) {
        sprite = Sprite::Create(sprite_url, pixelRatio, *fileSource);
    }

    return sprite;
}


#pragma mark - Size

void Map::resize(uint16_t width, uint16_t height, float ratio) {
    resize(width, height, ratio, width * ratio, height * ratio);
}

void Map::resize(uint16_t width, uint16_t height, float ratio, uint16_t fb_width, uint16_t fb_height) {
    if (transform.resize(width, height, ratio, fb_width, fb_height)) {
        update();
    }
}

#pragma mark - Transitions

void Map::cancelTransitions() {
    transform.cancelTransitions();

    update();
}


#pragma mark - Position

void Map::moveBy(double dx, double dy, double duration) {
    transform.moveBy(dx, dy, duration * 1_second);
    update();
}

void Map::setLonLat(double lon, double lat, double duration) {
    transform.setLonLat(lon, lat, duration * 1_second);
    update();
}

void Map::getLonLat(double& lon, double& lat) const {
    transform.getLonLat(lon, lat);
}

void Map::startPanning() {
    transform.startPanning();
    update();
}

void Map::stopPanning() {
    transform.stopPanning();
    update();
}

void Map::resetPosition() {
    transform.setAngle(0);
    transform.setLonLat(0, 0);
    transform.setZoom(0);
    update();
}


#pragma mark - Scale

void Map::scaleBy(double ds, double cx, double cy, double duration) {
    transform.scaleBy(ds, cx, cy, duration * 1_second);
    update();
}

void Map::setScale(double scale, double cx, double cy, double duration) {
    transform.setScale(scale, cx, cy, duration * 1_second);
    update();
}

double Map::getScale() const {
    return transform.getScale();
}

void Map::setZoom(double zoom, double duration) {
    transform.setZoom(zoom, duration * 1_second);
    update();
}

double Map::getZoom() const {
    return transform.getZoom();
}

void Map::setLonLatZoom(double lon, double lat, double zoom, double duration) {
    transform.setLonLatZoom(lon, lat, zoom, duration * 1_second);
    update();
}

void Map::getLonLatZoom(double& lon, double& lat, double& zoom) const {
    transform.getLonLatZoom(lon, lat, zoom);
}

void Map::resetZoom() {
    setZoom(0);
}

void Map::startScaling() {
    transform.startScaling();
    update();
}

void Map::stopScaling() {
    transform.stopScaling();
    update();
}

double Map::getMinZoom() const {
    return transform.getMinZoom();
}

double Map::getMaxZoom() const {
    return transform.getMaxZoom();
}


#pragma mark - Rotation

void Map::rotateBy(double sx, double sy, double ex, double ey, double duration) {
    transform.rotateBy(sx, sy, ex, ey, duration * 1_second);
    update();
}

void Map::setBearing(double degrees, double duration) {
    transform.setAngle(-degrees * M_PI / 180, duration * 1_second);
    update();
}

void Map::setBearing(double degrees, double cx, double cy) {
    transform.setAngle(-degrees * M_PI / 180, cx, cy);
    update();
}

double Map::getBearing() const {
    return -transform.getAngle() / M_PI * 180;
}

void Map::resetNorth() {
    transform.setAngle(0, 500_milliseconds);
    update();
}

void Map::startRotating() {
    transform.startRotating();
    update();
}

void Map::stopRotating() {
    transform.stopRotating();
    update();
}


#pragma mark - Toggles

void Map::setDebug(bool value) {
    debug = value;
    painter.setDebug(debug);
    update();
}

void Map::toggleDebug() {
    setDebug(!debug);
}

bool Map::getDebug() const {
    return debug;
}

void Map::setAppliedClasses(const std::vector<std::string> &classes) {
    if (style) {
        style->setAppliedClasses(classes);
        if (style->hasTransitions()) {
            update();
        }
    }
    else {
        appliedClassesMutex.lock();
        appliedClasses = std::make_unique<std::vector<std::string>>(classes);
        appliedClassesMutex.unlock();
    }
}


void Map::toggleClass(const std::string &name) {
    style->toggleClass(name);
    if (style->hasTransitions()) {
        update();
    }
}

const std::vector<std::string> &Map::getAppliedClasses() const {
   return style->getAppliedClasses();
}

void Map::setDefaultTransitionDuration(uint64_t duration_milliseconds) {
    defaultTransitionDuration = duration_milliseconds;
    if (style) {
        style->setDefaultTransitionDuration(duration_milliseconds);
    }
}

uint64_t Map::getDefaultTransitionDuration() {
    return defaultTransitionDuration;
}

void Map::updateSources() {
    assert(uv_thread_self() == map_thread);

    // First, disable all existing sources.
    for (const auto& source : activeSources) {
        source->enabled = false;
    }

    // Then, reenable all of those that we actually use when drawing this layer.
    updateSources(style->layers);

    // Then, construct or destroy the actual source object, depending on enabled state.
    for (const auto& style_source : activeSources) {
        if (style_source->enabled) {
            if (!style_source->source) {
                style_source->source = std::make_shared<Source>(style_source->info);
                style_source->source->load(*this, *fileSource);
            }
        } else {
            style_source->source.reset();
        }
    }

    // Finally, remove all sources that are disabled.
    util::erase_if(activeSources, [](util::ptr<StyleSource> source){
        return !source->enabled;
    });
}

void Map::updateSources(const util::ptr<StyleLayerGroup> &group) {
    if (!group) {
        return;
    }
    for (const util::ptr<StyleLayer> &layer : group->layers) {
        if (!layer) continue;
        if (layer->bucket) {
            if (layer->bucket->style_source) {
                (*activeSources.emplace(layer->bucket->style_source).first)->enabled = true;
            }
        } else if (layer->layers) {
            updateSources(layer->layers);
        }
    }
}

void Map::updateTiles() {
    for (const auto& source : activeSources) {
        source->source->update(*this, getWorker(),
                               style, glyphAtlas, *glyphStore,
                               spriteAtlas, getSprite(),
                               *texturepool, *fileSource, [this](){ update(); });
    }
}

void Map::prepare() {
    if (!fileSource) {
        fileSource = std::make_shared<FileSource>(**loop, platform::defaultCacheDatabase());
        glyphStore = std::make_shared<GlyphStore>(*fileSource);
    }

    if (!style) {
        style = std::make_shared<Style>();

        fileSource->request(ResourceType::JSON, styleURL)->onload([&](const Response &res) {
            if (res.code == 200) {
                // Calculate the base
                const size_t pos = styleURL.rfind('/');
                std::string base = "";
                if (pos != std::string::npos) {
                    base = styleURL.substr(0, pos + 1);
                }

                setStyleJSON(res.data, base);
            } else {
                Log::Error(Event::Setup, "loading style failed: %ld (%s)", res.code, res.message.c_str());
            }
        });
    }

    // Update transform transitions.
    animationTime = util::now();
    if (transform.needsTransition()) {
        transform.updateTransitions(animationTime);
    }

    state = transform.currentState();

    animationTime = util::now();
    updateSources();
    style->updateProperties(state.getNormalizedZoom(), animationTime);

    // Allow the sprite atlas to potentially pull new sprite images if needed.
    spriteAtlas.resize(state.getPixelRatio());
    spriteAtlas.setSprite(getSprite());

    updateTiles();
}

void Map::render() {
    painter.render(*style, activeSources,
                   state, animationTime);
    // Schedule another rerender when we definitely need a next frame.
    if (transform.needsTransition() || style->hasTransitions()) {
        update();
    }
}
