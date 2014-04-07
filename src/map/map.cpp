#include <llmr/map/map.hpp>
#include <llmr/map/settings.hpp>

#include <algorithm>

using namespace llmr;

Map::Map(Settings& settings)
    : dirty(true),
      swap(false),
      transform(),
      settings(settings),
      texturepool(),
      style(),
      glyphAtlas(1024, 1024),
      painter(transform, style, glyphAtlas),
      min_zoom(0),
      max_zoom(kTileVectorMaxZoom),
      debug(0)
{
    uv_async_init(*render_loop, &async_transform, async_transform_cb);
    async_transform.data = this;

    uv_async_init(*render_loop, &async_render, async_render_cb);
    async_render.data = this;

    uv_async_init(*render_loop, &async_terminate, async_terminate_cb);
    async_terminate.data = this;
}

void Map::start(ViewContext *ctx) {
    assert(context == nullptr);
    context = ctx;
    uv_thread_create(&render_thread, loop, this);

    loadSettings();
}

void Map::stop() {
    assert(context != nullptr);
    uv_async_send(&async_terminate);
    uv_thread_join(&render_thread);
    context = nullptr;
}

bool Map::can_swap() const {
    return swap;
}

void Map::swapped() {
    swap = false;
    settings.persist(config);
    rerender();
}

// Thread safe. Schedules a rerender.
void Map::rerender() {
    uv_async_send(&async_render);
}

void Map::scheduleTransform(const TransformCommand &cmd) {
    std::lock_guard<std::mutex> lock(transform_commands_lock);
    transform_commands.push(cmd);
}

void Map::resize(uint16_t width, uint16_t height, uint16_t fb_width, uint16_t fb_height) {
    scheduleTransform(TransformResizeCommand{ width, height, fb_width, fb_height });
    uv_async_send(&async_transform);
}

void Map::moveBy(int16_t dx, int16_t dy, float duration) {
    scheduleTransform({ TransformMoveByCommand{ dx, dy }, duration });
    uv_async_send(&async_transform);
}

void Map::resetPosition() {
    {
        std::lock_guard<std::mutex> lock(transform_commands_lock);
        transform_commands.push(TransformAngleCommand{ 0 });
        transform_commands.push(TransformScaleCommand{ 1 });
        transform_commands.push(TransformLonLatCommand{ 0, 0 });
    }
    uv_async_send(&async_transform);
}

void Map::scaleBy(double ds, int16_t cx, int16_t cy, float duration) {
    scheduleTransform({ TransformScaleByCommand { ds, cx, cy }, duration});
    uv_async_send(&async_transform);
}

void Map::rotateBy(int16_t cx, int16_t cy, int16_t sx, int16_t sy, int16_t ex, int16_t ey, float duration) {
    scheduleTransform({ TransformRotateByCommand { cx, cy, sx, sy, ex, ey }, duration});
    uv_async_send(&async_transform);
}

void Map::resetNorth() {
    scheduleTransform({ TransformAngleCommand { 0 }, 500_milliseconds });
    uv_async_send(&async_transform);
}

void Map::toggleDebug() {
    debug ^= 0xFF;
    scheduleTransform({});
    uv_async_send(&async_transform);
}

void Map::setTransforming(bool value) {
    scheduleTransform({ TransformTransformCommand { value }, 200_milliseconds });
    uv_async_send(&async_transform);
}

// void Map::setup(float pixelRatio) {

//     painter.setup();

//     pixel_ratio = pixelRatio;

//     style.sprite = std::make_shared<Sprite>();
//     style.sprite->load(kSpriteURL, pixel_ratio);

//     style.loadJSON(resources::style, resources::style_size);
// }

// void Map::loadStyle(const uint8_t *const data, uint32_t bytes) {
//     style.loadJSON(data, bytes);
//     update();
// }

void Map::loadSettings() {
    Configuration config = settings.load();
    {
        std::lock_guard<std::mutex> lock(transform_commands_lock);
        transform_commands.push(TransformAngleCommand{ config.angle });
        transform_commands.push(TransformScaleCommand{ config.scale });
        transform_commands.push(TransformLonLatCommand{ config.longitude, config.latitude });
    }
    uv_async_send(&async_transform);
}

// void Map::resize(uint32_t width, uint32_t height, uint32_t fb_width, uint32_t fb_height) {
//     transform.width = width;
//     transform.height = height;
//     transform.fb_width = fb_width;
//     transform.fb_height = fb_height;
//     transform.pixelRatio = pixel_ratio;
//     painter.resize(fb_width, fb_height);
//     update();
// }

