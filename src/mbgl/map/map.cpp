#include <mbgl/map/map.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/map/annotation.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/clip_id.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/style/style_layer.hpp>
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
#include <mbgl/util/worker.hpp>

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

namespace mbgl {

class MapContext {
public:
    MapContext() :
      glyphAtlas(util::make_unique<GlyphAtlas>(1024, 1024)),
      spriteAtlas(util::make_unique<SpriteAtlas>(512, 512)),
      lineAtlas(util::make_unique<LineAtlas>(512, 512)),
      painter(util::make_unique<Painter>(*spriteAtlas, *glyphAtlas, *lineAtlas))
    {

    }

public:
    std::unique_ptr<GlyphAtlas> glyphAtlas;
    std::unique_ptr<SpriteAtlas> spriteAtlas;
    std::unique_ptr<LineAtlas> lineAtlas;
    std::unique_ptr<Painter> painter;
};


Map::Map(View& view_, FileSource& fileSource_)
    : env(util::make_unique<Environment>(fileSource_)),
      scope(util::make_unique<EnvironmentScope>(*env, ThreadType::Main, "Main")),
      view(view_),
      data(util::make_unique<MapData>(view_)),
      context(util::make_unique<MapContext>()),
      fileSource(fileSource_),
      glyphStore(std::make_shared<GlyphStore>(*env)),
      texturePool(std::make_shared<TexturePool>()),
      updated(static_cast<UpdateType>(Update::Nothing))
{
    view.initialize(this);
}

Map::~Map() {
    if (mode != Mode::None) {
        stop();
    }

    // Extend the scope to include both Main and Map thread types to ease cleanup.
    scope.reset();
    scope = util::make_unique<EnvironmentScope>(
        *env, static_cast<ThreadType>(static_cast<uint8_t>(ThreadType::Main) |
                                      static_cast<uint8_t>(ThreadType::Map)),
        "MapandMain");

    // Explicitly reset all pointers.
    sprite.reset();
    glyphStore.reset();
    style.reset();
    workers.reset();
    context->painter.reset();
    context->lineAtlas.reset();
    context->spriteAtlas.reset();
    context->glyphAtlas.reset();

    uv_run(env->loop, UV_RUN_DEFAULT);

    env->performCleanup();
}

Worker& Map::getWorker() {
    assert(workers);
    return *workers;
}

void Map::start(bool startPaused, Mode renderMode) {
    assert(Environment::currentlyOn(ThreadType::Main));
    assert(mode == Mode::None);

    // When starting map rendering in another thread, we perform async/continuously
    // updated rendering. Only in these cases, we attach the async handlers.
    mode = renderMode;

    // Reset the flag.
    isStopped = false;

    // Setup async notifications
    asyncTerminate = util::make_unique<uv::async>(env->loop, [this]() {
        assert(Environment::currentlyOn(ThreadType::Map));

        // Remove all of these to make sure they are destructed in the correct thread.
        style.reset();

        // It's now safe to destroy/join the workers since there won't be any more callbacks that
        // could dispatch to the worker pool.
        workers.reset();

        terminating = true;

        // Closes all open handles on the loop. This means that the loop will automatically terminate.
        asyncRender.reset();
        asyncUpdate.reset();
        asyncInvoke.reset();
        asyncTerminate.reset();
    });

    asyncUpdate = util::make_unique<uv::async>(env->loop, [this] {
        // Whenever we call triggerUpdate(), we ref() the asyncUpdate handle to make sure that all
        // of the calls actually get triggered.
        asyncUpdate->unref();

        update();
    });

    asyncInvoke = util::make_unique<uv::async>(env->loop, [this] {
        processTasks();
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

void Map::stop(std::function<void ()> cb) {
    assert(Environment::currentlyOn(ThreadType::Main));
    assert(mode != Mode::None);

    asyncTerminate->send();

    resume();

    if (cb) {
        // Wait until the render thread stopped. We are using this construct instead of plainly
        // relying on the thread_join because the system might need to run things in the current
        // thread that is required for the render thread to terminate correctly. This is for example
        // the case with Cocoa's NSURLRequest. Otherwise, we will eventually deadlock because this
        // thread (== main thread) is blocked. The callback function should use an efficient waiting
        // function to avoid a busy waiting loop.
        while (!isStopped) {
            cb();
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
    assert(mode != Mode::None);

    mutexRun.lock();
    pausing = false;
    condRun.notify_all();
    mutexRun.unlock();
}

void Map::renderStill(StillImageCallback fn) {
    assert(Environment::currentlyOn(ThreadType::Main));

    if (mode != Mode::Still) {
        throw util::Exception("Map is not in still image render mode");
    }

    if (callback) {
        throw util::Exception("Map is currently rendering an image");
    }

    assert(mode == Mode::Still);

    callback = std::move(fn);

    triggerUpdate(Update::RenderStill);
}

void Map::run() {
    EnvironmentScope mapScope(*env, ThreadType::Map, "Map");
    assert(Environment::currentlyOn(ThreadType::Map));
    assert(mode != Mode::None);

    if (mode == Mode::Continuous) {
        checkForPause();
    }

    auto styleInfo = data->getStyleInfo();

    view.activate();
    view.discard();

    workers = util::make_unique<Worker>(env->loop, 4);

    setup();
    prepare();

    if (mode == Mode::Continuous) {
        terminating = false;
        while (!terminating) {
            uv_run(env->loop, UV_RUN_DEFAULT);
            checkForPause();
        }
    } else if (mode == Mode::Still) {
        terminating = false;
        while (!terminating) {
            uv_run(env->loop, UV_RUN_DEFAULT);

            // After the loop terminated, these async handles may have been deleted if the terminate()
            // callback was fired. In this case, we are exiting the loop.
            if (asyncTerminate && asyncUpdate) {
                 // Otherwise, loop termination means that we have acquired and parsed all resources
                // required for this map image and we can now proceed to rendering.
                render();
                auto image = view.readStillImage();

                // We are moving the callback out of the way and empty it in case the callback function
                // starts the next map image render.
                assert(callback);
                StillImageCallback cb;
                std::swap(cb, callback);

                // Now we can finally invoke the callback function with the map image we rendered.
                cb(std::move(image));

                // To prepare for the next event loop run, we have to make sure the async handles keep
                // the loop alive.
                asyncTerminate->ref();
                asyncUpdate->ref();
                asyncInvoke->ref();
                asyncRender->ref();
            }
        }
    } else {
        abort();
    }

    // Run the event loop once more to make sure our async delete handlers are called.
    uv_run(env->loop, UV_RUN_ONCE);

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

    if (asyncUpdate) {
        asyncUpdate->ref();
        asyncUpdate->send();
    }
}

void Map::triggerRender() {
    assert(mode == Mode::Continuous);
    assert(asyncRender);
    asyncRender->send();
}

// Runs the function in the map thread.
void Map::invokeTask(std::function<void()>&& fn) {
    {
        std::lock_guard<std::mutex> lock(mutexTask);
        tasks.emplace(::std::forward<std::function<void()>>(fn));
    }

    if (asyncInvoke) {
        asyncInvoke->send();
    }
}

template <typename Fn> auto Map::invokeSyncTask(const Fn& fn) -> decltype(fn()) {
    std::promise<decltype(fn())> promise;
    invokeTask([&fn, &promise] { promise.set_value(fn()); });
    return promise.get_future().get();
}

// Processes the functions that should be run in the map thread.
void Map::processTasks() {
    std::queue<std::function<void()>> queue;
    {
        std::lock_guard<std::mutex> lock(mutexTask);
        queue.swap(tasks);
    }

    while (!queue.empty()) {
        queue.front()();
        queue.pop();
    }
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
    assert(context->painter);
    context->painter->terminate();
    view.deactivate();
}

#pragma mark - Setup

void Map::setup() {
    assert(Environment::currentlyOn(ThreadType::Map));
    assert(context->painter);
    context->painter->setup();
}

std::string Map::getStyleURL() const {
    return data->getStyleInfo().url;
}

void Map::setStyleURL(const std::string &url) {
    assert(Environment::currentlyOn(ThreadType::Main));

    const std::string styleURL = mbgl::util::mapbox::normalizeStyleURL(url, getAccessToken());

    const size_t pos = styleURL.rfind('/');
    std::string base = "";
    if (pos != std::string::npos) {
        base = styleURL.substr(0, pos + 1);
    }

    data->setStyleInfo({ styleURL, base, "" });
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
    assert(Environment::currentlyOn(ThreadType::Map));
    const float pixelRatio = data->getTransformState().getPixelRatio();
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
    if (data->transform.resize(width, height, ratio, fbWidth, fbHeight)) {
        triggerUpdate();
    }
}

#pragma mark - Transitions

void Map::cancelTransitions() {
    data->transform.cancelTransitions();
    triggerUpdate();
}

void Map::setGestureInProgress(bool inProgress) {
    data->transform.setGestureInProgress(inProgress);
    triggerUpdate();
}

#pragma mark - Position

void Map::moveBy(double dx, double dy, Duration duration) {
    data->transform.moveBy(dx, dy, duration);
    triggerUpdate();
}

void Map::setLatLng(LatLng latLng, Duration duration) {
    data->transform.setLatLng(latLng, duration);
    triggerUpdate();
}

LatLng Map::getLatLng() const {
    return data->transform.getLatLng();
}

void Map::resetPosition() {
    data->transform.setAngle(0);
    data->transform.setLatLng(LatLng(0, 0));
    data->transform.setZoom(0);
    triggerUpdate(Update::Zoom);
}


#pragma mark - Scale

void Map::scaleBy(double ds, double cx, double cy, Duration duration) {
    data->transform.scaleBy(ds, cx, cy, duration);
    triggerUpdate(Update::Zoom);
}

void Map::setScale(double scale, double cx, double cy, Duration duration) {
    data->transform.setScale(scale, cx, cy, duration);
    triggerUpdate(Update::Zoom);
}

double Map::getScale() const {
    return data->transform.getScale();
}

void Map::setZoom(double zoom, Duration duration) {
    data->transform.setZoom(zoom, duration);
    triggerUpdate(Update::Zoom);
}

double Map::getZoom() const {
    return data->transform.getZoom();
}

void Map::setLatLngZoom(LatLng latLng, double zoom, Duration duration) {
    data->transform.setLatLngZoom(latLng, zoom, duration);
    triggerUpdate(Update::Zoom);
}

void Map::resetZoom() {
    setZoom(0);
}

uint16_t Map::getWidth() const {
    return data->getTransformState().getWidth();
}

uint16_t Map::getHeight() const {
    return data->getTransformState().getHeight();
}


#pragma mark - Rotation

void Map::rotateBy(double sx, double sy, double ex, double ey, Duration duration) {
    data->transform.rotateBy(sx, sy, ex, ey, duration);
    triggerUpdate();
}

void Map::setBearing(double degrees, Duration duration) {
    data->transform.setAngle(-degrees * M_PI / 180, duration);
    triggerUpdate();
}

void Map::setBearing(double degrees, double cx, double cy) {
    data->transform.setAngle(-degrees * M_PI / 180, cx, cy);
    triggerUpdate();
}

double Map::getBearing() const {
    return -data->transform.getAngle() / M_PI * 180;
}

void Map::resetNorth() {
    data->transform.setAngle(0, std::chrono::milliseconds(500));
    triggerUpdate();
}


#pragma mark - Rotation

double Map::getMinZoom() const {
    return data->transform.getMinZoom();
}

double Map::getMaxZoom() const {
    return data->transform.getMaxZoom();
}



#pragma mark - Access Token

void Map::setAccessToken(const std::string &token) {
    data->setAccessToken(token);
}

std::string Map::getAccessToken() const {
    return data->getAccessToken();
}


#pragma mark - Projection

void Map::getWorldBoundsMeters(ProjectedMeters& sw, ProjectedMeters& ne) const {
    Projection::getWorldBoundsMeters(sw, ne);
}

void Map::getWorldBoundsLatLng(LatLng& sw, LatLng& ne) const {
    Projection::getWorldBoundsLatLng(sw, ne);
}

double Map::getMetersPerPixelAtLatitude(const double lat, const double zoom) const {
    return Projection::getMetersPerPixelAtLatitude(lat, zoom);
}

const ProjectedMeters Map::projectedMetersForLatLng(const LatLng latLng) const {
    return Projection::projectedMetersForLatLng(latLng);
}

const LatLng Map::latLngForProjectedMeters(const ProjectedMeters projectedMeters) const {
    return Projection::latLngForProjectedMeters(projectedMeters);
}

const vec2<double> Map::pixelForLatLng(const LatLng latLng) const {
    return data->transform.currentState().pixelForLatLng(latLng);
}

const LatLng Map::latLngForPixel(const vec2<double> pixel) const {
    return data->transform.currentState().latLngForPixel(pixel);
}

#pragma mark - Annotations

void Map::setDefaultPointAnnotationSymbol(const std::string& symbol) {
    assert(Environment::currentlyOn(ThreadType::Main));
    invokeTask([=] {
        data->annotationManager.setDefaultPointAnnotationSymbol(symbol);
    });
}

double Map::getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol) {
    assert(Environment::currentlyOn(ThreadType::Main));
    return invokeSyncTask([&] {
        assert(sprite);
        const SpritePosition pos = sprite->getSpritePosition(symbol);
        return -pos.height / pos.pixelRatio / 2;
    });
}

uint32_t Map::addPointAnnotation(const LatLng& point, const std::string& symbol) {
    return addPointAnnotations({ point }, { symbol }).front();
}

std::vector<uint32_t> Map::addPointAnnotations(const std::vector<LatLng>& points, const std::vector<std::string>& symbols) {
    assert(Environment::currentlyOn(ThreadType::Main));
    return invokeSyncTask([&] {
        auto result = data->annotationManager.addPointAnnotations(points, symbols, *data);
        updateAnnotationTiles(result.first);
        return result.second;
    });
}

void Map::removeAnnotation(uint32_t annotation) {
    assert(Environment::currentlyOn(ThreadType::Main));
    removeAnnotations({ annotation });
}

void Map::removeAnnotations(const std::vector<uint32_t>& annotations) {
    assert(Environment::currentlyOn(ThreadType::Main));
    invokeTask([=] {
        auto result = data->annotationManager.removeAnnotations(annotations, *data);
        updateAnnotationTiles(result);
    });
}

std::vector<uint32_t> Map::getAnnotationsInBounds(const LatLngBounds& bounds) {
    assert(Environment::currentlyOn(ThreadType::Main));
    return invokeSyncTask([&] {
        return data->annotationManager.getAnnotationsInBounds(bounds, *data);
    });
}

LatLngBounds Map::getBoundsForAnnotations(const std::vector<uint32_t>& annotations) {
    assert(Environment::currentlyOn(ThreadType::Main));
    return invokeSyncTask([&] {
        return data->annotationManager.getBoundsForAnnotations(annotations);
    });
}

void Map::updateAnnotationTiles(const std::vector<TileID>& ids) {
    assert(Environment::currentlyOn(ThreadType::Main));
    if (!style) return;
    for (const auto &source : style->sources) {
        if (source->info.type == SourceType::Annotations) {
            source->invalidateTiles(ids);
        }
    }
    triggerUpdate();
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

void Map::setDefaultTransitionDuration(Duration duration) {
    assert(Environment::currentlyOn(ThreadType::Main));

    data->setDefaultTransitionDuration(duration);
    triggerUpdate(Update::DefaultTransitionDuration);
}

Duration Map::getDefaultTransitionDuration() {
    assert(Environment::currentlyOn(ThreadType::Main));
    return data->getDefaultTransitionDuration();
}

void Map::updateTiles() {
    assert(Environment::currentlyOn(ThreadType::Map));
    if (!style) return;
    for (const auto& source : style->sources) {
        source->update(*data, getWorker(), style, *context->glyphAtlas, *glyphStore,
                       *context->spriteAtlas, getSprite(), *texturePool, [this]() {
            assert(Environment::currentlyOn(ThreadType::Map));
            triggerUpdate();
        });
    }
}

void Map::update() {
    assert(Environment::currentlyOn(ThreadType::Map));

    if (data->getTransformState().hasSize()) {
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
    } else if (!styleInfo.json.empty()) {
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
    style->cascade(data->getClasses());
    style->setDefaultTransitionDuration(data->getDefaultTransitionDuration());

    const std::string glyphURL = util::mapbox::normalizeGlyphsURL(style->glyph_url, getAccessToken());
    glyphStore->setURL(glyphURL);

    for (const auto& source : style->sources) {
        source->load(getAccessToken(), *env, [this]() {
            assert(Environment::currentlyOn(ThreadType::Map));
            triggerUpdate();
        });
    }

    triggerUpdate(Update::Zoom);
}

void Map::prepare() {
    assert(Environment::currentlyOn(ThreadType::Map));

    const auto now = Clock::now();
    data->setAnimationTime(now);

    auto u = updated.exchange(static_cast<UpdateType>(Update::Nothing)) |
             data->transform.updateTransitions(now);

    if (!style) {
        u |= static_cast<UpdateType>(Update::StyleInfo);
    }

    data->setTransformState(data->transform.currentState());

    if (u & static_cast<UpdateType>(Update::StyleInfo)) {
        reloadStyle();
    }

    if (u & static_cast<UpdateType>(Update::Debug)) {
        assert(context->painter);
        context->painter->setDebug(data->getDebug());
    }

    if (u & static_cast<UpdateType>(Update::RenderStill)) {
        // Triggers a view resize.
        view.discard();

        // Whenever we trigger an image render, we are unrefing all async handles so the loop will
        // eventually terminate. However, it'll stay alive as long as there are pending requests
        // (like work requests or HTTP requests).
        asyncTerminate->unref();
        asyncUpdate->unref();
        asyncInvoke->unref();
        asyncRender->unref();
    }

    if (style) {
        if (u & static_cast<UpdateType>(Update::DefaultTransitionDuration)) {
            style->setDefaultTransitionDuration(data->getDefaultTransitionDuration());
        }

        if (u & static_cast<UpdateType>(Update::Classes)) {
            style->cascade(data->getClasses());
        }

        if (u & static_cast<UpdateType>(Update::StyleInfo) ||
            u & static_cast<UpdateType>(Update::Classes) ||
            u & static_cast<UpdateType>(Update::Zoom)) {
            style->recalculate(data->getTransformState().getNormalizedZoom(), now);
        }

        // Allow the sprite atlas to potentially pull new sprite images if needed.
        context->spriteAtlas->resize(data->getTransformState().getPixelRatio());
        context->spriteAtlas->setSprite(getSprite());

        updateTiles();
    }

    if (mode == Mode::Continuous) {
        view.invalidate();
    }
}

void Map::render() {
    assert(Environment::currentlyOn(ThreadType::Map));

    view.discard();

    // Cleanup OpenGL objects that we abandoned since the last render call.
    env->performCleanup();

    assert(style);
    assert(context->painter);

    context->painter->render(*style, data->getTransformState(), data->getAnimationTime());

    // Schedule another rerender when we definitely need a next frame.
    if (data->transform.needsTransition() || style->hasTransitions()) {
        triggerUpdate();
    }
}

void Map::setSourceTileCacheSize(size_t size) {
    if (size != getSourceTileCacheSize()) {
        invokeTask([=] {
            sourceCacheSize = size;
            if (!style) return;
            for (const auto &source : style->sources) {
                source->setCacheSize(sourceCacheSize);
            }
            env->performCleanup();
        });
    }
}

void Map::onLowMemory() {
    invokeTask([=] {
        if (!style) return;
        for (const auto &source : style->sources) {
            source->onLowMemory();
        }
        env->performCleanup();
    });
}

}
