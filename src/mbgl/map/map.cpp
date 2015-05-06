#include <mbgl/map/map.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/map_data.hpp>

#include <mbgl/util/std.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/thread.hpp>

namespace mbgl {

Map::Map(View& view, FileSource& fileSource, MapMode mode, bool startPaused)
    : data(util::make_unique<MapData>(view, mode)),
      context(util::make_unique<util::Thread<MapContext>>("Map", util::ThreadPriority::Regular, view, fileSource, *data, startPaused))
{
    view.initialize(this);
}

Map::~Map() {
    resume();
}

void Map::pause() {
    assert(data->mode == MapMode::Continuous);

    std::unique_lock<std::mutex> lockPause(data->mutexPause);
    context->invoke(&MapContext::pause);
    data->condPaused.wait(lockPause);
}

void Map::resume() {
    data->condResume.notify_all();
}

void Map::renderStill(StillImageCallback callback) {
    context->invoke(&MapContext::renderStill, callback);
}

void Map::renderSync() {
    context->invokeSync(&MapContext::render);
}

void Map::renderAsync() {
    context->invoke(&MapContext::render);
}

void Map::update(Update update_) {
    context->invoke(&MapContext::triggerUpdate, update_);
}

#pragma mark - Style

void Map::setStyleURL(const std::string &url) {
    context->invoke(&MapContext::setStyleURL, url);
}

void Map::setStyleJSON(const std::string& json, const std::string& base) {
    context->invoke(&MapContext::setStyleJSON, json, base);
}

std::string Map::getStyleURL() const {
    return context->invokeSync<std::string>(&MapContext::getStyleURL);
}

std::string Map::getStyleJSON() const {
    return context->invokeSync<std::string>(&MapContext::getStyleJSON);
}

#pragma mark - Size

void Map::resize(uint16_t width, uint16_t height, float ratio) {
    if (data->transform.resize(width, height, ratio, width * ratio, height * ratio)) {
        context->invoke(&MapContext::resize, width, height, ratio);
    }
}

#pragma mark - Transitions

void Map::cancelTransitions() {
    data->transform.cancelTransitions();
    update();
}

void Map::setGestureInProgress(bool inProgress) {
    data->transform.setGestureInProgress(inProgress);
    update();
}

#pragma mark - Position

void Map::moveBy(double dx, double dy, Duration duration) {
    data->transform.moveBy(dx, dy, duration);
    update();
}

void Map::setLatLng(LatLng latLng, Duration duration) {
    data->transform.setLatLng(latLng, duration);
    update();
}

LatLng Map::getLatLng() const {
    return data->transform.getLatLng();
}

void Map::resetPosition() {
    data->transform.setAngle(0);
    data->transform.setLatLng(LatLng(0, 0));
    data->transform.setZoom(0);
    update(Update::Zoom);
}


#pragma mark - Scale

void Map::scaleBy(double ds, double cx, double cy, Duration duration) {
    data->transform.scaleBy(ds, cx, cy, duration);
    update(Update::Zoom);
}

void Map::setScale(double scale, double cx, double cy, Duration duration) {
    data->transform.setScale(scale, cx, cy, duration);
    update(Update::Zoom);
}

double Map::getScale() const {
    return data->transform.getScale();
}

void Map::setZoom(double zoom, Duration duration) {
    data->transform.setZoom(zoom, duration);
    update(Update::Zoom);
}

double Map::getZoom() const {
    return data->transform.getZoom();
}

void Map::setLatLngZoom(LatLng latLng, double zoom, Duration duration) {
    data->transform.setLatLngZoom(latLng, zoom, duration);
    update(Update::Zoom);
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
    update();
}

void Map::setBearing(double degrees, Duration duration) {
    data->transform.setAngle(-degrees * M_PI / 180, duration);
    update();
}

void Map::setBearing(double degrees, double cx, double cy) {
    data->transform.setAngle(-degrees * M_PI / 180, cx, cy);
    update();
}

double Map::getBearing() const {
    return -data->transform.getAngle() / M_PI * 180;
}

void Map::resetNorth() {
    data->transform.setAngle(0, std::chrono::milliseconds(500));
    update();
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
    data->annotationManager.setDefaultPointAnnotationSymbol(symbol);
}

double Map::getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol) {
    return context->invokeSync<double>(&MapContext::getTopOffsetPixelsForAnnotationSymbol, symbol);
}

uint32_t Map::addPointAnnotation(const LatLng& point, const std::string& symbol) {
    return addPointAnnotations({ point }, { symbol }).front();
}

std::vector<uint32_t> Map::addPointAnnotations(const std::vector<LatLng>& points, const std::vector<std::string>& symbols) {
    auto result = data->annotationManager.addPointAnnotations(points, symbols, *data);
    context->invoke(&MapContext::updateAnnotationTiles, result.first);
    return result.second;
}

void Map::removeAnnotation(uint32_t annotation) {
    removeAnnotations({ annotation });
}

void Map::removeAnnotations(const std::vector<uint32_t>& annotations) {
    auto result = data->annotationManager.removeAnnotations(annotations, *data);
    context->invoke(&MapContext::updateAnnotationTiles, result);
}

std::vector<uint32_t> Map::getAnnotationsInBounds(const LatLngBounds& bounds) {
    return data->annotationManager.getAnnotationsInBounds(bounds, *data);
}

LatLngBounds Map::getBoundsForAnnotations(const std::vector<uint32_t>& annotations) {
    return data->annotationManager.getBoundsForAnnotations(annotations);
}


#pragma mark - Toggles

void Map::setDebug(bool value) {
    data->setDebug(value);
    update(Update::Debug);
}

void Map::toggleDebug() {
    data->toggleDebug();
    update(Update::Debug);
}

bool Map::getDebug() const {
    return data->getDebug();
}

bool Map::isFullyLoaded() const {
    return data->getFullyLoaded();
}

void Map::addClass(const std::string& klass) {
    if (data->addClass(klass)) {
        update(Update::Classes);
    }
}

void Map::removeClass(const std::string& klass) {
    if (data->removeClass(klass)) {
        update(Update::Classes);
    }
}

void Map::setClasses(const std::vector<std::string>& classes) {
    data->setClasses(classes);
    update(Update::Classes);
}

bool Map::hasClass(const std::string& klass) const {
    return data->hasClass(klass);
}

std::vector<std::string> Map::getClasses() const {
    return data->getClasses();
}

void Map::setDefaultTransitionDuration(Duration duration) {
    data->setDefaultTransitionDuration(duration);
    update(Update::DefaultTransitionDuration);
}

Duration Map::getDefaultTransitionDuration() {
    return data->getDefaultTransitionDuration();
}

void Map::setSourceTileCacheSize(size_t size) {
    context->invoke(&MapContext::setSourceTileCacheSize, size);
}

void Map::onLowMemory() {
    context->invoke(&MapContext::onLowMemory);
}

}
