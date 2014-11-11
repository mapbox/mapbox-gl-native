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

#include <algorithm>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

using namespace mbgl;

Map::Map(View& view_)
    : loop(std::make_unique<uv::loop>()),
      thread(std::make_unique<uv::thread>()),
      async_terminate(new uv_async_t()),
      async_render(new uv_async_t()),
      async_cleanup(new uv_async_t()),
      mutex_run(std::make_unique<uv::mutex>()),
      cond_run(std::make_unique<uv::cond>()),
      mutex_pause(std::make_unique<uv::mutex>()),
      cond_pause(std::make_unique<uv::cond>()),
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
    texturepool.reset();
    sprite.reset();
    glyphStore.reset();
    style.reset();
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

// Iron out the differences between libuv 0.10 and 0.11
#ifdef UV_ASYNC_CALLBACK
#error Cannot overwrite UV_ASYNC_CALLBACK
#endif
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
#define UV_ASYNC_CALLBACK(name) [](uv_async_t *a, int) { return Map::name(a); }
#else
#define UV_ASYNC_CALLBACK(name) name
#endif

    uv_async_init(**loop, async_terminate.get(), UV_ASYNC_CALLBACK(terminate));
    async_terminate->data = this;

    uv_async_init(**loop, async_render.get(), UV_ASYNC_CALLBACK(render));
    async_render->data = this;

    uv_async_init(**loop, async_cleanup.get(), UV_ASYNC_CALLBACK(cleanup));
    async_cleanup->data = this;

#undef UV_ASYNC_CALLBACK

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

    uv_async_send(async_terminate.get());

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
    mutex_run->lock();
    pausing = true;
    mutex_run->unlock();

    uv_stop(**loop);
    rerender(); // Needed to ensure uv_stop is seen and uv_run exits, otherwise we deadlock on wait_for_pause

    if (wait_for_pause) {
        mutex_pause->lock();
        while (!is_paused) {
            cond_pause->wait(*mutex_pause);
        }
        mutex_pause->unlock();
    }
}

void Map::resume() {
    assert(uv_thread_self() == main_thread);
    assert(async);

    mutex_run->lock();
    pausing = false;
    cond_run->broadcast();
    mutex_run->unlock();
}

void Map::run() {
#ifndef NDEBUG
    if (!async) {
        map_thread = main_thread;
    }
#endif
    assert(uv_thread_self() == map_thread);

    check_for_pause();

    setup();
    prepare();

    terminating = false;
    while(!terminating) {
        uv_run(**loop, UV_RUN_DEFAULT);
        check_for_pause();
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
}

void Map::check_for_pause() {
    mutex_run->lock();
    while (pausing) {
        view.make_inactive();

        mutex_pause->lock();
        is_paused = true;
        cond_pause->broadcast();
        mutex_pause->unlock();

        cond_run->wait(*mutex_run);

        view.make_active();
    }

    mutex_pause->lock();
    is_paused = false;
    mutex_pause->unlock();

    mutex_run->unlock();
}

void Map::rerender() {
    // We only send render events if we want to continuously update the map
    // (== async rendering).
    if (async) {
        uv_async_send(async_render.get());
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
        uv_async_send(async_cleanup.get());
    }
}

void Map::cleanup(uv_async_t *async) {
    Map *map = static_cast<Map *>(async->data);

    map->painter.cleanup();
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

void Map::render(uv_async_t *async) {
    Map *map = static_cast<Map *>(async->data);
    assert(uv_thread_self() == map->map_thread);

    if (map->state.hasSize()) {
        if (map->is_rendered.test_and_set() == false) {
            map->prepare();
            if (map->is_clean.test_and_set() == false) {
                map->render();
                map->is_swapped.clear();
                map->view.swap();
            } else {
                // We set the rendered flag in the test above, so we have to reset it
                // now that we're not actually rendering because the map is clean.
                map->is_rendered.clear();
            }
        }
    }
}

void Map::terminate(uv_async_t *async) {
    // Closes all open handles on the loop. This means that the loop will automatically terminate.
    Map *map = static_cast<Map *>(async->data);
    assert(uv_thread_self() == map->map_thread);

    // Remove all of these to make sure they are destructed in the correct thread.
    map->glyphStore.reset();
    map->fileSource.reset();
    map->style.reset();
    map->workers.reset();
    map->activeSources.clear();

    map->view.make_inactive();

    map->terminating = true;

    uv_close((uv_handle_t *)map->async_cleanup.get(), nullptr);
    uv_close((uv_handle_t *)map->async_render.get(), nullptr);
    uv_close((uv_handle_t *)map->async_terminate.get(), nullptr);
}

#pragma mark - Setup

void Map::setup() {
    assert(uv_thread_self() == map_thread);
    view.make_active();
    painter.setup();
}

void Map::setStyleURL(const std::string &url) {
    // TODO: Make threadsafe.
    styleURL = url;
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
        glyphStore = std::make_shared<GlyphStore>(fileSource);
    }
    fileSource->setBase(base);
    glyphStore->setURL(util::mapbox::normalizeGlyphsURL(style->glyph_url, getAccessToken()));
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
        sprite = Sprite::Create(sprite_url, pixelRatio, fileSource);
    }

    return sprite;
}


