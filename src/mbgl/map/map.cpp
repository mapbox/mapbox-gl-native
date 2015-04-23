#include <mbgl/map/map.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/map/source.hpp>

#include <mbgl/style/style.hpp>

#include <mbgl/renderer/painter.hpp>

#include <mbgl/text/glyph_store.hpp>

#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>

#include <mbgl/util/std.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/thread.hpp>

#include <algorithm>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

namespace mbgl {

Map::Map(View& view, FileSource& fileSource, MapMode mode, bool startPaused)
    : env(util::make_unique<Environment>(fileSource)),
      scope(util::make_unique<EnvironmentScope>(*env, ThreadType::Main, "Main")),
      data(util::make_unique<MapData>(view, mode)),
      context(util::make_unique<util::Thread<MapContext>>("Map", *env, view, *data, startPaused))
{
    view.initialize(this);
}

Map::~Map() {
    resume();
}

void Map::pause(bool waitForPause) {
    assert(Environment::currentlyOn(ThreadType::Main));
    assert(data->mode == MapMode::Continuous);

    std::unique_lock<std::mutex> lockPause(data->mutexPause);
    context->invoke(&MapContext::pause);

    if (waitForPause) {
        data->condPaused.wait(lockPause);
    }
}

void Map::resume() {
    assert(Environment::currentlyOn(ThreadType::Main));

    data->condResume.notify_all();
}

void Map::renderStill(StillImageCallback fn) {
    assert(Environment::currentlyOn(ThreadType::Main));

    if (data->mode != MapMode::Still) {
        throw util::Exception("Map is not in still image render mode");
    }

    if (data->callback) {
        throw util::Exception("Map is currently rendering an image");
    }

    data->callback = std::move(fn);
    triggerUpdate(Update::RenderStill);
}

void Map::renderSync() {
    assert(Environment::currentlyOn(ThreadType::Main));

    context->invokeSync(&MapContext::render);
}

void Map::renderAsync() {
    assert(Environment::currentlyOn(ThreadType::Main));

    context->invoke(&MapContext::render);
}

void Map::update() {
    triggerUpdate();
}

#pragma mark - Setup

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

#pragma mark - Size

void Map::resize(uint16_t width, uint16_t height, float ratio) {
    if (data->transform.resize(width, height, ratio, width * ratio, height * ratio)) {
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

double Map::getMinZoom() const {
    return data->transform.getMinZoom();
}

double Map::getMaxZoom() const {
    return data->transform.getMaxZoom();
}


#pragma mark - Size

uint16_t Map::getWidth() const {
    return data->transform.currentState().getWidth();
}

uint16_t Map::getHeight() const {
    return data->transform.currentState().getHeight();
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
    data->annotationManager.setDefaultPointAnnotationSymbol(symbol);
}

double Map::getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol) {
    assert(Environment::currentlyOn(ThreadType::Main));
    return context->invokeSync<double>(&MapContext::getTopOffsetPixelsForAnnotationSymbol, symbol);
}

uint32_t Map::addPointAnnotation(const LatLng& point, const std::string& symbol) {
    return addPointAnnotations({ point }, { symbol }).front();
}

std::vector<uint32_t> Map::addPointAnnotations(const std::vector<LatLng>& points, const std::vector<std::string>& symbols) {
    assert(Environment::currentlyOn(ThreadType::Main));
    auto result = data->annotationManager.addPointAnnotations(points, symbols, *data);
    context->invoke(&MapContext::updateAnnotationTiles, result.first);
    return result.second;
}

void Map::removeAnnotation(uint32_t annotation) {
    assert(Environment::currentlyOn(ThreadType::Main));
    removeAnnotations({ annotation });
}

void Map::removeAnnotations(const std::vector<uint32_t>& annotations) {
    assert(Environment::currentlyOn(ThreadType::Main));
    auto result = data->annotationManager.removeAnnotations(annotations, *data);
    context->invoke(&MapContext::updateAnnotationTiles, result);
}

std::vector<uint32_t> Map::getAnnotationsInBounds(const LatLngBounds& bounds) {
    assert(Environment::currentlyOn(ThreadType::Main));
    return data->annotationManager.getAnnotationsInBounds(bounds, *data);
}

LatLngBounds Map::getBoundsForAnnotations(const std::vector<uint32_t>& annotations) {
    assert(Environment::currentlyOn(ThreadType::Main));
    return data->annotationManager.getBoundsForAnnotations(annotations);
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

void Map::setSourceTileCacheSize(size_t size) {
    context->invoke(&MapContext::setSourceTileCacheSize, size);
}

void Map::onLowMemory() {
    context->invoke(&MapContext::onLowMemory);
}

#pragma mark - Private

void Map::triggerUpdate(Update update_) {
    context->invoke(&MapContext::triggerUpdate, update_);
}

}
