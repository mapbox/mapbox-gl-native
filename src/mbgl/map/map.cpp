#include <mbgl/map/map.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/map_data.hpp>

#include <mbgl/util/projection.hpp>
#include <mbgl/util/thread.hpp>

#include <unordered_map>

namespace mbgl {

Map::Map(View& view, FileSource& fileSource, MapMode mode)
    : data(std::make_unique<MapData>(view, mode)),
      context(std::make_unique<util::Thread<MapContext>>(util::ThreadContext{"Map", util::ThreadType::Map, util::ThreadPriority::Regular}, view, fileSource, *data))
{
    view.initialize(this);
}

Map::~Map() {
    resume();
    context->invoke(&MapContext::cleanup);
}

void Map::pause() {
    assert(data->mode == MapMode::Continuous);

    if (!paused) {
        std::unique_lock<std::mutex> lockPause(data->mutexPause);
        context->invoke(&MapContext::pause);
        data->condPaused.wait(lockPause);
        paused = true;
    }
}

void Map::resume() {
    data->condResume.notify_all();
    paused = false;
}

void Map::renderStill(StillImageCallback callback) {
    context->invoke(&MapContext::renderStill, callback);
}

void Map::renderSync() {
    context->invokeSync(&MapContext::renderSync);
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

void Map::fitBounds(LatLngBounds bounds, EdgeInsets padding, Duration duration) {
    // Calculate the bounds of the possibly rotated `bounds` parameter with respect to the viewport.
    vec2<> nwPixel = pixelForLatLng({bounds.ne.latitude, bounds.sw.longitude});
    vec2<> swPixel = pixelForLatLng(bounds.sw);
    vec2<> sePixel = pixelForLatLng({bounds.sw.latitude, bounds.ne.longitude});
    vec2<> nePixel = pixelForLatLng(bounds.ne);
    vec2<> visualBounds = {
        (std::max(std::max(nwPixel.x, swPixel.x),
                  std::max(sePixel.x, nePixel.x)) -
         std::min(std::min(nwPixel.x, swPixel.x),
                  std::min(sePixel.x, nePixel.x))),
        (std::max(std::max(nwPixel.y, swPixel.y),
                  std::max(sePixel.y, nePixel.y)) -
         std::min(std::min(nwPixel.y, swPixel.y),
                  std::min(sePixel.y, nePixel.y))),
    };

    // Calculate the zoom level.
    double scaleX = (getWidth() - padding.left - padding.right) / visualBounds.x;
    double scaleY = (getHeight() - padding.top - padding.bottom) / visualBounds.y;
    double minScale = std::fmin(scaleX, scaleY);
    double zoom = std::log2(getScale() * minScale);
    zoom = std::fmax(std::fmin(zoom, getMaxZoom()), getMinZoom());

    // Calculate the center point of a virtual bounds that is extended in all directions by padding.
    vec2<> paddedNEPixel = {
        nePixel.x + padding.right / minScale,
        nePixel.y + padding.top / minScale,
    };
    vec2<> paddedSWPixel = {
        swPixel.x - padding.left / minScale,
        swPixel.y - padding.bottom / minScale,
    };
    vec2<> centerPixel = (paddedNEPixel + paddedSWPixel) * 0.5;
    LatLng centerLatLng = latLngForPixel(centerPixel);

    setLatLngZoom(centerLatLng, zoom, duration);
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

AnnotationIDs Map::addPointAnnotations(const AnnotationSegment& points,
                                       const std::vector<std::string>& symbols) {
    AnnotationsProperties properties = { { "symbols", symbols } };
    auto result = data->annotationManager.addPointAnnotations(points, properties, data->transform.getMaxZoom());
    context->invoke(&MapContext::updateAnnotationTiles, result.first);
    return result.second;
}

uint32_t Map::addShapeAnnotation(const AnnotationSegments& shape,
                                 const StyleProperties& styleProperties) {
    return addShapeAnnotations({ shape }, { styleProperties }).front();
}

AnnotationIDs Map::addShapeAnnotations(const std::vector<AnnotationSegments>& shapes,
                                       const std::vector<StyleProperties>& styleProperties) {
    auto result = data->annotationManager.addShapeAnnotations(shapes, styleProperties, {{}}, data->transform.getMaxZoom());
    context->invoke(&MapContext::updateAnnotationTiles, result.first);
    return result.second;
}

void Map::removeAnnotation(uint32_t annotation) {
    removeAnnotations({ annotation });
}

void Map::removeAnnotations(const std::vector<uint32_t>& annotations) {
    auto result = data->annotationManager.removeAnnotations(annotations, data->transform.getMaxZoom());
    context->invoke(&MapContext::updateAnnotationTiles, result);
}

std::vector<uint32_t> Map::getAnnotationsInBounds(const LatLngBounds& bounds, const AnnotationType& type) {
    return data->annotationManager.getAnnotationsInBounds(bounds, data->transform.getMaxZoom(), type);
}

LatLngBounds Map::getBoundsForAnnotations(const std::vector<uint32_t>& annotations) {
    return data->annotationManager.getBoundsForAnnotations(annotations);
}


#pragma mark - Toggles

void Map::setDebug(bool value) {
    data->setDebug(value);
    update();
}

void Map::toggleDebug() {
    data->toggleDebug();
    update();
}

bool Map::getDebug() const {
    return data->getDebug();
}

void Map::setCollisionDebug(bool value) {
    data->setCollisionDebug(value);
    update();
}

void Map::toggleCollisionDebug() {
    data->toggleCollisionDebug();
    update();
}

bool Map::getCollisionDebug() const {
    return data->getCollisionDebug();
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
