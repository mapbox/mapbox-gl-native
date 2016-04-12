#include <mbgl/map/map.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/property_transition.hpp>
#include <mbgl/layer/custom_layer.hpp>

#include <mbgl/util/projection.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

Map::Map(View& view_, FileSource& fileSource, MapMode mapMode, GLContextMode contextMode, ConstrainMode constrainMode)
    : view(view_),
      transform(std::make_unique<Transform>(view, constrainMode)),
      context(std::make_unique<util::Thread<MapContext>>(
        util::ThreadContext{"Map", util::ThreadType::Map, util::ThreadPriority::Regular},
        view, fileSource, mapMode, contextMode, view.getPixelRatio())),
      data(&context->invokeSync<MapData&>(&MapContext::getData))
{
    view.initialize(this);
    update(Update::Dimensions);
}

Map::~Map() {
    resume();
    context->invokeSync(&MapContext::cleanup);
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
                    FrameData { view.getFramebufferSize(), Clock::now() }, callback);
}

void Map::renderSync() {
    if (renderState == RenderState::never) {
        view.notifyMapChange(MapChangeWillStartRenderingMap);
    }

    view.notifyMapChange(MapChangeWillStartRenderingFrame);

    const Update flags = transform->updateTransitions(Clock::now());
    const bool fullyLoaded = context->invokeSync<bool>(
            &MapContext::renderSync, transform->getState(), FrameData { view.getFramebufferSize(), Clock::now() });

    view.notifyMapChange(fullyLoaded ?
        MapChangeDidFinishRenderingFrameFullyRendered :
        MapChangeDidFinishRenderingFrame);

    if (!fullyLoaded) {
        renderState = RenderState::partial;
    } else if (renderState != RenderState::fully) {
        renderState = RenderState::fully;
        view.notifyMapChange(MapChangeDidFinishRenderingMapFullyRendered);
        if (loading) {
            loading = false;
            view.notifyMapChange(MapChangeDidFinishLoadingMap);
        }
    }

    // Triggers an asynchronous update, that eventually triggers a view
    // invalidation, causing renderSync to be called again if in transition.
    if (flags != Update::Nothing) {
        update(flags);
    }
}

void Map::update(Update flags) {
    if (flags & Update::Dimensions) transform->resize(view.getSize());
    context->invoke(&MapContext::triggerUpdate, transform->getState(), flags);
}

#pragma mark - Style

void Map::setStyleURL(const std::string &url) {
    loading = true;
    view.notifyMapChange(MapChangeWillStartLoadingMap);
    context->invoke(&MapContext::setStyleURL, url);
}

