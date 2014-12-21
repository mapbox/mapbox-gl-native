#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/util/transition.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/clip_ids.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/uv.hpp>

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

Map::Map(View& view_, FileSource& fileSource_)
    : loop(util::make_unique<uv::loop>()),
      view(view_),
#ifndef NDEBUG
      mainThread(std::this_thread::get_id()),
      mapThread(mainThread),
#endif
      transform(view_),
      fileSource(fileSource_),
      glyphAtlas(util::make_unique<GlyphAtlas>(1024, 1024)),
      glyphStore(std::make_shared<GlyphStore>(fileSource)),
      spriteAtlas(util::make_unique<SpriteAtlas>(512, 512)),
      texturePool(std::make_shared<TexturePool>()),
      painter(util::make_unique<Painter>(*spriteAtlas, *glyphAtlas))
{
    view.initialize(this);
    // Make sure that we're doing an initial drawing in all cases.
    isClean.clear();
    isRendered.clear();
    isSwapped.test_and_set();
}

Map::~Map() {
    if (mode == Mode::Continuous) {
        stop();
    }

    // Explicitly reset all pointers.
    activeSources.clear();
    sprite.reset();
    glyphStore.reset();
    style.reset();
    texturePool.reset();
    workers.reset();

    uv_run(**loop, UV_RUN_DEFAULT);
}

uv::worker &Map::getWorker() {
    if (!workers) {
        workers = util::make_unique<uv::worker>(**loop, 4, "Tile Worker");
    }
    return *workers;
}

void Map::start(bool startPaused) {
    assert(std::this_thread::get_id() == mainThread);
    assert(mode == Mode::None);

    // When starting map rendering in another thread, we perform async/continuously
    // updated rendering. Only in these cases, we attach the async handlers.
    mode = Mode::Continuous;

    // Reset the flag.
    isStopped = false;

    // Setup async notifications
    asyncTerminate = util::make_unique<uv::async>(**loop, [this]() {
        assert(std::this_thread::get_id() == mapThread);

        // Remove all of these to make sure they are destructed in the correct thread.
        style.reset();
        workers.reset();
        activeSources.clear();

        fileSource.clearLoop();

        terminating = true;

        // Closes all open handles on the loop. This means that the loop will automatically terminate.
        asyncRender.reset();
        asyncTerminate.reset();
    });

    asyncRender = util::make_unique<uv::async>(**loop, [this]() {
        assert(std::this_thread::get_id() == mapThread);

        if (state.hasSize()) {
            if (isRendered.test_and_set() == false) {
                prepare();
                if (isClean.test_and_set() == false) {
                    render();
                    isSwapped.clear();
                    view.swap();
                } else {
                    // We set the rendered flag in the test above, so we have to reset it
                    // now that we're not actually rendering because the map is clean.
                    isRendered.clear();
                }
            }
        }
    });

    // Do we need to pause first?
    if (startPaused) {
        pause();
    }

    thread = std::thread([this]() {
#ifndef NDEBUG
        mapThread = std::this_thread::get_id();
#endif

#ifdef __APPLE__
        pthread_setname_np("Map");
#endif

        run();

#ifndef NDEBUG
        mapThread = std::thread::id();
#endif

        // Make sure that the stop() function knows when to stop invoking the callback function.
        isStopped = true;
        view.notify();
    });
}

void Map::stop(std::function<void ()> callback) {
    assert(std::this_thread::get_id() == mainThread);
    assert(mainThread != mapThread);
    assert(mode == Mode::Continuous);

    asyncTerminate->send();

    resume();

    if (callback) {
        // Wait until the render thread stopped. We are using this construct instead of plainly
        // relying on the thread_join because the system might need to run things in the current
        // thread that is required for the render thread to terminate correctly. This is for example
        // the case with Cocoa's NSURLRequest. Otherwise, we will eventually deadlock because this
        // thread (== main thread) is blocked. The callback function should use an efficient waiting
        // function to avoid a busy waiting loop.
        while (!isStopped) {
            callback();
        }
    }

    // If a callback function was provided, this should return immediately because the thread has
    // already finished executing.
    thread.join();

    mode = Mode::None;
}

void Map::pause(bool waitForPause) {
    assert(std::this_thread::get_id() == mainThread);
    assert(mode == Mode::Continuous);
    mutexRun.lock();
    pausing = true;
    mutexRun.unlock();

    uv_stop(**loop);
    rerender(); // Needed to ensure uv_stop is seen and uv_run exits, otherwise we deadlock on wait_for_pause

    if (waitForPause) {
        std::unique_lock<std::mutex> lockPause (mutexPause);
        while (!isPaused) {
            condPause.wait(lockPause);
        }
    }
}

void Map::resume() {
    assert(std::this_thread::get_id() == mainThread);
    assert(mode == Mode::Continuous);

    mutexRun.lock();
    pausing = false;
    condRun.notify_all();
    mutexRun.unlock();
}

