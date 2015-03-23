#include <mbgl/map/map.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/map_data.hpp>
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
      scope(util::make_unique<EnvironmentScope>(*env, ThreadType::Main, "Main")),
      view(view_),
      transform(view_),
      fileSource(fileSource_),
      glyphAtlas(util::make_unique<GlyphAtlas>(1024, 1024)),
      glyphStore(std::make_shared<GlyphStore>(*env)),
      spriteAtlas(util::make_unique<SpriteAtlas>(512, 512)),
      lineAtlas(util::make_unique<LineAtlas>(512, 512)),
      texturePool(std::make_shared<TexturePool>()),
      painter(util::make_unique<Painter>(*spriteAtlas, *glyphAtlas, *lineAtlas)),
      annotationManager(util::make_unique<AnnotationManager>()),
      data(util::make_unique<MapData>())
{
    view.initialize(this);
}

Map::~Map() {
    if (mode == Mode::Continuous) {
        stop();
    }

    // Extend the scope to include both Main and Map thread types to ease cleanup.
    scope.reset();
    scope = util::make_unique<EnvironmentScope>(
        *env, static_cast<ThreadType>(static_cast<uint8_t>(ThreadType::Main) |
                                      static_cast<uint8_t>(ThreadType::Map)),
        "MapandMain");

    // Explicitly reset all pointers.
    activeSources.clear();
    sprite.reset();
    glyphStore.reset();
    style.reset();
    workers.reset();
    painter.reset();
    annotationManager.reset();
    lineAtlas.reset();
    spriteAtlas.reset();
    glyphAtlas.reset();

    uv_run(env->loop, UV_RUN_DEFAULT);

    env->performCleanup();
}

uv::worker &Map::getWorker() {
    assert(workers);
    return *workers;
}

void Map::start(bool startPaused) {
    assert(Environment::currentlyOn(ThreadType::Main));
    assert(mode == Mode::None);

    // When starting map rendering in another thread, we perform async/continuously
    // updated rendering. Only in these cases, we attach the async handlers.
    mode = Mode::Continuous;

    // Reset the flag.
    isStopped = false;

    // Setup async notifications
    asyncTerminate = util::make_unique<uv::async>(env->loop, [this]() {
        assert(Environment::currentlyOn(ThreadType::Map));

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
        update();
    });

    asyncRender = util::make_unique<uv::async>(env->loop, [this] {
        // Must be called in Map thread.
        assert(Environment::currentlyOn(ThreadType::Map));

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
#ifdef __APPLE__
        pthread_setname_np("Map");
#endif

        run();

        // Make sure that the stop() function knows when to stop invoking the callback function.
        isStopped = true;
        view.notify();
    });

    triggerUpdate();
}

void Map::stop(std::function<void ()> callback) {
    assert(Environment::currentlyOn(ThreadType::Main));
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
    assert(Environment::currentlyOn(ThreadType::Main));
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
    assert(Environment::currentlyOn(ThreadType::Main));
    assert(mode == Mode::Continuous);

    mutexRun.lock();
    pausing = false;
    condRun.notify_all();
    mutexRun.unlock();
}

void Map::run() {
    ThreadType threadType = ThreadType::Map;
    std::string threadName("Map");

    if (mode == Mode::None) {
        mode = Mode::Static;

        // FIXME: Threads should have only one purpose. When running on Static mode,
        // we are currently not spawning a Map thread and running the code on the
        // Main thread, thus, the Main thread in this case is both Main and Map thread.
        threadType = static_cast<ThreadType>(static_cast<uint8_t>(threadType) | static_cast<uint8_t>(ThreadType::Main));
        threadName += "andMain";
    }

    EnvironmentScope mapScope(*env, threadType, threadName);

    if (mode == Mode::Continuous) {
        checkForPause();
    }

    auto styleInfo = data->getStyleInfo();

    if (mode == Mode::Static && !style && (styleInfo.url.empty() && styleInfo.json.empty())) {
        throw util::Exception("Style is not set");
    }

    view.activate();

    workers = util::make_unique<uv::worker>(env->loop, 4, "Tile Worker");

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
        mode = Mode::None;
    }

    view.deactivate();
}

void Map::renderSync() {
    // Must be called in UI thread.
    assert(Environment::currentlyOn(ThreadType::Main));

    triggerRender();

    std::unique_lock<std::mutex> lock(mutexRendered);
    condRendered.wait(lock, [this] { return rendered; });
    rendered = false;
}