// void Map::toggleRaster() {
//     use_raster = ! use_raster;
//     max_zoom = (use_raster ? kTileRasterMaxZoom : kTileVectorMaxZoom);
//     tiles.clear();
//     update();
// }

// void Map::moveBy(double dx, double dy, double duration) {
//     transform.moveBy(dx, dy, duration);
//     update();

//     transform.getLonLat(settings.longitude, settings.latitude);
//     settings.persist();
// }

// void Map::startPanning() {
//     transform.startPanning();
//     platform::restart();
// }

// void Map::stopPanning() {
//     transform.stopPanning();
//     platform::restart();
// }

// void Map::scaleBy(double ds, double cx, double cy, double duration) {
//     transform.scaleBy(ds, cx, cy, duration);
//     style.cascade(transform.getNormalizedZoom());
//     update();

//     transform.getLonLat(settings.longitude, settings.latitude);
//     settings.scale = transform.getScale();
//     settings.persist();
// }

// void Map::startScaling() {
//     transform.startScaling();
//     platform::restart();
// }

// void Map::stopScaling() {
//     transform.stopScaling();
//     platform::restart();
// }

// void Map::rotateBy(double cx, double cy, double sx, double sy, double ex, double ey, double duration) {
//     transform.rotateBy(cx, cy, sx, sy, ex, ey, duration);
//     update();

//     settings.angle = transform.getAngle();
//     settings.persist();
// }

// void Map::startRotating() {
//     transform.startRotating();
//     platform::restart();
// }

// void Map::stopRotating() {
//     transform.stopRotating();
//     platform::restart();
// }

// void Map::setLonLat(double lon, double lat, double duration) {
//     transform.setLonLat(lon, lat, duration);
//     update();

//     transform.getLonLat(settings.longitude, settings.latitude);
//     settings.persist();
// }

// void Map::getLonLat(double& lon, double& lat) const {
//     transform.getLonLat(lon, lat);
// }

// void Map::setLonLatZoom(double lon, double lat, double zoom, double duration) {
//     transform.setLonLatZoom(lon, lat, zoom, duration);
//     style.cascade(transform.getNormalizedZoom());
//     update();

//     transform.getLonLat(settings.longitude, settings.latitude);
//     settings.scale = transform.getScale();
//     settings.persist();
// }

// void Map::getLonLatZoom(double& lon, double& lat, double& zoom) const {
//     transform.getLonLatZoom(lon, lat, zoom);
// }

// void Map::setScale(double scale, double cx, double cy, double duration) {
//     transform.setScale(scale, cx, cy, duration);
//     style.cascade(transform.getNormalizedZoom());
//     update();

//     transform.getLonLat(settings.longitude, settings.latitude);
//     settings.scale = transform.getScale();
//     settings.persist();
// }

// void Map::setZoom(double zoom, double duration) {
//     transform.setZoom(zoom, duration);
//     style.cascade(transform.getNormalizedZoom());
//     update();

//     transform.getLonLat(settings.longitude, settings.latitude);
//     settings.scale = transform.getScale();
//     settings.persist();
// }

// double Map::getZoom() const {
//     return transform.getZoom();
// }

// void Map::setAngle(double angle, double x, double y, double duration) {
//     double dx = 0, dy = 0;

//     if (x >= 0 && y >= 0) {
//         dx = (transform.width  / 2) - x;
//         dy = (transform.height / 2) - y;
//         transform.moveBy(dx, dy, duration);
//     }

//     transform.setAngle(angle, duration);

//     if (x >= 0 && y >= 0) {
//         transform.moveBy(-dx, -dy, duration);
//     }

//     update();

//     transform.getLonLat(settings.longitude, settings.latitude);
//     settings.angle = transform.getAngle();
//     settings.persist();
// }

// double Map::getScale() const {
//     return transform.getScale();
// }

// double Map::getAngle() const {
//     return transform.getAngle();
// }

// void Map::resetNorth() {
//     transform.setAngle(0, 0.5);
//     update();

//     settings.angle = transform.getAngle();
//     settings.persist();
// }

// void Map::resetPosition() {
//     transform.setAngle(0);
//     transform.setLonLat(0, 0);
//     transform.setZoom(0);
//     style.cascade(transform.getNormalizedZoom());
//     update();

//     transform.getLonLat(settings.longitude, settings.latitude);
//     settings.scale = transform.getScale();
//     settings.angle = transform.getAngle();
//     settings.persist();
// }

// void Map::resetZoom() {
//     setZoom(0);
// }

// void Map::cancelAnimations() {
//     transform.cancelAnimations();
// }

// void Map::update() {
//     updateTiles();
//     platform::restart();
// }


