#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/map/still_image.hpp>
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

Map::Map(View& view_, FileSource& fileSource_, RenderMode mode_)
    : view(view_),
      fileSource(fileSource_),
      renderMode(mode_),
      loop(util::make_unique<uv::loop>()),
      uiThread(std::this_thread::get_id()),
      transform(view_),
      active(false)
{
    // Setup async notifications
    asyncTerminate = util::make_unique<uv::async>(**loop, [this]() { terminate(); });
    asyncUpdate = util::make_unique<uv::async>(**loop, [this]() { updated(); });

    // Initialize the OpenGL context
    view.initialize(this);

    // Start the Map thread
    thread = std::thread([this]() { run(); });
}

Map::~Map() {
    assert(inUIThread());

    assert(asyncTerminate);
    asyncTerminate->send();

    thread.join();

    // Make sure that all of these are empty.
    assert(!sprite);
    assert(!style);
    assert(activeSources.empty());
    assert(!workers);
    assert(!painter);
    assert(!texturePool);
    assert(!lineAtlas);
    assert(!spriteAtlas);
    assert(!glyphStore);
    assert(!glyphAtlas);
}

uv::worker &Map::getWorker() {
    assert(workers);
    return *workers;
}

bool Map::inUIThread() const {
    return std::this_thread::get_id() == uiThread;
}

bool Map::inMapThread() const {
    return std::this_thread::get_id() == mapThread;
}

void Map::start() {
    assert(inUIThread());
    assert(renderMode == RenderMode::Continuous);
    assert(!active);

    active = true;

    update();

    // TODO: Send a notification to the thread to start rendering.
}

void Map::stop() {
    assert(inUIThread());
    assert(renderMode == RenderMode::Continuous);
    assert(active);

    active = false;

    // TODO: Send a notification to the thread to stop rendering.
}

void Map::renderStill(StillImageCallback fn) {
    assert(inUIThread());
    assert(renderMode == RenderMode::Still);
    assert(!active);
    assert(!callback);

    callback = std::move(fn);

    active = true;

    update();
}

// Triggers a refresh of the map based on changed UI state, e.g. the user moved the map viewport,
// or changed any other externally accessible value of the Map object.
void Map::update() {
    assert(asyncUpdate);
    asyncUpdate->send();
}

// Triggers a refresh of the map, following an updated bit of information the library itself
// triggered. This may be the availabilty of a new tile.
void Map::rerender() {
    assert(inMapThread());
    assert(asyncUpdate);
    if (renderMode == RenderMode::Still) {
        // Make sure the loop actually stays alive for the asyncUpdate callback to be invoked.
        asyncUpdate->ref();
    }
    asyncUpdate->send();
}

void Map::run() {
    mapThread = std::this_thread::get_id();

#ifdef __APPLE__
    pthread_setname_np("Map");
#endif

    view.activate();

    glyphAtlas = util::make_unique<GlyphAtlas>(1024, 1024);
    glyphStore = std::make_shared<GlyphStore>(fileSource);
    spriteAtlas = util::make_unique<SpriteAtlas>(512, 512);
    lineAtlas = util::make_unique<LineAtlas>(512, 512);
    texturePool = std::make_shared<TexturePool>();
    painter = util::make_unique<Painter>(*spriteAtlas, *glyphAtlas, *lineAtlas);
    workers = util::make_unique<uv::worker>(**loop, 4, "Tile Worker");

    setup();

    if (renderMode == RenderMode::Continuous) {
        runContinuous();
    } else if (renderMode == RenderMode::Still) {
        runStillImage();
    } else {
        abort();
    }

    sprite.reset();
    style.reset();
    activeSources.clear();
    workers.reset();
    painter.reset();
    texturePool.reset();
    lineAtlas.reset();
    spriteAtlas.reset();
    glyphStore.reset();
    glyphAtlas.reset();

    view.deactivate();
}

void Map::updated() {
    assert(inMapThread());

    if (renderMode == RenderMode::Continuous) {
        updatedContinuous();
    } else if (renderMode == RenderMode::Still) {
        updatedStillImage();
    } else {
        abort();
    }
}