void Map::triggerUpdate(const Update u) {
    updated |= static_cast<UpdateType>(u);

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
    assert(Environment::currentlyOn(ThreadType::Map));
    assert(painter);
    painter->setup();
}

void Map::setStyleURL(const std::string &url) {
    assert(Environment::currentlyOn(ThreadType::Main));

    const size_t pos = url.rfind('/');
    std::string base = "";
    if (pos != std::string::npos) {
        base = url.substr(0, pos + 1);
    }

    data->setStyleInfo({ url, base, "" });
    triggerUpdate(Update::StyleInfo);
}

void Map::setStyleJSON(const std::string& json, const std::string& base) {
    assert(Environment::currentlyOn(ThreadType::Main));

    data->setStyleInfo({ "", base, json });
    triggerUpdate(Update::StyleInfo);
}

std::string Map::getStyleJSON() const {
    return data->getStyleInfo().json;
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
    data->setAccessToken(token);
}

std::string Map::getAccessToken() const {
    return data->getAccessToken();
}

#pragma mark - Annotations

void Map::setDefaultPointAnnotationSymbol(const std::string& symbol) {
    assert(Environment::currentlyOn(ThreadType::Main));
    annotationManager->setDefaultPointAnnotationSymbol(symbol);
}

uint32_t Map::addPointAnnotation(const LatLng& point, const std::string& symbol) {
    assert(Environment::currentlyOn(ThreadType::Main));
    std::vector<LatLng> points({ point });
    std::vector<std::string> symbols({ symbol });
    return addPointAnnotations(points, symbols)[0];
}

std::vector<uint32_t> Map::addPointAnnotations(const std::vector<LatLng>& points, const std::vector<std::string>& symbols) {
    assert(Environment::currentlyOn(ThreadType::Main));
    auto result = annotationManager->addPointAnnotations(points, symbols, *this);
    updateAnnotationTiles(result.first);
    return result.second;
}

void Map::removeAnnotation(uint32_t annotation) {
    assert(Environment::currentlyOn(ThreadType::Main));
    removeAnnotations({ annotation });
}

void Map::removeAnnotations(const std::vector<uint32_t>& annotations) {
    assert(Environment::currentlyOn(ThreadType::Main));
    auto result = annotationManager->removeAnnotations(annotations);
    updateAnnotationTiles(result);
}

std::vector<uint32_t> Map::getAnnotationsInBounds(const LatLngBounds& bounds) const {
    assert(Environment::currentlyOn(ThreadType::Main));
    return annotationManager->getAnnotationsInBounds(bounds, *this);
}

LatLngBounds Map::getBoundsForAnnotations(const std::vector<uint32_t>& annotations) const {
    assert(Environment::currentlyOn(ThreadType::Main));
    return annotationManager->getBoundsForAnnotations(annotations);
}

void Map::updateAnnotationTiles(const std::vector<Tile::ID>& ids) {
    assert(Environment::currentlyOn(ThreadType::Main));
    for (const auto &source : activeSources) {
        if (source->info.type == SourceType::Annotations) {
            source->source->invalidateTiles(*this, ids);
            return;
        }
    }
}

#pragma mark - Toggles

void Map::setDebug(bool value) {
    data->setDebug(value);
    triggerUpdate(Update::Debug);
}

void Map::toggleDebug() {
    data->toggleDebug();
    triggerUpdate(Update::Debug);
}

bool Map::getDebug() const {
    return data->getDebug();
}

std::chrono::steady_clock::time_point Map::getTime() const {
    return data->getAnimationTime();
}

void Map::addClass(const std::string& klass) {
    if (data->addClass(klass)) {
        triggerUpdate(Update::Classes);
    }
}

void Map::removeClass(const std::string& klass) {
    if (data->removeClass(klass)) {
        triggerUpdate(Update::Classes);
    }
}

void Map::setClasses(const std::vector<std::string>& classes) {
    data->setClasses(classes);
    triggerUpdate(Update::Classes);
}

bool Map::hasClass(const std::string& klass) const {
    return data->hasClass(klass);
}

std::vector<std::string> Map::getClasses() const {
    return data->getClasses();
}

void Map::setDefaultTransitionDuration(std::chrono::steady_clock::duration duration) {
    assert(Environment::currentlyOn(ThreadType::Main));

    data->setDefaultTransitionDuration(duration);
    triggerUpdate(Update::DefaultTransitionDuration);
}