#pragma mark - Size

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::resize(uint16_t width, uint16_t height, float ratio) {
    resize(width, height, ratio, width * ratio, height * ratio);
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::resize(uint16_t width, uint16_t height, float ratio, uint16_t fb_width, uint16_t fb_height) {
    if (transform.resize(width, height, ratio, fb_width, fb_height)) {
        update();
    }
}

#pragma mark - Transitions

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::cancelTransitions() {
    transform.cancelTransitions();

    update();
}


#pragma mark - Position

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::moveBy(double dx, double dy, double duration) {
    transform.moveBy(dx, dy, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setLonLat(double lon, double lat, double duration) {
    transform.setLonLat(lon, lat, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::getLonLat(double& lon, double& lat) const {
    transform.getLonLat(lon, lat);
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::startPanning() {
    transform.startPanning();
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::stopPanning() {
    transform.stopPanning();
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::resetPosition() {
    transform.setAngle(0);
    transform.setLonLat(0, 0);
    transform.setZoom(0);
    update();
}


#pragma mark - Scale

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::scaleBy(double ds, double cx, double cy, double duration) {
    transform.scaleBy(ds, cx, cy, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setScale(double scale, double cx, double cy, double duration) {
    transform.setScale(scale, cx, cy, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
double Map::getScale() const {
    return transform.getScale();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setZoom(double zoom, double duration) {
    transform.setZoom(zoom, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
double Map::getZoom() const {
    return transform.getZoom();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setLonLatZoom(double lon, double lat, double zoom, double duration) {
    transform.setLonLatZoom(lon, lat, zoom, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::getLonLatZoom(double& lon, double& lat, double& zoom) const {
    transform.getLonLatZoom(lon, lat, zoom);
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::resetZoom() {
    setZoom(0);
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::startScaling() {
    transform.startScaling();
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
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

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::rotateBy(double sx, double sy, double ex, double ey, double duration) {
    transform.rotateBy(sx, sy, ex, ey, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setBearing(double degrees, double duration) {
    transform.setAngle(-degrees * M_PI / 180, duration * 1_second);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::setBearing(double degrees, double cx, double cy) {
    transform.setAngle(-degrees * M_PI / 180, cx, cy);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
double Map::getBearing() const {
    return -transform.getAngle() / M_PI * 180;
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::resetNorth() {
    transform.setAngle(0, 500_milliseconds);
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::startRotating() {
    transform.startRotating();
    update();
}

// Note: This function is called from another thread. Make sure you only call threadsafe functions!
void Map::stopRotating() {
    transform.stopRotating();
    update();
}

bool Map::canRotate() {
    return transform.canRotate();
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
    style->setAppliedClasses(classes);
    if (style->hasTransitions()) {
        update();
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
    style->setDefaultTransitionDuration(duration_milliseconds);
}

void Map::updateSources() {
    assert(uv_thread_self() == map_thread);

    // First, disable all existing sources.
    for (const util::ptr<StyleSource> &source : activeSources) {
        source->enabled = false;
    }

    // Then, reenable all of those that we actually use when drawing this layer.
    updateSources(style->layers);

    // Then, construct or destroy the actual source object, depending on enabled state.
    for (const util::ptr<StyleSource> &style_source : activeSources) {
        if (style_source->enabled) {
            if (!style_source->source) {
                style_source->source = std::make_shared<Source>(style_source->info);
                style_source->source->load(*this);
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

const std::set<util::ptr<StyleSource>> Map::getActiveSources() const {
    return activeSources;
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
    for (const util::ptr<StyleSource> &source : getActiveSources()) {
        source->source->update(*this);
    }
}

void Map::prepare() {
    if (!fileSource) {
        fileSource = std::make_shared<FileSource>(**loop, platform::defaultCacheDatabase());
        glyphStore = std::make_shared<GlyphStore>(fileSource);
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
