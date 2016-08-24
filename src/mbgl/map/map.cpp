#include <mbgl/map/map.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/observer.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/style/query_parameters.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/gl/object_store.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/tile_coordinate.hpp>

namespace mbgl {

using namespace style;

enum class RenderState : uint8_t {
    Never,
    Partial,
    Fully,
};

class Map::Impl : public style::Observer {
public:
    Impl(View&, FileSource&, MapMode, GLContextMode, ConstrainMode, ViewportMode);

    void onNeedsRepaint() override;
    void onStyleError() override;
    void onResourceError(std::exception_ptr) override;

    void update();
    void render();

    void loadStyleJSON(const std::string&);

    View& view;
    FileSource& fileSource;

    RenderState renderState = RenderState::Never;
    Transform transform;

    const MapMode mode;
    const GLContextMode contextMode;
    const float pixelRatio;

    MapDebugOptions debugOptions { MapDebugOptions::NoDebug };

    gl::ObjectStore store;
    Update updateFlags = Update::Nothing;
    util::AsyncTask asyncUpdate;

    std::unique_ptr<AnnotationManager> annotationManager;
    std::unique_ptr<Painter> painter;
    std::unique_ptr<Style> style;

    std::string styleURL;
    std::string styleJSON;
    bool styleMutated = false;

    std::unique_ptr<AsyncRequest> styleRequest;

