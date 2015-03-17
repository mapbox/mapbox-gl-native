#include <mbgl/map/map.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/map/annotation.hpp>
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
#include <mbgl/util/exception.hpp>

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
    : env(util::make_unique<Environment>(fileSource_)),
      view(view_),
      mainThread(std::this_thread::get_id()),
      mapThread(mainThread),
      transform(view_),
      fileSource(fileSource_),
      glyphAtlas(util::make_unique<GlyphAtlas>(1024, 1024)),
      glyphStore(std::make_shared<GlyphStore>(*env)),
      spriteAtlas(util::make_unique<SpriteAtlas>(512, 512)),
      lineAtlas(util::make_unique<LineAtlas>(512, 512)),
      texturePool(std::make_shared<TexturePool>()),
      painter(util::make_unique<Painter>(*spriteAtlas, *glyphAtlas, *lineAtlas)),
      annotationManager(util::make_unique<AnnotationManager>())
{
    view.initialize(this);
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

    uv_run(env->loop, UV_RUN_DEFAULT);
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
    asyncTerminate = util::make_unique<uv::async>(env->loop, [this]() {
        assert(std::this_thread::get_id() == mapThread);

        // Remove all of these to make sure they are destructed in the correct thread.
        style.reset();
        workers.reset();
        activeSources.clear();

        terminating = true;

        // Closes all open handles on the loop. This means that the loop will automatically terminate.
        asyncRender.reset();
        asyncUpdate.reset();
        asyncTerminate.reset();
    });

    asyncUpdate = util::make_unique<uv::async>(env->loop, [this] {
        assert(std::this_thread::get_id() == mapThread);

        if (state.hasSize()) {
            prepare();
        }
    });

    asyncRender = util::make_unique<uv::async>(env->loop, [this] {
        // Must be called in Map thread.
        assert(std::this_thread::get_id() == mapThread);

        render();

        // Finally, notify all listeners that we have finished rendering this frame.
        {
            std::lock_guard<std::mutex> lk(mutexRendered);
            rendered = true;
        }
        condRendered.notify_all();
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

    uv_stop(env->loop);
    triggerUpdate(); // Needed to ensure uv_stop is seen and uv_run exits, otherwise we deadlock on wait_for_pause

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

    if (mode == Mode::Static && !style && styleURL.empty()) {
        throw util::Exception("Style is not set");
    }

    view.activate();

    workers = util::make_unique<uv::worker>(env->loop, 4, "Tile Worker");

    env->setup();

    setup();
    prepare();

    if (mode == Mode::Continuous) {
        terminating = false;
        while(!terminating) {
            uv_run(env->loop, UV_RUN_DEFAULT);
            checkForPause();
        }
    } else {
        uv_run(env->loop, UV_RUN_DEFAULT);
    }

    // Run the event loop once more to make sure our async delete handlers are called.
    uv_run(env->loop, UV_RUN_ONCE);

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

void Map::renderSync() {
    // Must be called in UI thread.
    assert(std::this_thread::get_id() == mainThread);

    triggerRender();

    std::unique_lock<std::mutex> lock(mutexRendered);
    condRendered.wait(lock, [this] { return rendered; });
    rendered = false;
}

void Map::triggerUpdate() {
    if (mode == Mode::Static) {
        prepare();
    } else if (asyncUpdate) {
        asyncUpdate->send();
    }
}

void Map::triggerRender() {
    assert(asyncRender);
    asyncRender->send();
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

    triggerUpdate();
}

std::string Map::getStyleJSON() const {
    return styleJSON;
}

util::ptr<Sprite> Map::getSprite() {
    const float pixelRatio = state.getPixelRatio();
    const std::string &sprite_url = style->getSpriteURL();
    if (!sprite || !sprite->hasPixelRatio(pixelRatio)) {
        sprite = Sprite::Create(sprite_url, pixelRatio, *env);
    }

    return sprite;
}


#pragma mark - Size

void Map::resize(uint16_t width, uint16_t height, float ratio) {
    resize(width, height, ratio, width * ratio, height * ratio);
}

void Map::resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight) {
    if (transform.resize(width, height, ratio, fbWidth, fbHeight)) {
        triggerUpdate();
    }
}

#pragma mark - Transitions

void Map::cancelTransitions() {
    transform.cancelTransitions();

    triggerUpdate();
}


#pragma mark - Position

void Map::moveBy(double dx, double dy, std::chrono::steady_clock::duration duration) {
    transform.moveBy(dx, dy, duration);
    triggerUpdate();
}

void Map::setLatLng(LatLng latLng, std::chrono::steady_clock::duration duration) {
    transform.setLatLng(latLng, duration);
    triggerUpdate();
}

LatLng Map::getLatLng() const {
    return state.getLatLng();
}

void Map::startPanning() {
    transform.startPanning();
    triggerUpdate();
}

void Map::stopPanning() {
    transform.stopPanning();
    triggerUpdate();
}

void Map::resetPosition() {
    transform.setAngle(0);
    transform.setLatLng(LatLng(0, 0));
    transform.setZoom(0);
    triggerUpdate();
}


#pragma mark - Scale

void Map::scaleBy(double ds, double cx, double cy, std::chrono::steady_clock::duration duration) {
    transform.scaleBy(ds, cx, cy, duration);
    triggerUpdate();
}

void Map::setScale(double scale, double cx, double cy, std::chrono::steady_clock::duration duration) {
    transform.setScale(scale, cx, cy, duration);
    triggerUpdate();
}

double Map::getScale() const {
    return transform.getScale();
}

void Map::setZoom(double zoom, std::chrono::steady_clock::duration duration) {
    transform.setZoom(zoom, duration);
    triggerUpdate();
}

double Map::getZoom() const {
    return transform.getZoom();
}

void Map::setLatLngZoom(LatLng latLng, double zoom, std::chrono::steady_clock::duration duration) {
    transform.setLatLngZoom(latLng, zoom, duration);
    triggerUpdate();
}

void Map::resetZoom() {
    setZoom(0);
}

void Map::startScaling() {
    transform.startScaling();
    triggerUpdate();
}

void Map::stopScaling() {
    transform.stopScaling();
    triggerUpdate();
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
    triggerUpdate();
}

void Map::setBearing(double degrees, std::chrono::steady_clock::duration duration) {
    transform.setAngle(-degrees * M_PI / 180, duration);
    triggerUpdate();
}

void Map::setBearing(double degrees, double cx, double cy) {
    transform.setAngle(-degrees * M_PI / 180, cx, cy);
    triggerUpdate();
}

double Map::getBearing() const {
    return -transform.getAngle() / M_PI * 180;
}

void Map::resetNorth() {
    transform.setAngle(0, std::chrono::milliseconds(500));
    triggerUpdate();
}

void Map::startRotating() {
    transform.startRotating();
    triggerUpdate();
}

void Map::stopRotating() {
    transform.stopRotating();
    triggerUpdate();
}

#pragma mark - Access Token

void Map::setAccessToken(const std::string &token) {
    accessToken = token;
}

const std::string &Map::getAccessToken() const {
    return accessToken;
}

#pragma mark - Annotations

void Map::setDefaultPointAnnotationSymbol(std::string& symbol) {
    assert(std::this_thread::get_id() == mainThread);
    annotationManager->setDefaultPointAnnotationSymbol(symbol);
}

uint32_t Map::addPointAnnotation(LatLng point, std::string& symbol) {
    assert(std::this_thread::get_id() == mainThread);
    std::vector<LatLng> points({ point });
    std::vector<std::string> symbols({ symbol });
    return addPointAnnotations(points, symbols)[0];
}

std::vector<uint32_t> Map::addPointAnnotations(std::vector<LatLng> points, std::vector<std::string>& symbols) {
    assert(std::this_thread::get_id() == mainThread);
    auto result = annotationManager->addPointAnnotations(points, symbols, *this);
    updateAnnotationTiles(result.first);
    return result.second;
}

void Map::removeAnnotation(uint32_t annotation) {
    assert(std::this_thread::get_id() == mainThread);
    removeAnnotations({ annotation });
}

void Map::removeAnnotations(std::vector<uint32_t> annotations) {
    assert(std::this_thread::get_id() == mainThread);
    auto result = annotationManager->removeAnnotations(annotations);
    updateAnnotationTiles(result);
}

std::vector<uint32_t> Map::getAnnotationsInBounds(LatLngBounds bounds) const {
    assert(std::this_thread::get_id() == mainThread);
    return annotationManager->getAnnotationsInBounds(bounds, *this);
}

LatLngBounds Map::getBoundsForAnnotations(std::vector<uint32_t> annotations) const {
    assert(std::this_thread::get_id() == mainThread);
    return annotationManager->getBoundsForAnnotations(annotations);
}

void Map::updateAnnotationTiles(std::vector<Tile::ID>& ids) {
    for (const auto &source : activeSources) {
        if (source->info.type == SourceType::Annotations) {
            source->source->invalidateTiles(*this, ids);
            return;
        }
    }
}

#pragma mark - Toggles

void Map::setDebug(bool value) {
    debug = value;
    assert(painter);
    painter->setDebug(debug);
    triggerUpdate();
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
            triggerUpdate();
        }
    }
}