void Map::runContinuous() {
    assert(inMapThread());

    // This loop will terminate once we Map object's destructor gets called and sends the
    // asyncTerminate signal.
    uv_run(**loop, UV_RUN_DEFAULT);
}

void Map::updatedContinuous() {
    if (active) {
        prepare();
        render();
        view.swap();
    }
}

void Map::runStillImage() {
    assert(inMapThread());

    // When rendering still images, the Map event loop will terminate after every map image. However,
    // the map thread stays alive, so we are restarting the event loop after every map image.
    while (true) {
        uv_run(**loop, UV_RUN_DEFAULT);

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
            active = false;

            // Now we can finally invoke the callback function with the map image we rendered.
            cb(std::move(image));

            // To prepare for the next event loop run, we have to make sure the async handles keep
            // the loop alive.
            asyncUpdate->ref();
            asyncTerminate->ref();
        } else {
            break;
        }
    }
}

void Map::updatedStillImage() {
    if (active) {
        // Those two async handles are keeping the event loop alive. However, we want it to
        // terminate after everything that is required for this image is loaded so we can do a final
        // render. Hence, we unref these handles so that the loop can terminate. Whenever something
        // calls rerender(), we are ref()ing the asyncUpdate handle to make sure the loop stays
        // alive and the asyncUpdate callback is actually fired.
        asyncUpdate->unref();
        asyncTerminate->unref();

        prepare();
    }
}

void Map::terminate() {
    assert(inMapThread());

    assert(asyncUpdate);
    assert(asyncTerminate);

    // Closes all open handles on the loop. This means that the loop will automatically terminate.
    asyncUpdate.reset();
    asyncTerminate.reset();
}

#pragma mark - Setup

void Map::setup() {
    assert(inMapThread());
    assert(painter);
    painter->setup();
}

void Map::setStyleURL(const std::string &url) {
    assert(inUIThread());
    styleURL = url;

    update();
}