    Map::StillImageCallback callback;
    size_t sourceCacheSize;
    TimePoint timePoint;
    bool loading = false;
};

Map::Map(View& view, FileSource& fileSource, MapMode mapMode, GLContextMode contextMode, ConstrainMode constrainMode, ViewportMode viewportMode)
    : impl(std::make_unique<Impl>(view, fileSource, mapMode, contextMode, constrainMode, viewportMode)) {
    view.initialize(this);
    update(Update::Dimensions);
}

Map::Impl::Impl(View& view_,
                FileSource& fileSource_,
                MapMode mode_,
                GLContextMode contextMode_,
                ConstrainMode constrainMode_,
                ViewportMode viewportMode_)
    : view(view_),
      fileSource(fileSource_),
      transform([this](MapChange change) { view.notifyMapChange(change); },
                constrainMode_,
                viewportMode_),
      mode(mode_),
      contextMode(contextMode_),
      pixelRatio(view.getPixelRatio()),
      asyncUpdate([this] { update(); }),
      annotationManager(std::make_unique<AnnotationManager>(pixelRatio)) {
}

Map::~Map() {
    impl->view.activate();

    impl->styleRequest = nullptr;

    // Explicit resets currently necessary because these abandon resources that need to be
    // cleaned up by store.reset();
    impl->style.reset();
    impl->painter.reset();
    impl->annotationManager.reset();
    impl->store.reset();

    impl->view.deactivate();
}

void Map::renderStill(StillImageCallback callback) {
    if (!callback) {
        Log::Error(Event::General, "StillImageCallback not set");
        return;
    }

    if (impl->mode != MapMode::Still) {
        callback(std::make_exception_ptr(util::MisuseException("Map is not in still image render mode")), {});
        return;
    }

    if (impl->callback) {
        callback(std::make_exception_ptr(util::MisuseException("Map is currently rendering an image")), {});
        return;
    }

    if (!impl->style) {
        callback(std::make_exception_ptr(util::MisuseException("Map doesn't have a style")), {});
        return;
    }

    if (impl->style->getLastError()) {
        callback(impl->style->getLastError(), {});
        return;
    }

    impl->callback = callback;
    impl->updateFlags |= Update::RenderStill;
    impl->asyncUpdate.send();
}

void Map::update(Update flags) {
    if (flags & Update::Dimensions) {
        impl->transform.resize(impl->view.getSize());
    }

    impl->updateFlags |= flags;
    impl->asyncUpdate.send();
}

void Map::render() {
    if (!impl->style) {
        return;
    }

    if (impl->renderState == RenderState::Never) {
        impl->view.notifyMapChange(MapChangeWillStartRenderingMap);
    }

    impl->view.notifyMapChange(MapChangeWillStartRenderingFrame);

    const Update flags = impl->transform.updateTransitions(Clock::now());

    impl->render();

    impl->view.notifyMapChange(isFullyLoaded() ?
        MapChangeDidFinishRenderingFrameFullyRendered :
        MapChangeDidFinishRenderingFrame);

    if (!isFullyLoaded()) {
        impl->renderState = RenderState::Partial;
    } else if (impl->renderState != RenderState::Fully) {
        impl->renderState = RenderState::Fully;
        impl->view.notifyMapChange(MapChangeDidFinishRenderingMapFullyRendered);
        if (impl->loading) {
            impl->loading = false;
            impl->view.notifyMapChange(MapChangeDidFinishLoadingMap);
        }
    }

    // Triggers an asynchronous update, that eventually triggers a view
    // invalidation, causing renderSync to be called again if in transition.
    if (flags != Update::Nothing) {
        update(flags);
    }
}

void Map::Impl::update() {
    if (!style) {
        updateFlags = Update::Nothing;
    }

    if (updateFlags == Update::Nothing || (mode == MapMode::Still && !callback)) {
        return;
    }

    // This time point is used to:
    // - Calculate style property transitions;
    // - Hint style sources to notify when all its tiles are loaded;
    timePoint = Clock::now();

    if (style->loaded && updateFlags & Update::AnnotationStyle) {
        annotationManager->updateStyle(*style);
        updateFlags |= Update::Classes;
    }

    if (updateFlags & Update::AnnotationData) {
        annotationManager->updateData();
    }

    if (updateFlags & Update::Classes) {
        style->cascade(timePoint, mode);
    }

    if (updateFlags & Update::Classes || updateFlags & Update::RecalculateStyle) {
        style->recalculate(transform.getZoom(), timePoint, mode);
    }

    style::UpdateParameters parameters(pixelRatio,
                                       debugOptions,
                                       timePoint,
                                       transform.getState(),
                                       style->workers,
                                       fileSource,
                                       style->shouldReparsePartialTiles,
                                       mode,
                                       *annotationManager,
                                       *style);

    style->update(parameters);

    if (mode == MapMode::Continuous) {
        view.invalidate();
    } else if (callback && style->isLoaded()) {
        view.activate();
        render();
        view.deactivate();
    }

    updateFlags = Update::Nothing;
}

void Map::Impl::render() {
    if (!painter) {
        painter = std::make_unique<Painter>(transform.getState(), store);
    }

    FrameData frameData { view.getFramebufferSize(),
                          timePoint,
                          pixelRatio,
                          mode,
                          contextMode,
                          debugOptions };

    painter->render(*style,
                    frameData,
                    annotationManager->getSpriteAtlas());

    if (mode == MapMode::Still) {
        callback(nullptr, view.readStillImage());
        callback = nullptr;
    }

    store.performCleanup();

    if (style->hasTransitions()) {
        updateFlags |= Update::RecalculateStyle;
        asyncUpdate.send();
    } else if (painter->needsAnimation()) {
        updateFlags |= Update::Repaint;
        asyncUpdate.send();
    }
}

#pragma mark - Style

void Map::setStyleURL(const std::string& url) {
    if (impl->styleURL == url) {
        return;
    }

    impl->loading = true;

    impl->view.notifyMapChange(MapChangeWillStartLoadingMap);

    impl->styleRequest = nullptr;
    impl->styleURL = url;
    impl->styleJSON.clear();
    impl->styleMutated = false;

    impl->style = std::make_unique<Style>(impl->fileSource, impl->pixelRatio);

    impl->styleRequest = impl->fileSource.request(Resource::style(impl->styleURL), [this](Response res) {
        // Once we get a fresh style, or the style is mutated, stop revalidating.
        if (res.isFresh() || impl->styleMutated) {
            impl->styleRequest.reset();
        }

        // Don't allow a loaded, mutated style to be overwritten with a new version.
        if (impl->styleMutated && impl->style->loaded) {
            return;
        }

        if (res.error) {
            if (res.error->reason == Response::Error::Reason::NotFound &&
                util::mapbox::isMapboxURL(impl->styleURL)) {
                Log::Error(Event::Setup, "style %s could not be found or is an incompatible legacy map or style", impl->styleURL.c_str());
            } else {
                Log::Error(Event::Setup, "loading style failed: %s", res.error->message.c_str());
            }
            impl->onStyleError();
            impl->onResourceError(std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified || res.noContent) {
            return;
        } else {
            impl->loadStyleJSON(*res.data);
        }
    });
}

void Map::setStyleJSON(const std::string& json) {
    if (impl->styleJSON == json) {
        return;
    }

    impl->loading = true;

    impl->view.notifyMapChange(MapChangeWillStartLoadingMap);

    impl->styleURL.clear();
    impl->styleJSON.clear();
    impl->styleMutated = false;

    impl->style = std::make_unique<Style>(impl->fileSource, impl->pixelRatio);

    impl->loadStyleJSON(json);
}

void Map::Impl::loadStyleJSON(const std::string& json) {
    style->setObserver(this);
    style->setJSON(json);
    styleJSON = json;

    // force style cascade, causing all pending transitions to complete.
    style->cascade(Clock::now(), mode);

    updateFlags |= Update::Classes | Update::RecalculateStyle | Update::AnnotationStyle;
    asyncUpdate.send();
}

std::string Map::getStyleURL() const {
    return impl->styleURL;
}

std::string Map::getStyleJSON() const {
    return impl->styleJSON;
}

#pragma mark - Transitions

void Map::cancelTransitions() {
    impl->transform.cancelTransitions();
    update(Update::Repaint);
}

void Map::setGestureInProgress(bool inProgress) {
    impl->transform.setGestureInProgress(inProgress);
    update(Update::Repaint);
}

bool Map::isGestureInProgress() const {
    return impl->transform.isGestureInProgress();
}

bool Map::isRotating() const {
    return impl->transform.isRotating();
}

bool Map::isScaling() const {
    return impl->transform.isScaling();
}

bool Map::isPanning() const {
    return impl->transform.isPanning();
}

#pragma mark -

CameraOptions Map::getCameraOptions(optional<EdgeInsets> padding) const {
    return impl->transform.getCameraOptions(padding);
}

void Map::jumpTo(const CameraOptions& camera) {
    impl->transform.jumpTo(camera);
    update(camera.zoom ? Update::RecalculateStyle : Update::Repaint);
}

void Map::easeTo(const CameraOptions& camera, const AnimationOptions& animation) {
    impl->transform.easeTo(camera, animation);
    update(camera.zoom ? Update::RecalculateStyle : Update::Repaint);
}

void Map::flyTo(const CameraOptions& camera, const AnimationOptions& animation) {
    impl->transform.flyTo(camera, animation);
    update(Update::RecalculateStyle);
}

#pragma mark - Position

void Map::moveBy(const ScreenCoordinate& point, const Duration& duration) {
    impl->transform.moveBy(point, duration);
    update(Update::Repaint);
}

void Map::setLatLng(const LatLng& latLng, const Duration& duration) {
    setLatLng(latLng, ScreenCoordinate {}, duration);
}

void Map::setLatLng(const LatLng& latLng, optional<EdgeInsets> padding, const Duration& duration) {
    impl->transform.setLatLng(latLng, padding, duration);
    update(Update::Repaint);
}

void Map::setLatLng(const LatLng& latLng, optional<ScreenCoordinate> anchor, const Duration& duration) {
    impl->transform.setLatLng(latLng, anchor, duration);
    update(Update::Repaint);
}

LatLng Map::getLatLng(optional<EdgeInsets> padding) const {
    return impl->transform.getLatLng(padding);
}

void Map::resetPosition(optional<EdgeInsets> padding) {
    CameraOptions camera;
    camera.angle = 0;
    camera.pitch = 0;
    camera.center = LatLng(0, 0);
    camera.padding = padding;
    camera.zoom = 0;
    impl->transform.jumpTo(camera);
    update(Update::RecalculateStyle);
}


#pragma mark - Scale

void Map::scaleBy(double ds, optional<ScreenCoordinate> anchor, const Duration& duration) {
    impl->transform.scaleBy(ds, anchor, duration);
    update(Update::RecalculateStyle);
}

void Map::setScale(double scale, optional<ScreenCoordinate> anchor, const Duration& duration) {
    impl->transform.setScale(scale, anchor, duration);
    update(Update::RecalculateStyle);
}

double Map::getScale() const {
    return impl->transform.getScale();
}

void Map::setZoom(double zoom, const Duration& duration) {
    setZoom(zoom, {}, duration);
}

void Map::setZoom(double zoom, optional<EdgeInsets> padding, const Duration& duration) {
    impl->transform.setZoom(zoom, padding, duration);
    update(Update::RecalculateStyle);
}

double Map::getZoom() const {
    return impl->transform.getZoom();
}

void Map::setLatLngZoom(const LatLng& latLng, double zoom, const Duration& duration) {
    setLatLngZoom(latLng, zoom, {}, duration);
}

void Map::setLatLngZoom(const LatLng& latLng, double zoom, optional<EdgeInsets> padding, const Duration& duration) {
    impl->transform.setLatLngZoom(latLng, zoom, padding, duration);
    update(Update::RecalculateStyle);
}

CameraOptions Map::cameraForLatLngBounds(const LatLngBounds& bounds, optional<EdgeInsets> padding) const {
    return cameraForLatLngs({
        bounds.northwest(),
        bounds.southwest(),
        bounds.southeast(),
        bounds.northeast(),
    }, padding);
}

CameraOptions Map::cameraForLatLngs(const std::vector<LatLng>& latLngs, optional<EdgeInsets> padding) const {
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
    double minScale = INFINITY;
    if (width > 0 || height > 0) {
        double scaleX = getWidth() / width;
        double scaleY = getHeight() / height;
        if (padding && *padding) {
            scaleX -= (padding->left + padding->right) / width;
            scaleY -= (padding->top + padding->bottom) / height;
        }
        minScale = util::min(scaleX, scaleY);
    }
    double zoom = util::log2(getScale() * minScale);
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
        centerPixel = centerPixel + paddedNEPixel - paddedSWPixel;
    }
    centerPixel /= 2.0;

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
    impl->transform.setMinZoom(minZoom);
    if (getZoom() < minZoom) {
        setZoom(minZoom);
    }
}

double Map::getMinZoom() const {
    return impl->transform.getState().getMinZoom();
}

void Map::setMaxZoom(const double maxZoom) {
    impl->transform.setMaxZoom(maxZoom);
    if (getZoom() > maxZoom) {
        setZoom(maxZoom);
    }
}

double Map::getMaxZoom() const {
    return impl->transform.getState().getMaxZoom();
}

#pragma mark - Size

uint16_t Map::getWidth() const {
    return impl->transform.getState().getWidth();
}

uint16_t Map::getHeight() const {
    return impl->transform.getState().getHeight();
}

#pragma mark - Rotation

void Map::rotateBy(const ScreenCoordinate& first, const ScreenCoordinate& second, const Duration& duration) {
    impl->transform.rotateBy(first, second, duration);
    update(Update::Repaint);
}

void Map::setBearing(double degrees, const Duration& duration) {
    setBearing(degrees, EdgeInsets(), duration);
}

void Map::setBearing(double degrees, optional<ScreenCoordinate> anchor, const Duration& duration) {
    impl->transform.setAngle(-degrees * util::DEG2RAD, anchor, duration);
    update(Update::Repaint);
}

void Map::setBearing(double degrees, optional<EdgeInsets> padding, const Duration& duration) {
    impl->transform.setAngle(-degrees * util::DEG2RAD, padding, duration);
    update(Update::Repaint);
}

double Map::getBearing() const {
    return -impl->transform.getAngle() * util::RAD2DEG;
}

void Map::resetNorth(const Duration& duration) {
    impl->transform.setAngle(0, duration);
    update(Update::Repaint);
}

#pragma mark - Pitch

void Map::setPitch(double pitch, const Duration& duration) {
    setPitch(pitch, {}, duration);
}

void Map::setPitch(double pitch, optional<ScreenCoordinate> anchor, const Duration& duration) {
    impl->transform.setPitch(pitch * util::DEG2RAD, anchor, duration);
    update(Update::Repaint);
}

double Map::getPitch() const {
    return impl->transform.getPitch() * util::RAD2DEG;
}

#pragma mark - North Orientation

void Map::setNorthOrientation(NorthOrientation orientation) {
    impl->transform.setNorthOrientation(orientation);
    update(Update::Repaint);
}

NorthOrientation Map::getNorthOrientation() const {
    return impl->transform.getNorthOrientation();
}

#pragma mark - Constrain mode

void Map::setConstrainMode(mbgl::ConstrainMode mode) {
    impl->transform.setConstrainMode(mode);
    update(Update::Repaint);
}

ConstrainMode Map::getConstrainMode() const {
    return impl->transform.getConstrainMode();
}

#pragma mark - Viewport mode

void Map::setViewportMode(mbgl::ViewportMode mode) {
    impl->transform.setViewportMode(mode);
    update(Update::Repaint);
}

ViewportMode Map::getViewportMode() const {
    return impl->transform.getViewportMode();
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
    return impl->transform.latLngToScreenCoordinate(latLng);
}

LatLng Map::latLngForPixel(const ScreenCoordinate& pixel) const {
    return impl->transform.screenCoordinateToLatLng(pixel);
}

#pragma mark - Annotations

void Map::addAnnotationIcon(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    impl->annotationManager->addIcon(name, sprite);
}

void Map::removeAnnotationIcon(const std::string& name) {
    impl->annotationManager->removeIcon(name);
}

double Map::getTopOffsetPixelsForAnnotationIcon(const std::string& name) {
    return impl->annotationManager->getTopOffsetPixelsForIcon(name);
}

AnnotationID Map::addAnnotation(const Annotation& annotation) {
    auto result = impl->annotationManager->addAnnotation(annotation, getMaxZoom());
    update(Update::AnnotationStyle | Update::AnnotationData);
    return result;
}

void Map::updateAnnotation(AnnotationID id, const Annotation& annotation) {
    update(impl->annotationManager->updateAnnotation(id, annotation, getMaxZoom()));
}

void Map::removeAnnotation(AnnotationID annotation) {
    impl->annotationManager->removeAnnotation(annotation);
    update(Update::AnnotationStyle | Update::AnnotationData);
}

#pragma mark - Feature query api

std::vector<Feature> Map::queryRenderedFeatures(const ScreenCoordinate& point, const optional<std::vector<std::string>>& layerIDs) {
    if (!impl->style) return {};

    return impl->style->queryRenderedFeatures({
        { point },
        impl->transform.getState(),
        layerIDs
    });
}

std::vector<Feature> Map::queryRenderedFeatures(const ScreenBox& box, const optional<std::vector<std::string>>& layerIDs) {
    if (!impl->style) return {};

    return impl->style->queryRenderedFeatures({
        {
            box.min,
            { box.max.x, box.min.y },
            box.max,
            { box.min.x, box.max.y },
            box.min
        },
        impl->transform.getState(),
        layerIDs
    });
}

AnnotationIDs Map::queryPointAnnotations(const ScreenBox& box) {
    auto features = queryRenderedFeatures(box, {{ AnnotationManager::PointLayerID }});
    AnnotationIDs ids;
    ids.reserve(features.size());
    for (auto &feature : features) {
        assert(feature.id);
        assert(*feature.id <= std::numeric_limits<AnnotationID>::max());
        ids.push_back(static_cast<AnnotationID>(feature.id->get<uint64_t>()));
    }
    return ids;
}

#pragma mark - Style API

style::Source* Map::getSource(const std::string& sourceID) {
    impl->styleMutated = true;
    return impl->style ? impl->style->getSource(sourceID) : nullptr;
}

void Map::addSource(std::unique_ptr<style::Source> source) {
    impl->styleMutated = true;
    impl->style->addSource(std::move(source));
}

void Map::removeSource(const std::string& sourceID) {
    impl->styleMutated = true;
    impl->style->removeSource(sourceID);
}

style::Layer* Map::getLayer(const std::string& layerID) {
    impl->styleMutated = true;
    return impl->style ? impl->style->getLayer(layerID) : nullptr;
}

void Map::addLayer(std::unique_ptr<Layer> layer, const optional<std::string>& before) {
    impl->styleMutated = true;
    impl->view.activate();

    impl->style->addLayer(std::move(layer), before);
    impl->updateFlags |= Update::Classes;
    impl->asyncUpdate.send();

    impl->view.deactivate();
}

void Map::removeLayer(const std::string& id) {
    impl->styleMutated = true;
    impl->view.activate();

    impl->style->removeLayer(id);
    impl->updateFlags |= Update::Classes;
    impl->asyncUpdate.send();

    impl->view.deactivate();
}

#pragma mark - Defaults

std::string Map::getStyleName() const {
    return impl->style->getName();
}

LatLng Map::getDefaultLatLng() const {
    return impl->style->getDefaultLatLng();
}

double Map::getDefaultZoom() const {
    return impl->style->getDefaultZoom();
}

double Map::getDefaultBearing() const {
    return impl->style->getDefaultBearing();
}

double Map::getDefaultPitch() const {
    return impl->style->getDefaultPitch();
}

#pragma mark - Toggles

void Map::setDebug(MapDebugOptions debugOptions) {
    impl->debugOptions = debugOptions;
    update(Update::Repaint);
}

void Map::cycleDebugOptions() {
#ifndef GL_ES_VERSION_2_0
    if (impl->debugOptions & MapDebugOptions::StencilClip)
        impl->debugOptions = MapDebugOptions::NoDebug;
    else if (impl->debugOptions & MapDebugOptions::Overdraw)
        impl->debugOptions = MapDebugOptions::StencilClip;
#else
    if (impl->debugOptions & MapDebugOptions::Overdraw)
        impl->debugOptions = MapDebugOptions::NoDebug;
#endif // GL_ES_VERSION_2_0
    else if (impl->debugOptions & MapDebugOptions::Collision)
        impl->debugOptions = MapDebugOptions::Overdraw;
    else if (impl->debugOptions & MapDebugOptions::Timestamps)
        impl->debugOptions = impl->debugOptions | MapDebugOptions::Collision;
    else if (impl->debugOptions & MapDebugOptions::ParseStatus)
        impl->debugOptions = impl->debugOptions | MapDebugOptions::Timestamps;
    else if (impl->debugOptions & MapDebugOptions::TileBorders)
        impl->debugOptions = impl->debugOptions | MapDebugOptions::ParseStatus;
    else
        impl->debugOptions = MapDebugOptions::TileBorders;

    update(Update::Repaint);
}

MapDebugOptions Map::getDebug() const {
    return impl->debugOptions;
}

bool Map::isFullyLoaded() const {
    return impl->style->isLoaded();
}

void Map::addClass(const std::string& className, const TransitionOptions& properties) {
    if (impl->style->addClass(className, properties)) {
        update(Update::Classes);
    }
}

void Map::removeClass(const std::string& className, const TransitionOptions& properties) {
    if (impl->style->removeClass(className, properties)) {
        update(Update::Classes);
    }
}

void Map::setClasses(const std::vector<std::string>& classNames, const TransitionOptions& properties) {
    impl->style->setClasses(classNames, properties);
    update(Update::Classes);
}

bool Map::hasClass(const std::string& className) const {
    return impl->style->hasClass(className);
}

std::vector<std::string> Map::getClasses() const {
    return impl->style->getClasses();
}

void Map::setSourceTileCacheSize(size_t size) {
    if (size != impl->sourceCacheSize) {
        impl->sourceCacheSize = size;
        if (!impl->style) return;
        impl->style->setSourceTileCacheSize(size);
        impl->view.invalidate();
    }
}

void Map::onLowMemory() {
    impl->store.performCleanup();
    if (!impl->style) return;
    impl->style->onLowMemory();
    impl->view.invalidate();
}

void Map::Impl::onNeedsRepaint() {
    updateFlags |= Update::Repaint;
    asyncUpdate.send();
}

void Map::Impl::onStyleError() {
    view.notifyMapChange(MapChangeDidFailLoadingMap);
}

void Map::Impl::onResourceError(std::exception_ptr error) {
    if (mode == MapMode::Still && callback) {
        callback(error, {});
        callback = nullptr;
    }
}

void Map::dumpDebugLogs() const {
    Log::Info(Event::General, "--------------------------------------------------------------------------------");
    Log::Info(Event::General, "MapContext::styleURL: %s", impl->styleURL.c_str());
    if (impl->style) {
        impl->style->dumpDebugLogs();
    } else {
        Log::Info(Event::General, "no style loaded");
    }
    Log::Info(Event::General, "--------------------------------------------------------------------------------");
}

} // namespace mbgl
