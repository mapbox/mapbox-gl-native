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

void Map::setup(float pixelRatio) {

    painter.setup();

    pixel_ratio = pixelRatio;

    style.sprite = std::make_shared<Sprite>();
    style.sprite->load(kSpriteURL, pixel_ratio);

    style.loadJSON(resources::style, resources::style_size);

    sources.emplace("mapbox streets", Source(*this, transform, painter, texturepool, "http://a.gl-api-us-east-1.tilestream.net/v3/mapbox.mapbox-streets-v4/%d/%d/%d.gl.pbf", pixelRatio, Source::Type::vector, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14}, 512, 0, 14, true));

    sources.emplace("raster", Source(*this, transform, painter, texturepool, "https://a.tiles.mapbox.com/v3/justin.map-pgygbwdm/%d/%d/%d%s.png256", pixelRatio, Source::Type::raster, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21}, 256, 0, 21, false));
}

void Map::loadStyle(const uint8_t *const data, uint32_t bytes) {
    style.loadJSON(data, bytes);
    update();
}

void Map::loadSettings() {
    transform.setAngle(settings.angle);
    transform.setScale(settings.scale);
    transform.setLonLat(settings.longitude, settings.latitude);
    style.cascade(transform.getNormalizedZoom());
    update();
}

void Map::resize(uint32_t width, uint32_t height, uint32_t fb_width, uint32_t fb_height) {
    transform.width = width;
    transform.height = height;
    transform.fb_width = fb_width;
    transform.fb_height = fb_height;
    transform.pixelRatio = pixel_ratio;
    painter.resize(fb_width, fb_height);
    update();
}

box Map::cornersToBox(uint32_t z) const {
    return transform.cornersToBox(z);
}

float Map::getPixelRatio() const {
    return pixel_ratio;
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
    style.cascade(transform.getNormalizedZoom());
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

void Map::rotateBy(double cx, double cy, double sx, double sy, double ex, double ey, double duration) {
    transform.rotateBy(cx, cy, sx, sy, ex, ey, duration);
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
    style.cascade(transform.getNormalizedZoom());
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
    style.cascade(transform.getNormalizedZoom());
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.persist();
}

void Map::setZoom(double zoom, double duration) {
    transform.setZoom(zoom, duration);
    style.cascade(transform.getNormalizedZoom());
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
        dx = (transform.width  / 2) - x;
        dy = (transform.height / 2) - y;
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
    style.cascade(transform.getNormalizedZoom());
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
    for (std::pair<std::string, Source&> pair : sources) {
        Source& source = pair.second;
        source.enabled = !source.enabled;
    }

    update();
}

void Map::cancelAnimations() {
    transform.cancelAnimations();
}

void Map::update() {
    updateTiles();
    platform::restart();
}

bool Map::updateTiles() {
    bool changed = false;

    for (std::pair<std::string, Source&> pair : sources) {
        Source& source = pair.second;
        if (source.enabled) {
            changed = source.update() || changed;
        }
    }

    return changed;
}

bool Map::render() {
    if (transform.needsAnimation()) {
        transform.updateAnimations();
    }

    if (*style.sprite->raster && !style.sprite->raster->textured) {
        style.sprite->raster->setTexturepool(&texturepool);
    }

    bool changed = updateTiles();

    painter.clear();

    painter.changeMatrix();

    // First, update all tile matrices with the new transform and render into
    // the stencil buffer.
//    uint8_t i = 1;
    
    painter.prepareClippingMask();

//    for (Tile& tile : tiles) {
//        if (tile.data && tile.data->state == TileData::State::parsed) {
//            // The position matrix.
//            transform.matrixFor(tile.matrix, tile.id);
//            matrix::multiply(tile.matrix, painter.projMatrix, tile.matrix);
//            tile.clip_id = i++;
//            painter.drawClippingMask(tile.matrix, tile.clip_id, !tile.data->use_raster);
//        }
//    }

    for (std::pair<std::string, Source&> pair : sources) {
        Source& source = pair.second;
        source.render();
    }

//    painter.finishClippingMask();

//    for (const Tile& tile : tiles) {
//        if (tile.data && tile.data->state == TileData::State::parsed) {
//            if (tile.data->use_raster && *tile.data->raster && !tile.data->raster->textured) {
//                tile.data->raster->setTexturepool(&texturepool);
//                tile.data->raster->beginFadeInAnimation();
//            }
//            if (tile.data->use_raster && tile.data->raster->needsAnimation()) {
//                tile.data->raster->updateAnimations();
//            }
//            painter.render(tile);
//        }
//    }

    painter.renderMatte();

    return changed || transform.needsAnimation() || painter.needsAnimation();
}
