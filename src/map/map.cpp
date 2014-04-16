#include <llmr/map/map.hpp>
#include <llmr/map/settings.hpp>
#include <llmr/map/source.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/style/resources.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/util/animation.hpp>

#include <algorithm>
#include <memory>

using namespace llmr;

Map::Map(Settings& settings)
    : settings(settings),
      transform(),
      texturepool(),
      style(),
      glyphAtlas(1024, 1024),
      painter(transform, settings, style, glyphAtlas),
      min_zoom(0),
      max_zoom(21) {
}

Map::~Map() {
    settings.sync();
}

void Map::setup() {
    painter.setup();

    style.loadJSON(resources::style, resources::style_size);

    sources.emplace("mapbox streets",
                    std::unique_ptr<Source>(new Source(*this,
                           transform,
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
                           transform,
                           painter,
                           texturepool,
                           "https://a.tiles.mapbox.com/v3/justin.hh0gkdfm/%d/%d/%d%s.png256",
                           Source::Type::raster,
                           {{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 }},
                           256,
                           0,
                           21,
                           false)));
}

void Map::loadStyle(const uint8_t *const data, uint32_t bytes) {
    style.loadJSON(data, bytes);
    update();
}

void Map::loadSettings() {
    transform.setAngle(settings.angle);
    transform.setScale(settings.scale);
    transform.setLonLat(settings.longitude, settings.latitude);
    update();
}

void Map::resize(uint16_t width, uint16_t height, float ratio) {
    resize(width, height, ratio, width * ratio, height * ratio);
}

void Map::resize(uint16_t width, uint16_t height, float ratio, uint16_t fb_width, uint16_t fb_height) {
    transform.resize(width, height, ratio, fb_width, fb_height);
    painter.resize();

    if (!style.sprite || style.sprite->pixelRatio != transform.getPixelRatio()) {
        style.sprite = std::make_shared<Sprite>(transform.getPixelRatio());
        style.sprite->load(kSpriteURL);
    }
}

box Map::cornersToBox(uint32_t z) const {
    return transform.cornersToBox(z);
}

float Map::getPixelRatio() const {
    return transform.getPixelRatio();
}

Style& Map::getStyle() {
    return style;
}

GlyphAtlas& Map::getGlyphAtlas() {
    return glyphAtlas;
}

void Map::moveBy(double dx, double dy, double duration) {
    transform.moveBy(dx, dy, duration);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.persist();
}

void Map::startPanning() {
    transform.startPanning();
    platform::restart();
}

void Map::stopPanning() {
    transform.stopPanning();
    platform::restart();
}

void Map::scaleBy(double ds, double cx, double cy, double duration) {
    transform.scaleBy(ds, cx, cy, duration);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.persist();
}

void Map::startScaling() {
    transform.startScaling();
    platform::restart();
}

void Map::stopScaling() {
    transform.stopScaling();
    platform::restart();
}

void Map::rotateBy(double sx, double sy, double ex, double ey, double duration) {
    transform.rotateBy(sx, sy, ex, ey, duration);
    update();

    settings.angle = transform.getAngle();
    settings.persist();
}

void Map::startRotating() {
    transform.startRotating();
    platform::restart();
}

void Map::stopRotating() {
    transform.stopRotating();
    platform::restart();
}

void Map::setLonLat(double lon, double lat, double duration) {
    transform.setLonLat(lon, lat, duration);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.persist();
}

void Map::getLonLat(double& lon, double& lat) const {
    transform.getLonLat(lon, lat);
}

void Map::setLonLatZoom(double lon, double lat, double zoom, double duration) {
    transform.setLonLatZoom(lon, lat, zoom, duration);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.persist();
}

void Map::getLonLatZoom(double& lon, double& lat, double& zoom) const {
    transform.getLonLatZoom(lon, lat, zoom);
}

void Map::setScale(double scale, double cx, double cy, double duration) {
    transform.setScale(scale, cx, cy, duration);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.persist();
}

void Map::setZoom(double zoom, double duration) {
    transform.setZoom(zoom, duration);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.persist();
}

double Map::getZoom() const {
    return transform.getZoom();
}

void Map::setAngle(double angle, double x, double y, double duration) {
    double dx = 0, dy = 0;

    if (x >= 0 && y >= 0) {
        dx = (transform.getWidth() / 2) - x;
        dy = (transform.getWidth() / 2) - y;
        transform.moveBy(dx, dy, duration);
    }

    transform.setAngle(angle, duration);

    if (x >= 0 && y >= 0) {
        transform.moveBy(-dx, -dy, duration);
    }

    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.angle = transform.getAngle();
    settings.persist();
}

double Map::getScale() const {
    return transform.getScale();
}

double Map::getAngle() const {
    return transform.getAngle();
}

void Map::resetNorth() {
    transform.setAngle(0, 0.5);
    update();

    settings.angle = transform.getAngle();
    settings.persist();
}

void Map::resetPosition() {
    transform.setAngle(0);
    transform.setLonLat(0, 0);
    transform.setZoom(0);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.angle = transform.getAngle();
    settings.persist();
}

void Map::resetZoom() {
    setZoom(0);
}

void Map::toggleDebug() {
    settings.debug = !settings.debug;
    update();

    settings.persist();
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

        style.cascade(transform.getNormalizedZoom());

        update();
    }
}

void Map::cancelAnimations() {
    transform.cancelAnimations();
}

void Map::update() {
    // Loads new tiles and removes old ones.
    updateTiles();

    // Updates the stylesheet to the new zoom level
    style.cascade(transform.getNormalizedZoom());

    // Schedules a repaint
    platform::restart();
}

bool Map::updateTiles() {
    bool changed = false;

    for (auto &pair : sources) {
        Source &source = *pair.second;
        if (source.enabled) {
            changed = source.update() || changed;
        }
    }

    return changed;
}

bool Map::render() {
    double animationTime = platform::elapsed();

    if (transform.needsAnimation()) {
        transform.updateAnimations(animationTime);

        // Animations change the transform which affects the stylesheet.
        style.cascade(transform.getNormalizedZoom());
    }

    if (*style.sprite->raster && !style.sprite->raster->textured) {
        style.sprite->raster->setTexturepool(&texturepool);
    }

    bool changed = updateTiles();

    painter.clear();

    painter.changeMatrix();

    // First, update the sources' tile matrices with the new
    // transform and render into the stencil buffer, including
    // drawing the background.

    painter.prepareClippingMask();

    bool is_baselayer = true;

    for (auto &pair : sources) {
        Source &source = *pair.second;
        source.prepare_render(is_baselayer);
        is_baselayer = source.enabled ? false : (true && is_baselayer);
    }

    painter.finishClippingMask();

    // Then, render each source's tiles. TODO: handle more than one
    // vector source.

    is_baselayer = true;

    for (auto &pair : sources) {
        Source &source = *pair.second;
        source.render(animationTime, is_baselayer);
        is_baselayer = source.enabled ? false : (true && is_baselayer);
    }

    painter.renderMatte();


    bool transition = transform.needsAnimation();
    bool animation = painter.needsAnimation();

    return changed || transition || animation;
}