void Map::removeClass(const std::string& klass) {
    if (!hasClass(klass)) return;
    classes.erase(std::remove(classes.begin(), classes.end(), klass), classes.end());
    if (style) {
        style->cascadeClasses(classes);
        if (style->hasTransitions()) {
            triggerUpdate();
        }
    }
}

void Map::setClasses(const std::vector<std::string>& classes_) {
    classes = classes_;
    if (style) {
        style->cascadeClasses(classes);
        if (style->hasTransitions()) {
            triggerUpdate();
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
                source->source->load(*this, *env);
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
    assert(std::this_thread::get_id() == mapThread);
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
    assert(std::this_thread::get_id() == mapThread);
    for (const auto &source : activeSources) {
        source->source->update(*this, *env, getWorker(), style, *glyphAtlas, *glyphStore,
                               *spriteAtlas, getSprite(), *texturePool, [this]() {
            assert(std::this_thread::get_id() == mapThread);
            triggerUpdate();
        });
    }
}

void Map::prepare() {
    assert(std::this_thread::get_id() == mapThread);

    if (!style) {
        style = std::make_shared<Style>();

        env->request({ Resource::Kind::JSON, styleURL}, [&](const Response &res) {
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

    if (mode == Mode::Continuous) {
        view.invalidate();
    }
}

void Map::render() {
    assert(std::this_thread::get_id() == mapThread);
    assert(painter);
    painter->render(*style, activeSources,
                    state, animationTime);
    // Schedule another rerender when we definitely need a next frame.
    if (transform.needsTransition() || style->hasTransitions()) {
        triggerUpdate();
    }
}