void Map::run() {
#ifdef __ANDROID__
    COFFEE_TRY() {
#endif

    if (mode == Mode::None) {
#ifndef NDEBUG
        mapThread = mainThread;
#endif
        mode = Mode::Static;
    }
    assert(std::this_thread::get_id() == mapThread);

    if (mode == Mode::Continuous) {
        checkForPause();
    }

    setup();
    prepare();

    if (mode == Mode::Continuous) {
        terminating = false;
        while(!terminating) {
            uv_run(**loop, UV_RUN_DEFAULT);
            checkForPause();
        }
    } else {
        uv_run(**loop, UV_RUN_DEFAULT);
    }

    // Run the event loop once more to make sure our async delete handlers are called.
    uv_run(**loop, UV_RUN_ONCE);

    // If the map rendering wasn't started asynchronously, we perform one render
    // *after* all events have been processed.
    if (mode == Mode::Static) {
        render();
#ifndef NDEBUG
        mapThread = std::thread::id();
#endif
        mode = Mode::None;
        fileSource.clearLoop();
    }
#ifdef __ANDROID__
    } COFFEE_CATCH() {
        Log::Error(Event::Crash, "Map::run() crash:\n%s", coffeecatch_get_message());
        abort();
    }
#endif
}

void Map::checkForPause() {
    std::unique_lock<std::mutex> lockRun (mutexRun);
    while (pausing) {
        view.make_inactive();

        mutexPause.lock();
        isPaused = true;
        condPause.notify_all();
        mutexPause.unlock();

        condRun.wait(lockRun);

        view.make_active();
    }

    mutexPause.lock();
    isPaused = false;
    mutexPause.unlock();
}

void Map::rerender() {
    if (mode == Mode::Static) {
        prepare();
    } else if (mode == Mode::Continuous) {
        // We only send render events if we want to continuously update the map
        // (== async rendering).
        assert(asyncRender);
        asyncRender->send();
    }
}

void Map::update() {
    isClean.clear();
    rerender();
}

bool Map::needsSwap() {
    return isSwapped.test_and_set() == false;
}

void Map::swapped() {
    isRendered.clear();
    rerender();
}

void Map::terminate() {
    assert(painter);
    painter->terminate();
}

#pragma mark - Setup

void Map::setup() {
    assert(std::this_thread::get_id() == mapThread);
    assert(painter);
    view.make_active();
    painter->setup();
}

void Map::setStyleURL(const std::string &url) {
    // TODO: Make threadsafe.

    styleURL = url;
    if (mode == Mode::Continuous) {
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
    fileSource.setBase(base);
    glyphStore->setURL(style->glyph_url);

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

util::ptr<Sprite> Map::getSprite() {
    const float pixelRatio = state.getPixelRatio();
    const std::string &sprite_url = style->getSpriteURL();
    if (!sprite || sprite->pixelRatio != pixelRatio) {
        sprite = Sprite::Create(sprite_url, pixelRatio, fileSource);
    }

    return sprite;
}


#pragma mark - Size

void Map::resize(uint16_t width, uint16_t height, float ratio) {
    resize(width, height, ratio, width * ratio, height * ratio);
}

void Map::resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight) {
    if (transform.resize(width, height, ratio, fbWidth, fbHeight)) {
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
    assert(painter);
    painter->setDebug(debug);
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
        std::lock_guard<std::mutex> lock(appliedClassesMutex);
        appliedClasses = mbgl::util::make_unique<std::vector<std::string>>(classes);
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

void Map::setDefaultTransitionDuration(uint64_t milliseconds) {
    defaultTransitionDuration = milliseconds;
    if (style) {
        style->setDefaultTransitionDuration(milliseconds);
    }
}

uint64_t Map::getDefaultTransitionDuration() {
    return defaultTransitionDuration;
}

void Map::updateSources() {
    assert(std::this_thread::get_id() == mapThread);

    // First, disable all existing sources.
    for (const auto& source : activeSources) {
        source->enabled = false;
    }

    // Then, reenable all of those that we actually use when drawing this layer.
    updateSources(style->layers);

    // Then, construct or destroy the actual source object, depending on enabled state.
    for (const auto& source : activeSources) {
        if (source->enabled) {
            if (!source->source) {
                source->source = std::make_shared<Source>(source->info);
                source->source->load(*this, fileSource);
            }
        } else {
            source->source.reset();
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
                               style, *glyphAtlas, *glyphStore,
                               *spriteAtlas, getSprite(),
                               *texturePool, fileSource, [this](){ update(); });
    }
}

void Map::prepare() {
    if (!fileSource.hasLoop()) {
        fileSource.setLoop(**loop);
    }

    if (!style) {
        style = std::make_shared<Style>();

        fileSource.request(ResourceType::JSON, styleURL)->onload([&](const Response &res) {
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
    spriteAtlas->resize(state.getPixelRatio());
    spriteAtlas->setSprite(getSprite());

    updateTiles();
}

void Map::render() {
    assert(painter);
    painter->render(*style, activeSources,
                   state, animationTime);
    // Schedule another rerender when we definitely need a next frame.
    if (transform.needsTransition() || style->hasTransitions()) {
        update();
    }
}