std::chrono::steady_clock::duration Map::getDefaultTransitionDuration() {
    assert(Environment::currentlyOn(ThreadType::Main));
    return data->getDefaultTransitionDuration();
}

void Map::updateSources() {
    assert(Environment::currentlyOn(ThreadType::Map));

    // First, disable all existing sources.
    for (const auto& source : activeSources) {
        source->enabled = false;
    }

    // Then, reenable all of those that we actually use when drawing this layer.
    if (style) {
        updateSources(style->layers);
    }

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
    assert(Environment::currentlyOn(ThreadType::Map));
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
    assert(Environment::currentlyOn(ThreadType::Map));
    for (const auto &source : activeSources) {
        source->source->update(*this, getWorker(), style, *glyphAtlas, *glyphStore,
                               *spriteAtlas, getSprite(), *texturePool, [this]() {
            assert(Environment::currentlyOn(ThreadType::Map));
            triggerUpdate();
        });
    }
}

void Map::update() {
    assert(Environment::currentlyOn(ThreadType::Map));

    if (state.hasSize()) {
        prepare();
    }
}

void Map::reloadStyle() {
    assert(Environment::currentlyOn(ThreadType::Map));

    style = std::make_shared<Style>();

    const auto styleInfo = data->getStyleInfo();

    if (!styleInfo.url.empty()) {
        const auto base = styleInfo.base;
        // We have a style URL
        env->request({ Resource::Kind::JSON, styleInfo.url }, [this, base](const Response &res) {
            if (res.status == Response::Successful) {
                loadStyleJSON(res.data, base);
            } else {
                Log::Error(Event::Setup, "loading style failed: %s", res.message.c_str());
            }
        });
    } else {
        // We got JSON data directly.
        loadStyleJSON(styleInfo.json, styleInfo.base);
    }
}

void Map::loadStyleJSON(const std::string& json, const std::string& base) {
    assert(Environment::currentlyOn(ThreadType::Map));

    sprite.reset();
    style = std::make_shared<Style>();
    style->base = base;
    style->loadJSON((const uint8_t *)json.c_str());
    style->cascadeClasses(data->getClasses());
    style->setDefaultTransitionDuration(data->getDefaultTransitionDuration());

    const std::string glyphURL = util::mapbox::normalizeGlyphsURL(style->glyph_url, getAccessToken());
    glyphStore->setURL(glyphURL);

    triggerUpdate();
}

void Map::prepare() {
    assert(Environment::currentlyOn(ThreadType::Map));

    const auto u = updated.exchange(static_cast<UpdateType>(Update::Nothing));
    if (u & static_cast<UpdateType>(Update::StyleInfo)) {
        reloadStyle();
    }
    if (u & static_cast<UpdateType>(Update::Debug)) {
        assert(painter);
        painter->setDebug(data->getDebug());
    }
    if (u & static_cast<UpdateType>(Update::DefaultTransitionDuration)) {
        if (style) {
            style->setDefaultTransitionDuration(data->getDefaultTransitionDuration());
        }
    }
    if (u & static_cast<UpdateType>(Update::Classes)) {
        if (style) {
            style->cascadeClasses(data->getClasses());
        }
    }

    // Update transform transitions.

    const auto animationTime = std::chrono::steady_clock::now();
    data->setAnimationTime(animationTime);
    if (transform.needsTransition()) {
        transform.updateTransitions(animationTime);
    }

    state = transform.currentState();

    if (style) {
        updateSources();
        style->updateProperties(state.getNormalizedZoom(), animationTime);

        // Allow the sprite atlas to potentially pull new sprite images if needed.
        spriteAtlas->resize(state.getPixelRatio());
        spriteAtlas->setSprite(getSprite());

        updateTiles();
    }

    if (mode == Mode::Continuous) {
        view.invalidate();
    }
}

void Map::render() {
    assert(Environment::currentlyOn(ThreadType::Map));

    // Cleanup OpenGL objects that we abandoned since the last render call.
    env->performCleanup();

    assert(painter);
    painter->render(*style, activeSources,
                    state, data->getAnimationTime());
    // Schedule another rerender when we definitely need a next frame.
    if (transform.needsTransition() || style->hasTransitions()) {
        triggerUpdate();
    }
}
