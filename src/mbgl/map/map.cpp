#include <mbgl/map/map.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>

#include <mbgl/util/projection.hpp>
#include <mbgl/util/thread.hpp>

namespace mbgl {

Map::Map(View& view_, FileSource& fileSource, MapMode mode)
    : view(view_),
      transform(std::make_unique<Transform>(view)),
      data(std::make_unique<MapData>(mode, view.getPixelRatio())),
      context(std::make_unique<util::Thread<MapContext>>(util::ThreadContext{"Map", util::ThreadType::Map, util::ThreadPriority::Regular}, view, fileSource, *data))
{
    view.initialize(this);
    update(Update::Dimensions);
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
    context->invoke(&MapContext::renderStill, transform->getState(),
                    FrameData{ view.getFramebufferSize() }, callback);
}

void Map::renderSync() {
    if (renderState == RenderState::never) {
        view.notifyMapChange(MapChangeWillStartRenderingMap);
    }

    view.notifyMapChange(MapChangeWillStartRenderingFrame);

    MapContext::RenderResult result = context->invokeSync<MapContext::RenderResult>(
        &MapContext::renderSync, transform->getState(), FrameData{ view.getFramebufferSize() });

    view.notifyMapChange(result.fullyLoaded ?
        MapChangeDidFinishRenderingFrameFullyRendered :
        MapChangeDidFinishRenderingFrame);

    if (!result.fullyLoaded) {
        renderState = RenderState::partial;
    } else if (renderState != RenderState::fully) {
        renderState = RenderState::fully;
        view.notifyMapChange(MapChangeDidFinishRenderingMapFullyRendered);
    }

    if (transform->needsTransition()) {
        update(Update(transform->updateTransitions(Clock::now())));
    } else if (result.needsRerender) {
        update();
    }
}

void Map::update(Update update_) {
    if (update_ == Update::Dimensions) {
        transform->resize(view.getSize());
    }

    context->invoke(&MapContext::triggerUpdate, transform->getState(), update_);
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

#pragma mark - Transitions

void Map::cancelTransitions() {
    transform->cancelTransitions();
    update();
}

void Map::setGestureInProgress(bool inProgress) {
    transform->setGestureInProgress(inProgress);
    update();
}

#pragma mark - Position

void Map::moveBy(double dx, double dy, Duration duration) {
    transform->moveBy(dx, dy, duration);
    update();
}

void Map::setLatLng(LatLng latLng, Duration duration) {
    transform->setLatLng(latLng, duration);
    update();
}

LatLng Map::getLatLng() const {
    return transform->getLatLng();
}

void Map::resetPosition() {
    transform->setAngle(0);
    transform->setLatLng(LatLng(0, 0));
    transform->setZoom(0);
    update(Update::Zoom);
}


#pragma mark - Scale

void Map::scaleBy(double ds, double cx, double cy, Duration duration) {
    transform->scaleBy(ds, cx, cy, duration);
    update(Update::Zoom);
}

void Map::setScale(double scale, double cx, double cy, Duration duration) {
    transform->setScale(scale, cx, cy, duration);
    update(Update::Zoom);
}

double Map::getScale() const {
    return transform->getScale();
}

void Map::setZoom(double zoom, Duration duration) {
    transform->setZoom(zoom, duration);
    update(Update::Zoom);
}

double Map::getZoom() const {
    return transform->getZoom();
}

void Map::setLatLngZoom(LatLng latLng, double zoom, Duration duration) {
    transform->setLatLngZoom(latLng, zoom, duration);
    update(Update::Zoom);
}

void Map::fitBounds(LatLngBounds bounds, EdgeInsets padding, Duration duration) {
    AnnotationSegment segment = {
        {bounds.ne.latitude, bounds.sw.longitude},
        bounds.sw,
        {bounds.sw.latitude, bounds.ne.longitude},
        bounds.ne,
    };
    fitBounds(segment, padding, duration);
}

void Map::fitBounds(AnnotationSegment segment, EdgeInsets padding, Duration duration) {
    if (segment.empty()) {
        return;
    }

    // Calculate the bounds of the possibly rotated shape with respect to the viewport.
    vec2<> nePixel = {-INFINITY, -INFINITY};
    vec2<> swPixel = {INFINITY, INFINITY};
    for (LatLng latLng : segment) {
        vec2<> pixel = pixelForLatLng(latLng);
        swPixel.x = std::min(swPixel.x, pixel.x);
        nePixel.x = std::max(nePixel.x, pixel.x);
        swPixel.y = std::min(swPixel.y, pixel.y);
        nePixel.y = std::max(nePixel.y, pixel.y);
    }
    vec2<> size = nePixel - swPixel;

    // Calculate the zoom level.
    double scaleX = (getWidth() - padding.left - padding.right) / size.x;
    double scaleY = (getHeight() - padding.top - padding.bottom) / size.y;
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
    return transform->getState().getMinZoom();
}

double Map::getMaxZoom() const {
    return transform->getState().getMaxZoom();
}


#pragma mark - Size

uint16_t Map::getWidth() const {
    return transform->getState().getWidth();
}

uint16_t Map::getHeight() const {
    return transform->getState().getHeight();
}


#pragma mark - Rotation

void Map::rotateBy(double sx, double sy, double ex, double ey, Duration duration) {
    transform->rotateBy(sx, sy, ex, ey, duration);
    update();
}

void Map::setBearing(double degrees, Duration duration) {
    transform->setAngle(-degrees * M_PI / 180, duration);
    update();
}

void Map::setBearing(double degrees, double cx, double cy) {
    transform->setAngle(-degrees * M_PI / 180, cx, cy);
    update();
}

double Map::getBearing() const {
    return -transform->getAngle() / M_PI * 180;
}

void Map::resetNorth() {
    transform->setAngle(0, std::chrono::milliseconds(500));
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
    return transform->getState().pixelForLatLng(latLng);
}

const LatLng Map::latLngForPixel(const vec2<double> pixel) const {
    return transform->getState().latLngForPixel(pixel);
}

#pragma mark - Annotations

void Map::setDefaultPointAnnotationSymbol(const std::string& symbol) {
    data->getAnnotationManager()->setDefaultPointAnnotationSymbol(symbol);
}

double Map::getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol) {
    return context->invokeSync<double>(&MapContext::getTopOffsetPixelsForAnnotationSymbol, symbol);
}

uint32_t Map::addPointAnnotation(const PointAnnotation& annotation) {
    return addPointAnnotations({ annotation }).front();
}

AnnotationIDs Map::addPointAnnotations(const std::vector<PointAnnotation>& annotations) {
    auto result = data->getAnnotationManager()->addPointAnnotations(annotations, getMaxZoom());
    context->invoke(&MapContext::updateAnnotationTiles, result.first);
    return result.second;
}

uint32_t Map::addShapeAnnotation(const ShapeAnnotation& annotation) {
    return addShapeAnnotations({ annotation }).front();
}

AnnotationIDs Map::addShapeAnnotations(const std::vector<ShapeAnnotation>& annotations) {
    auto result = data->getAnnotationManager()->addShapeAnnotations(annotations, getMaxZoom());
    context->invoke(&MapContext::updateAnnotationTiles, result.first);
    return result.second;
}

void Map::removeAnnotation(uint32_t annotation) {
    removeAnnotations({ annotation });
}

void Map::removeAnnotations(const std::vector<uint32_t>& annotations) {
    auto result = data->getAnnotationManager()->removeAnnotations(annotations, getMaxZoom());
    context->invoke(&MapContext::updateAnnotationTiles, result);
}

std::vector<uint32_t> Map::getAnnotationsInBounds(const LatLngBounds& bounds, const AnnotationType& type) {
    return data->getAnnotationManager()->getAnnotationsInBounds(bounds, getMaxZoom(), type);
}

LatLngBounds Map::getBoundsForAnnotations(const std::vector<uint32_t>& annotations) {
    return data->getAnnotationManager()->getBoundsForAnnotations(annotations);
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
    return context->invokeSync<bool>(&MapContext::isLoaded);
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