void Map::setStyleJSON(const std::string& json, const std::string& base) {
    loading = true;
    view.notifyMapChange(MapChangeWillStartLoadingMap);
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

bool Map::isGestureInProgress() const {
    return transform->isGestureInProgress();
}

bool Map::isRotating() const {
    return transform->isRotating();
}

bool Map::isScaling() const {
    return transform->isScaling();
}

bool Map::isPanning() const {
    return transform->isPanning();
}

#pragma mark -

void Map::jumpTo(const CameraOptions& camera) {
    transform->jumpTo(camera);
    update(camera.zoom ? Update::RecalculateStyle : Update::Repaint);
}

void Map::easeTo(const CameraOptions& camera, const AnimationOptions& animation) {
    transform->easeTo(camera, animation);
    update(camera.zoom ? Update::RecalculateStyle : Update::Repaint);
}
    
    
void Map::flyTo(const CameraOptions& camera, const AnimationOptions& animation) {
    transform->flyTo(camera, animation);
    update(Update::RecalculateStyle);
}

#pragma mark - Position

void Map::moveBy(const ScreenCoordinate& point, const Duration& duration) {
    transform->moveBy(point, duration);
    update(Update::Repaint);
}

void Map::setLatLng(const LatLng& latLng, const Duration& duration) {
    setLatLng(latLng, ScreenCoordinate {}, duration);
}

void Map::setLatLng(const LatLng& latLng, optional<EdgeInsets> padding, const Duration& duration) {
    transform->setLatLng(latLng, padding, duration);
    update(Update::Repaint);
}

void Map::setLatLng(const LatLng& latLng, optional<ScreenCoordinate> anchor, const Duration& duration) {
    transform->setLatLng(latLng, anchor, duration);
    update(Update::Repaint);
}

LatLng Map::getLatLng(optional<EdgeInsets> padding) const {
    return transform->getLatLng(padding);
}

void Map::resetPosition(optional<EdgeInsets> padding) {
    CameraOptions camera;
    camera.angle = 0;
    camera.pitch = 0;
    camera.center = LatLng(0, 0);
    camera.padding = padding;
    camera.zoom = 0;
    transform->jumpTo(camera);
    update(Update::RecalculateStyle);
}


#pragma mark - Scale

void Map::scaleBy(double ds, optional<ScreenCoordinate> anchor, const Duration& duration) {
    transform->scaleBy(ds, anchor, duration);
    update(Update::RecalculateStyle);
}

void Map::setScale(double scale, optional<ScreenCoordinate> anchor, const Duration& duration) {
    transform->setScale(scale, anchor, duration);
    update(Update::RecalculateStyle);
}

double Map::getScale() const {
    return transform->getScale();
}

void Map::setZoom(double zoom, const Duration& duration) {
    setZoom(zoom, {}, duration);
}

void Map::setZoom(double zoom, optional<EdgeInsets> padding, const Duration& duration) {
    transform->setZoom(zoom, padding, duration);
    update(Update::RecalculateStyle);
}

double Map::getZoom() const {
    return transform->getZoom();
}

void Map::setLatLngZoom(const LatLng& latLng, double zoom, const Duration& duration) {
    setLatLngZoom(latLng, zoom, {}, duration);
}

void Map::setLatLngZoom(const LatLng& latLng, double zoom, optional<EdgeInsets> padding, const Duration& duration) {
    transform->setLatLngZoom(latLng, zoom, padding, duration);
    update(Update::RecalculateStyle);
}

CameraOptions Map::cameraForLatLngBounds(const LatLngBounds& bounds, optional<EdgeInsets> padding) {
    AnnotationSegment segment = {
        bounds.northwest(),
        bounds.southwest(),
        bounds.southeast(),
        bounds.northeast(),
    };
    return cameraForLatLngs(segment, padding);
}

CameraOptions Map::cameraForLatLngs(const std::vector<LatLng>& latLngs, optional<EdgeInsets> padding) {
    CameraOptions options;
    if (latLngs.empty()) {
        return options;
    }

    // Calculate the bounds of the possibly rotated shape with respect to the viewport.
    ScreenCoordinate nePixel = {-INFINITY, -INFINITY};
    ScreenCoordinate swPixel = {INFINITY, INFINITY};
    double viewportHeight = getHeight();
    for (LatLng latLng : latLngs) {
        ScreenCoordinate pixel = pixelForLatLng(latLng);
        swPixel.x = std::min(swPixel.x, pixel.x);
        nePixel.x = std::max(nePixel.x, pixel.x);
        swPixel.y = std::min(swPixel.y, viewportHeight - pixel.y);
        nePixel.y = std::max(nePixel.y, viewportHeight - pixel.y);
    }
    double width = nePixel.x - swPixel.x;
    double height = nePixel.y - swPixel.y;

    // Calculate the zoom level.
    double scaleX = getWidth() / width;
    double scaleY = getHeight() / height;
    if (padding && *padding) {
        scaleX -= (padding->left + padding->right) / width;
        scaleY -= (padding->top + padding->bottom) / height;
    }
    double minScale = ::fmin(scaleX, scaleY);
    double zoom = ::log2(getScale() * minScale);
    zoom = util::clamp(zoom, getMinZoom(), getMaxZoom());

    // Calculate the center point of a virtual bounds that is extended in all directions by padding.
    ScreenCoordinate centerPixel = nePixel + swPixel;
    if (padding && *padding) {
        ScreenCoordinate paddedNEPixel = {
            padding->right / minScale,
            padding->top / minScale,
        };
        ScreenCoordinate paddedSWPixel = {
            padding->left / minScale,
            padding->bottom / minScale,
        };
        centerPixel = centerPixel - paddedNEPixel - paddedSWPixel;
    }
    centerPixel /= 2;

    // CameraOptions origin is at the top-left corner.
    centerPixel.y = viewportHeight - centerPixel.y;

    options.center = latLngForPixel(centerPixel);
    options.zoom = zoom;
    return options;
}

void Map::resetZoom() {
    setZoom(0);
}

void Map::setMinZoom(const double minZoom) {
    transform->setMinZoom(minZoom);
    if (getZoom() < minZoom) {
        setZoom(minZoom);
    }
}

double Map::getMinZoom() const {
    return transform->getState().getMinZoom();
}

void Map::setMaxZoom(const double maxZoom) {
    transform->setMaxZoom(maxZoom);
    if (getZoom() > maxZoom) {
        setZoom(maxZoom);
    }
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

void Map::rotateBy(const ScreenCoordinate& first, const ScreenCoordinate& second, const Duration& duration) {
    transform->rotateBy(first, second, duration);
    update(Update::Repaint);
}

void Map::setBearing(double degrees, const Duration& duration) {
    setBearing(degrees, EdgeInsets(), duration);
}

void Map::setBearing(double degrees, optional<ScreenCoordinate> anchor, const Duration& duration) {
    transform->setAngle(-degrees * util::DEG2RAD, anchor, duration);
    update(Update::Repaint);
}

void Map::setBearing(double degrees, optional<EdgeInsets> padding, const Duration& duration) {
    transform->setAngle(-degrees * util::DEG2RAD, padding, duration);
    update(Update::Repaint);
}

double Map::getBearing() const {
    return -transform->getAngle() * util::RAD2DEG;
}

void Map::resetNorth(const Duration& duration) {
    transform->setAngle(0, duration);
    update(Update::Repaint);
}


#pragma mark - Pitch

void Map::setPitch(double pitch, const Duration& duration) {
    setPitch(pitch, {}, duration);
}

void Map::setPitch(double pitch, optional<ScreenCoordinate> anchor, const Duration& duration) {
    transform->setPitch(pitch * util::DEG2RAD, anchor, duration);
    update(Update::Repaint);
}

double Map::getPitch() const {
    return transform->getPitch() * util::RAD2DEG;
}


#pragma mark - North Orientation

void Map::setNorthOrientation(NorthOrientation orientation) {
    transform->setNorthOrientation(orientation);
    update(Update::Repaint);
}

NorthOrientation Map::getNorthOrientation() const {
    return transform->getNorthOrientation();
}

#pragma mark - Constrain mode

void Map::setConstrainMode(mbgl::ConstrainMode mode) {
    transform->setConstrainMode(mode);
    update(Update::Repaint);
}

ConstrainMode Map::getConstrainMode() const {
    return transform->getConstrainMode();
}

#pragma mark - Projection

double Map::getMetersPerPixelAtLatitude(double lat, double zoom) const {
    return Projection::getMetersPerPixelAtLatitude(lat, zoom);
}

ProjectedMeters Map::projectedMetersForLatLng(const LatLng& latLng) const {
    return Projection::projectedMetersForLatLng(latLng);
}

LatLng Map::latLngForProjectedMeters(const ProjectedMeters& projectedMeters) const {
    return Projection::latLngForProjectedMeters(projectedMeters);
}

ScreenCoordinate Map::pixelForLatLng(const LatLng& latLng) const {
    return transform->latLngToScreenCoordinate(latLng);
}

LatLng Map::latLngForPixel(const ScreenCoordinate& pixel) const {
    return transform->screenCoordinateToLatLng(pixel);
}

#pragma mark - Annotations

void Map::addAnnotationIcon(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    context->invoke(&MapContext::addAnnotationIcon, name, sprite);
}

void Map::removeAnnotationIcon(const std::string& name) {
    context->invoke(&MapContext::removeAnnotationIcon, name);
}

double Map::getTopOffsetPixelsForAnnotationIcon(const std::string& symbol) {
    return context->invokeSync<double>(&MapContext::getTopOffsetPixelsForAnnotationIcon, symbol);
}

AnnotationID Map::addPointAnnotation(const PointAnnotation& annotation) {
    return addPointAnnotations({ annotation }).front();
}

AnnotationIDs Map::addPointAnnotations(const std::vector<PointAnnotation>& annotations) {
    auto result = data->getAnnotationManager()->addPointAnnotations(annotations, getMaxZoom());
    update(Update::Annotations);
    return result;
}

AnnotationID Map::addShapeAnnotation(const ShapeAnnotation& annotation) {
    return addShapeAnnotations({ annotation }).front();
}

AnnotationIDs Map::addShapeAnnotations(const std::vector<ShapeAnnotation>& annotations) {
    auto result = data->getAnnotationManager()->addShapeAnnotations(annotations, getMaxZoom());
    update(Update::Annotations);
    return result;
}

void Map::updatePointAnnotation(AnnotationID annotationId, const PointAnnotation& annotation) {
    data->getAnnotationManager()->updatePointAnnotation(annotationId, annotation, getMaxZoom());
    update(Update::Annotations);
}

void Map::removeAnnotation(AnnotationID annotation) {
    removeAnnotations({ annotation });
}

void Map::removeAnnotations(const AnnotationIDs& annotations) {
    data->getAnnotationManager()->removeAnnotations(annotations);
    update(Update::Annotations);
}

AnnotationIDs Map::getPointAnnotationsInBounds(const LatLngBounds& bounds) {
    return data->getAnnotationManager()->getPointAnnotationsInBounds(bounds);
}

#pragma mark - Style API

void Map::addCustomLayer(const std::string& id,
                         CustomLayerInitializeFunction initialize,
                         CustomLayerRenderFunction render,
                         CustomLayerDeinitializeFunction deinitialize,
                         void* context_,
                         const char* before) {
    context->invoke(&MapContext::addLayer,
        std::make_unique<CustomLayer>(id, initialize, render, deinitialize, context_),
        before ? std::string(before) : optional<std::string>());
}

void Map::removeCustomLayer(const std::string& id) {
    context->invoke(&MapContext::removeLayer, id);
}

#pragma mark - Toggles

void Map::setDebug(MapDebugOptions mode) {
    data->setDebug(mode);
    update(Update::Repaint);
}

void Map::cycleDebugOptions() {
    data->cycleDebugOptions();
    update(Update::Repaint);
}

MapDebugOptions Map::getDebug() const {
    return data->getDebug();
}

bool Map::isFullyLoaded() const {
    return context->invokeSync<bool>(&MapContext::isLoaded);
}

void Map::addClass(const std::string& className, const PropertyTransition& properties) {
    context->invoke(&MapContext::addClass, className, properties);
}

void Map::removeClass(const std::string& className, const PropertyTransition& properties) {
    context->invoke(&MapContext::removeClass, className, properties);
}

void Map::setClasses(const std::vector<std::string>& classNames, const PropertyTransition& properties) {
    context->invoke(&MapContext::setClasses, classNames, properties);
}

bool Map::hasClass(const std::string& className) const {
    return context->invokeSync<bool>(&MapContext::hasClass, className);
}

std::vector<std::string> Map::getClasses() const {
    return context->invokeSync<std::vector<std::string>>(&MapContext::getClasses);
}

void Map::setSourceTileCacheSize(size_t size) {
    context->invoke(&MapContext::setSourceTileCacheSize, size);
}

void Map::onLowMemory() {
    context->invoke(&MapContext::onLowMemory);
}

void Map::dumpDebugLogs() const {
    context->invokeSync(&MapContext::dumpDebugLogs);
}

} // namespace mbgl
