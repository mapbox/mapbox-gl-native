#include <mbgl/map/map.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>

#include <mbgl/util/projection.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/math.hpp>

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

    std::unique_lock<std::mutex> lockPause(data->mutexPause);
    if (!data->paused) {
        context->invoke(&MapContext::pause);
        data->condPause.wait(lockPause, [&]{ return data->paused; });
    }
}

bool Map::isPaused() {
    return data->paused;
}

void Map::resume() {
    std::unique_lock<std::mutex> lockPause(data->mutexPause);
    data->paused = false;
    data->condPause.notify_all();
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

    const Update flags = transform->updateTransitions(Clock::now());
    const bool fullyLoaded = context->invokeSync<bool>(
            &MapContext::renderSync, transform->getState(), FrameData { view.getFramebufferSize() });

    view.notifyMapChange(fullyLoaded ?
        MapChangeDidFinishRenderingFrameFullyRendered :
        MapChangeDidFinishRenderingFrame);

    if (!fullyLoaded) {
        renderState = RenderState::partial;
    } else if (renderState != RenderState::fully) {
        renderState = RenderState::fully;
        view.notifyMapChange(MapChangeDidFinishRenderingMapFullyRendered);
    }

    // Triggers an asynchronous update, that eventually triggers a view
    // invalidation, causing renderSync to be called again if in transition.
    if (transform->inTransition()) {
        update(flags);
    }
}

void Map::update(Update flags) {
    if (flags & Update::Dimensions) {
        transform->resize(view.getSize());
    }
    context->invoke(&MapContext::triggerUpdate, transform->getState(), flags);
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
    update(Update::Repaint);
}

void Map::setGestureInProgress(bool inProgress) {
    transform->setGestureInProgress(inProgress);
    update(Update::Repaint);
}

#pragma mark -

void Map::jumpTo(CameraOptions options) {
    transform->jumpTo(options);
    update(Update::Repaint);
}

void Map::easeTo(CameraOptions options) {
    transform->easeTo(options);
    update(options.zoom ? Update::Zoom : Update::Repaint);
}

#pragma mark - Position

void Map::moveBy(double dx, double dy, const Duration& duration) {
    transform->moveBy(dx, dy, duration);
    update(Update::Repaint);
}

void Map::setLatLng(LatLng latLng, const Duration& duration) {
    transform->setLatLng(latLng, duration);
    update(Update::Repaint);
}

LatLng Map::getLatLng() const {
    return transform->getLatLng();
}

void Map::resetPosition() {
    CameraOptions options;
    options.angle = 0;
    options.center = LatLng(0, 0);
    options.zoom = 0;
    transform->jumpTo(options);
    update(Update::Zoom);
}


#pragma mark - Scale

void Map::scaleBy(double ds, double cx, double cy, const Duration& duration) {
    transform->scaleBy(ds, cx, cy, duration);
    update(Update::Zoom);
}

void Map::setScale(double scale, double cx, double cy, const Duration& duration) {
    transform->setScale(scale, cx, cy, duration);
    update(Update::Zoom);
}

double Map::getScale() const {
    return transform->getScale();
}

void Map::setZoom(double zoom, const Duration& duration) {
    transform->setZoom(zoom, duration);
    update(Update::Zoom);
}

double Map::getZoom() const {
    return transform->getZoom();
}

void Map::setLatLngZoom(LatLng latLng, double zoom, const Duration& duration) {
    transform->setLatLngZoom(latLng, zoom, duration);
    update(Update::Zoom);
}

CameraOptions Map::cameraForLatLngBounds(LatLngBounds bounds, EdgeInsets padding) {
    AnnotationSegment segment = {
        {bounds.ne.latitude, bounds.sw.longitude},
        bounds.sw,
        {bounds.sw.latitude, bounds.ne.longitude},
        bounds.ne,
    };
    return cameraForLatLngs(segment, padding);
}