void Map::loadStyleURL() {
    // TODO: figure out what thread this may be called from
    sprite.reset();
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


void Map::setStyleJSON(std::string newStyleJSON, const std::string &base) {
    // TODO: figure out what thread this may be called from
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

    glyphURL = util::mapbox::normalizeGlyphsURL(style->glyph_url, getAccessToken());

    update();
}

std::string Map::getStyleJSON() const {
    assert(inUIThread());
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

void Map::resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight) {
    // This may be called from the UI and Map thread.
    if (transform.resize(width, height, ratio, fbWidth, fbHeight)) {
        update();
    }
}

#pragma mark - Transitions

void Map::cancelTransitions() {
    assert(inUIThread());
    transform.cancelTransitions();

    update();
}


#pragma mark - Position

void Map::moveBy(double dx, double dy, std::chrono::steady_clock::duration duration) {
    assert(inUIThread());
    transform.moveBy(dx, dy, duration);
    update();
}

void Map::setLatLng(LatLng latLng, std::chrono::steady_clock::duration duration) {
    assert(inUIThread());
    transform.setLatLng(latLng, duration);
    update();
}

const LatLng Map::getLatLng() const {
    assert(inUIThread());
    return state.getLatLng();
}

void Map::startPanning() {
    assert(inUIThread());
    transform.startPanning();
    update();
}

void Map::stopPanning() {
    assert(inUIThread());
    transform.stopPanning();
    update();
}

void Map::resetPosition() {
    assert(inUIThread());
    transform.setAngle(0);
    transform.setLatLng(LatLng(0, 0));
    transform.setZoom(0);
    update();
}


#pragma mark - Scale

void Map::scaleBy(double ds, double cx, double cy, std::chrono::steady_clock::duration duration) {
    assert(inUIThread());
    transform.scaleBy(ds, cx, cy, duration);
    update();
}

void Map::setScale(double scale, double cx, double cy, std::chrono::steady_clock::duration duration) {
    assert(inUIThread());
    transform.setScale(scale, cx, cy, duration);
    update();
}

double Map::getScale() const {
    assert(inUIThread());
    return transform.getScale();
}

void Map::setZoom(double zoom, std::chrono::steady_clock::duration duration) {
    assert(inUIThread());
    transform.setZoom(zoom, duration);
    update();
}

double Map::getZoom() const {
    assert(inUIThread());
    return transform.getZoom();
}

void Map::setLatLngZoom(LatLng latLng, double zoom, std::chrono::steady_clock::duration duration) {
    assert(inUIThread());
    transform.setLatLngZoom(latLng, zoom, duration);
    update();
}

void Map::resetZoom() {
    assert(inUIThread());
    setZoom(0);
}

void Map::startScaling() {
    assert(inUIThread());
    transform.startScaling();
    update();
}

void Map::stopScaling() {
    assert(inUIThread());
    transform.stopScaling();
    update();
}

double Map::getMinZoom() const {
    // TODO: figure out what thread this may be called from
    return transform.getMinZoom();
}

double Map::getMaxZoom() const {
    // TODO: figure out what thread this may be called from
    return transform.getMaxZoom();
}


#pragma mark - Rotation

void Map::rotateBy(double sx, double sy, double ex, double ey, std::chrono::steady_clock::duration duration) {
    assert(inUIThread());
    transform.rotateBy(sx, sy, ex, ey, duration);
    update();
}

void Map::setBearing(double degrees, std::chrono::steady_clock::duration duration) {
    assert(inUIThread());
    transform.setAngle(-degrees * M_PI / 180, duration);
    update();
}

void Map::setBearing(double degrees, double cx, double cy) {
    assert(inUIThread());
    transform.setAngle(-degrees * M_PI / 180, cx, cy);
    update();
}

double Map::getBearing() const {
    assert(inUIThread());
    return -transform.getAngle() / M_PI * 180;
}

void Map::resetNorth() {
    assert(inUIThread());
    transform.setAngle(0, std::chrono::milliseconds(500));
    update();
}

void Map::startRotating() {
    assert(inUIThread());
    transform.startRotating();
    update();
}

void Map::stopRotating() {
    assert(inUIThread());
    transform.stopRotating();
    update();
}

#pragma mark - Access Token

void Map::setAccessToken(const std::string &token) {
    assert(inUIThread());
    accessToken = token;
}

const std::string &Map::getAccessToken() const {
    return accessToken;
}

#pragma mark - Toggles

void Map::setDebug(bool value) {
    assert(inUIThread());
    debug = value;
    update();
}

void Map::toggleDebug() {
    assert(inUIThread());
    setDebug(!debug);
}

bool Map::getDebug() const {
    assert(inUIThread());
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
    assert(inUIThread());
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
    assert(inUIThread());
    classes = classes_;
    if (style) {
        style->cascadeClasses(classes);
        if (style->hasTransitions()) {
            update();
        }
    }
}

bool Map::hasClass(const std::string& klass) const {
    assert(inUIThread());
    return std::find(classes.begin(), classes.end(), klass) != classes.end();
}

std::vector<std::string> Map::getClasses() const {
    assert(inUIThread());
   return classes;
}

void Map::setDefaultTransitionDuration(std::chrono::steady_clock::duration duration) {
    assert(inUIThread());
    defaultTransitionDuration = duration;
    if (style) {
        style->setDefaultTransitionDuration(duration);
    }
}

std::chrono::steady_clock::duration Map::getDefaultTransitionDuration() {
    assert(inUIThread());
    return defaultTransitionDuration;
}

void Map::updateSources() {
    assert(inMapThread());

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
                               *texturePool, fileSource, ***loop, [this](){ rerender(); });
    }
}

void Map::prepare() {
    if (!style && !styleURL.empty()) {
        loadStyleURL();
    }

    if (!glyphURL.empty()) {
        glyphStore->setURL(glyphURL);
        glyphURL.clear();
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
    painter->render(*style, activeSources, state, animationTime, debug);

    // Schedule another rerender when we definitely need a next frame.
    if (transform.needsTransition() || style->hasTransitions()) {
        update();
    }
}
