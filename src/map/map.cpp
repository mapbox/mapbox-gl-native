#include <llmr/map/map.hpp>
#include <llmr/map/source.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/style/resources.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/util/animation.hpp>
#include <llmr/util/time.hpp>

#include <algorithm>
#include <memory>

using namespace llmr;

Map::Map(View& view)
    : view(view),
      transform(),
      texturepool(),
      style(),
      glyphAtlas(1024, 1024),
      painter(*this),
      loop(uv_loop_new()) {

    // Make sure that we're doing an initial drawing in all cases.
    clean.clear();
    rendered.clear();
    swapped.test_and_set();
}

Map::~Map() {
    uv_loop_delete(loop);
    loop = nullptr;
}

void Map::start() {
    // When starting map rendering in another thread, we perform async/continuously
    // updated rendering. Only in these cases, we attach the async handlers.
    async = true;

    // Setup async notifications
    async_terminate = new uv_async_t();
    uv_async_init(loop, async_terminate, terminate);
    async_terminate->data = loop;

    async_render = new uv_async_t();
    uv_async_init(loop, async_render, render);
    async_render->data = this;

    uv_thread_create(&thread, [](void *arg) {
        Map *map = static_cast<Map *>(arg);
        map->run();
    }, this);
}

void Map::stop() {
    uv_async_send(async_terminate);
    uv_thread_join(&thread);

    uv_close((uv_handle_t *)async_terminate, delete_async);
    async_terminate = nullptr;
    uv_close((uv_handle_t *)async_render, delete_async);
    async_render = nullptr;

    // Run the event loop once to make sure our async delete handlers are called.
    uv_run(loop, UV_RUN_ONCE);

    async = false;
}

void Map::delete_async(uv_handle_t *handle) {
    delete (uv_async_t *)handle;
}

void Map::run() {
    setup();
    prepare();
    uv_run(loop, UV_RUN_DEFAULT);

    // If the map rendering wasn't started asynchronously, we perform one render
    // *after* all events have been processed.
    if (!async) {
        render();
    }
}

void Map::rerender() {
    // We only send render events if we want to continuously update the map
    // (== async rendering).
    if (async) {
        uv_async_send(async_render);
    }
}

void Map::update() {
    clean.clear();
    rerender();
}

void Map::render(uv_async_t *async) {
    Map *map = static_cast<Map *>(async->data);

    map->prepare();

    if (map->rendered.test_and_set() == false) {
        if (map->clean.test_and_set() == false) {
            map->render();
            map->swapped.clear();
            map->view.swap();
        } else {
            // We set the rendered flag in the test above, so we have to reset it
            // now that we're not actually rendering because the map is clean.
            map->rendered.clear();
        }
    }
}

void Map::terminate(uv_async_t *async) {
    uv_stop(static_cast<uv_loop_t *>(async->data));
}

#pragma mark - Setup

void Map::setup() {
    view.make_active();

    painter.setup();

    sources.emplace("mapbox streets",
                    std::unique_ptr<Source>(new Source(*this,
                           painter,
                           texturepool,
                           "http://a.gl-api-us-east-1.tilestream.net/v3/mapbox.mapbox-streets-v4/%d/%d/%d.gl.pbf",
                           Source::Type::vector,
                           {{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 }},
                           512,
                           0,
                           14,
                           true)));

    sources.emplace("satellite",
                    std::unique_ptr<Source>(new Source(*this,
                           painter,
                           texturepool,
                           "https://a.tiles.mapbox.com/v3/justin.hh0gkdfm/%d/%d/%d%s.png256",
                           Source::Type::raster,
                           {{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 }},
                           256,
                           0,
                           21,
                           false)));

    loadStyle(resources::style, resources::style_size);
}

void Map::loadStyle(const uint8_t *const data, uint32_t bytes) {
    style.loadJSON(data, bytes);
    update();
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

#pragma mark - Animations

void Map::cancelAnimations() {
    transform.cancelAnimations();

    update();
}


#pragma mark - Position

void Map::moveBy(double dx, double dy, double duration) {
    transform.moveBy(dx, dy, duration);
    update();
}

void Map::setLonLat(double lon, double lat, double duration) {
    transform.setLonLat(lon, lat, duration);
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
    transform.scaleBy(ds, cx, cy, duration);
    update();
}

void Map::setScale(double scale, double cx, double cy, double duration) {
    transform.setScale(scale, cx, cy, duration);
    update();
}

double Map::getScale() const {
    return transform.getScale();
}

void Map::setZoom(double zoom, double duration) {
    transform.setZoom(zoom, duration);
    update();
}

double Map::getZoom() const {
    return transform.getZoom();
}

void Map::setLonLatZoom(double lon, double lat, double zoom, double duration) {
    transform.setLonLatZoom(lon, lat, zoom, duration);
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


#pragma mark - Rotation

void Map::rotateBy(double sx, double sy, double ex, double ey, double duration) {
    transform.rotateBy(sx, sy, ex, ey, duration);
    update();
}

void Map::setAngle(double angle, double cx, double cy) {
    transform.setAngle(angle, cx, cy);
    update();
}

double Map::getAngle() const {
    return transform.getAngle();
}

void Map::resetNorth() {
    transform.setAngle(0, 0.5);
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

void Map::toggleRaster() {
    auto it = sources.find("satellite");
    if (it != sources.end()) {
        Source &satellite_source = *it->second;

        if (satellite_source.enabled) {
            satellite_source.enabled = false;
            style.appliedClasses.erase(style.appliedClasses.find("satellite"));
        } else {
            satellite_source.enabled = true;
            style.appliedClasses.insert("satellite");
        }
    }

    update();
}

void Map::updateTiles() {
    for (auto &pair : sources) {
        const std::unique_ptr<Source> &source = pair.second;
        if (source->enabled) {
            source->update();
        }
    }
}

void Map::prepare() {
    view.make_active();

    // Update animations
    animationTime = (double)util::now() / 1e9;
    bool animating = transform.needsAnimation();
    if (animating) {
        transform.updateAnimations(animationTime);
    }

    state = transform.currentState();
    style.cascade(state.getNormalizedZoom());
    if (!style.sprite || style.sprite->pixelRatio != state.getPixelRatio()) {
        style.sprite = std::make_shared<Sprite>(*this, state.getPixelRatio());
        style.sprite->load(kSpriteURL);
    }

    updateTiles();
}

void Map::render() {
    painter.clear();

    painter.resize();

    painter.changeMatrix();

    // First, update the sources' tile matrices with the new
    // transform and render into the stencil buffer, including
    // drawing the background.

    painter.prepareClippingMask();

    bool is_baselayer = true;

    for (auto &pair : sources) {
        Source &source = *pair.second;
        source.prepare_render(state, is_baselayer);
        is_baselayer = source.enabled ? false : (true && is_baselayer);
    }

    painter.finishClippingMask();

    // Then, render each source's tiles. TODO: handle more than one
    // vector source.

    for (auto &pair : sources) {
        Source &source = *pair.second;
        source.render(animationTime);
    }

    painter.renderMatte();

    // Schedule another rerender when we definitely need a next frame.
    if (transform.needsAnimation()) {
        update();
    }
}