CameraOptions Map::cameraForLatLngs(std::vector<LatLng> latLngs, EdgeInsets padding) {
    CameraOptions options;
    if (latLngs.empty()) {
        return options;
    }

    // Calculate the bounds of the possibly rotated shape with respect to the viewport.
    vec2<> nePixel = {-INFINITY, -INFINITY};
    vec2<> swPixel = {INFINITY, INFINITY};
    for (LatLng latLng : latLngs) {
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
    double minScale = ::fmin(scaleX, scaleY);
    double zoom = ::log2(getScale() * minScale);
    zoom = ::fmax(::fmin(zoom, getMaxZoom()), getMinZoom());

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

    options.center = centerLatLng;
    options.zoom = zoom;
    return options;
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

void Map::rotateBy(double sx, double sy, double ex, double ey, const Duration& duration) {
    transform->rotateBy(sx, sy, ex, ey, duration);
    update(Update::Repaint);
}

void Map::setBearing(double degrees, const Duration& duration) {
    transform->setAngle(-degrees * M_PI / 180, duration);
    update(Update::Repaint);
}

void Map::setBearing(double degrees, double cx, double cy) {
    transform->setAngle(-degrees * M_PI / 180, cx, cy);
    update(Update::Repaint);
}

double Map::getBearing() const {
    return -transform->getAngle() / M_PI * 180;
}

void Map::resetNorth() {
    transform->setAngle(0, std::chrono::milliseconds(500));
    update(Update::Repaint);
}


#pragma mark - Pitch

void Map::setPitch(double pitch, const Duration& duration) {
    transform->setPitch(util::clamp(pitch, 0., 60.) * M_PI / 180, duration);
    update(Update::Repaint);
}

double Map::getPitch() const {
    return transform->getPitch() / M_PI * 180;
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
    return transform->getState().latLngToPoint(latLng);
}

const LatLng Map::latLngForPixel(const vec2<double> pixel) const {
    return transform->getState().pointToLatLng(pixel);
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
    update(Update::Annotations);
    return result;
}

uint32_t Map::addShapeAnnotation(const ShapeAnnotation& annotation) {
    return addShapeAnnotations({ annotation }).front();
}

AnnotationIDs Map::addShapeAnnotations(const std::vector<ShapeAnnotation>& annotations) {
    auto result = data->getAnnotationManager()->addShapeAnnotations(annotations, getMaxZoom());
    update(Update::Annotations);
    return result;
}

void Map::removeAnnotation(uint32_t annotation) {
    removeAnnotations({ annotation });
}

void Map::removeAnnotations(const std::vector<uint32_t>& annotations) {
    data->getAnnotationManager()->removeAnnotations(annotations, getMaxZoom());
    update(Update::Annotations);
}

std::vector<uint32_t> Map::getAnnotationsInBounds(const LatLngBounds& bounds, const AnnotationType& type) {
    return data->getAnnotationManager()->getAnnotationsInBounds(bounds, getMaxZoom(), type);
}

LatLngBounds Map::getBoundsForAnnotations(const std::vector<uint32_t>& annotations) {
    return data->getAnnotationManager()->getBoundsForAnnotations(annotations);
}


#pragma mark - Sprites

void Map::setSprite(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    context->invoke(&MapContext::setSprite, name, sprite);
}

void Map::removeSprite(const std::string& name) {
    setSprite(name, nullptr);
}


#pragma mark - Toggles

void Map::setDebug(bool value) {
    data->setDebug(value);
    update(Update::Repaint);
}

void Map::toggleDebug() {
    data->toggleDebug();
    update(Update::Repaint);
}

bool Map::getDebug() const {
    return data->getDebug();
}

void Map::setCollisionDebug(bool value) {
    data->setCollisionDebug(value);
    update(Update::Repaint);
}

void Map::toggleCollisionDebug() {
    data->toggleCollisionDebug();
    update(Update::Repaint);
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

void Map::setDefaultTransitionDuration(const Duration& duration) {
    data->setDefaultTransitionDuration(duration);
    update(Update::DefaultTransition);
}

Duration Map::getDefaultTransitionDuration() const {
    return data->getDefaultTransitionDuration();
}

void Map::setDefaultTransitionDelay(const Duration& delay) {
    data->setDefaultTransitionDelay(delay);
    update(Update::DefaultTransition);
}

Duration Map::getDefaultTransitionDelay() const {
    return data->getDefaultTransitionDelay();
}

void Map::setSourceTileCacheSize(size_t size) {
    context->invoke(&MapContext::setSourceTileCacheSize, size);
}

void Map::onLowMemory() {
    context->invoke(&MapContext::onLowMemory);
}

}
