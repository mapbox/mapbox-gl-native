#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/util/transition.hpp>
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
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/uv.hpp>
#include <mbgl/util/mapbox.hpp>

#include <algorithm>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include <uv.h>

// Check libuv library version.
const static bool uvVersionCheck = []() {
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

using namespace mbgl;

Map::Map(View& view_, FileSource& fileSource_)
    : loop(util::make_unique<uv::loop>()),
      view(view_),
#ifdef DEBUG
      mainThread(std::this_thread::get_id()),
      mapThread(mainThread),
#endif
      transform(view_),
      fileSource(fileSource_),
      glyphAtlas(util::make_unique<GlyphAtlas>(1024, 1024)),
      glyphStore(std::make_shared<GlyphStore>(fileSource)),
      spriteAtlas(util::make_unique<SpriteAtlas>(512, 512)),
      lineAtlas(util::make_unique<LineAtlas>(512, 512)),
      texturePool(std::make_shared<TexturePool>()),
      painter(util::make_unique<Painter>(*spriteAtlas, *glyphAtlas, *lineAtlas))
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
    assert(workers);
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
#ifdef DEBUG
        mapThread = std::this_thread::get_id();
#endif

#ifdef __APPLE__
        pthread_setname_np("Map");
#endif

        run();

#ifdef DEBUG
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
    if (mode == Mode::None) {
#ifdef DEBUG
        mapThread = mainThread;
#endif
        mode = Mode::Static;
    }
    assert(std::this_thread::get_id() == mapThread);

    if (mode == Mode::Continuous) {
        checkForPause();
    }

    view.activate();

    workers = util::make_unique<uv::worker>(**loop, 4, "Tile Worker");

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
#ifdef DEBUG
        mapThread = std::thread::id();
#endif
        mode = Mode::None;
    }

    view.deactivate();
}

void Map::checkForPause() {
    std::unique_lock<std::mutex> lockRun (mutexRun);
    while (pausing) {
        view.deactivate();

        mutexPause.lock();
        isPaused = true;
        condPause.notify_all();
        mutexPause.unlock();

        condRun.wait(lockRun);

        view.activate();
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
        if (asyncRender) {
            asyncRender->send();
        }
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
    view.deactivate();
}

#pragma mark - Setup

void Map::setup() {
    assert(std::this_thread::get_id() == mapThread);
    assert(painter);
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

    style->base = base;
    style->loadJSON((const uint8_t *)styleJSON.c_str());
    style->cascadeClasses(classes);
    style->setDefaultTransitionDuration(defaultTransitionDuration);

    const std::string glyphURL = util::mapbox::normalizeGlyphsURL(style->glyph_url, getAccessToken());
    glyphStore->setURL(glyphURL);

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

void Map::moveBy(double dx, double dy, std::chrono::steady_clock::duration duration) {
    transform.moveBy(dx, dy, duration);
    update();
}

void Map::setLatLng(LatLng latLng, std::chrono::steady_clock::duration duration) {
    transform.setLatLng(latLng, duration);
    update();
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
    transform.setLatLng(LatLng(0, 0));
    transform.setZoom(0);
    update();
}


#pragma mark - Scale

void Map::scaleBy(double ds, double cx, double cy, std::chrono::steady_clock::duration duration) {
    transform.scaleBy(ds, cx, cy, duration);
    update();
}

void Map::setScale(double scale, double cx, double cy, std::chrono::steady_clock::duration duration) {
    transform.setScale(scale, cx, cy, duration);
    update();
}

double Map::getScale() const {
    return transform.getScale();
}

void Map::setZoom(double zoom, std::chrono::steady_clock::duration duration) {
    transform.setZoom(zoom, duration);
    update();
}

double Map::getZoom() const {
    return transform.getZoom();
}

void Map::setLatLngZoom(LatLng latLng, double zoom, std::chrono::steady_clock::duration duration) {
    transform.setLatLngZoom(latLng, zoom, duration);
    update();
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

void Map::rotateBy(double sx, double sy, double ex, double ey, std::chrono::steady_clock::duration duration) {
    transform.rotateBy(sx, sy, ex, ey, duration);
    update();
}

void Map::setBearing(double degrees, std::chrono::steady_clock::duration duration) {
    transform.setAngle(-degrees * M_PI / 180, duration);
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
    transform.setAngle(0, std::chrono::milliseconds(500));
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

#pragma mark - Access Token

void Map::setAccessToken(const std::string &token) {
    accessToken = token;
}

const std::string &Map::getAccessToken() const {
    return accessToken;
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

void Map::addClass(const std::string& klass) {
    if (hasClass(klass)) return;
    classes.push_back(klass);
    if (style) {
        style->cascadeClasses(classes);
        if (style->hasTransitions()) {
            update();
        }
    }
}

void Map::removeClass(const std::string& klass) {
    if (!hasClass(klass)) return;
    classes.erase(std::remove(classes.begin(), classes.end(), klass), classes.end());
    if (style) {
        style->cascadeClasses(classes);
        if (style->hasTransitions()) {
            update();
        }
    }
}

void Map::setClasses(const std::vector<std::string>& classes_) {
    classes = classes_;
    if (style) {
        style->cascadeClasses(classes);
        if (style->hasTransitions()) {
            update();
        }
    }
}

bool Map::hasClass(const std::string& klass) const {
    return std::find(classes.begin(), classes.end(), klass) != classes.end();
}

std::vector<std::string> Map::getClasses() const {
   return classes;
}

void Map::setDefaultTransitionDuration(std::chrono::steady_clock::duration duration) {
    defaultTransitionDuration = duration;
    if (style) {
        style->setDefaultTransitionDuration(duration);
    }
}

std::chrono::steady_clock::duration Map::getDefaultTransitionDuration() {
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
        if (layer->bucket && layer->bucket->style_source) {
            (*activeSources.emplace(layer->bucket->style_source).first)->enabled = true;
        }
    }
}

void Map::updateTiles() {
    for (const auto& source : activeSources) {
        source->source->update(*this, getWorker(),
                               style, *glyphAtlas, *glyphStore,
                               *spriteAtlas, getSprite(),
                               *texturePool, fileSource, ***loop, [this](){ update(); });
    }
}

void Map::prepare() {
    if (!style) {
        style = std::make_shared<Style>();

        fileSource.request({ Resource::Kind::JSON, styleURL}, **loop, [&](const Response &res) {
            if (res.status == Response::Successful) {
                // Calculate the base
                const size_t pos = styleURL.rfind('/');
                std::string base = "";
                if (pos != std::string::npos) {
                    base = styleURL.substr(0, pos + 1);
                }

                setStyleJSON(res.data, base);
            } else {
                Log::Error(Event::Setup, "loading style failed: %s", res.message.c_str());
            }
        });
    }

    // Update transform transitions.
    animationTime = std::chrono::steady_clock::now();
    if (transform.needsTransition()) {
        transform.updateTransitions(animationTime);
    }

    state = transform.currentState();

    animationTime = std::chrono::steady_clock::now();
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
