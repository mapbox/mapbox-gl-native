#include <mbgl/map/map.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/map/backend.hpp>
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
#include <mbgl/util/projection.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/platform/log.hpp>

namespace mbgl {

using namespace style;

enum class RenderState : uint8_t {
    Never,
    Partial,
    Fully,
};

struct StillImageRequest {
    StillImageRequest(View& view_, Map::StillImageCallback&& callback_)
        : view(view_), callback(std::move(callback_)) {
    }

    View& view;
    Map::StillImageCallback callback;
};

class Map::Impl : public style::Observer {
public:
    Impl(Backend&,
         float pixelRatio,
         FileSource&,
         Scheduler&,
         MapMode,
         GLContextMode,
         ConstrainMode,
         ViewportMode);

    void onSourceAttributionChanged(style::Source&, const std::string&) override;
    void onUpdate(Update) override;
    void onStyleLoaded() override;
    void onStyleError() override;
    void onResourceError(std::exception_ptr) override;

    void update();
    void render(View&);

    void loadStyleJSON(const std::string&);

    Backend& backend;
    FileSource& fileSource;
    Scheduler& scheduler;

    RenderState renderState = RenderState::Never;
    Transform transform;

    const MapMode mode;
    const GLContextMode contextMode;
    const float pixelRatio;

    MapDebugOptions debugOptions { MapDebugOptions::NoDebug };

    Update updateFlags = Update::Nothing;
    util::AsyncTask asyncUpdate;

    std::unique_ptr<AnnotationManager> annotationManager;
    std::unique_ptr<Painter> painter;
    std::unique_ptr<Style> style;

    std::string styleURL;
    std::string styleJSON;
    bool styleMutated = false;

    std::unique_ptr<AsyncRequest> styleRequest;

    std::unique_ptr<StillImageRequest> stillImageRequest;
    size_t sourceCacheSize;
    TimePoint timePoint;
    bool loading = false;
};

Map::Map(Backend& backend,
         const Size size,
         const float pixelRatio,
         FileSource& fileSource,
         Scheduler& scheduler,
         MapMode mapMode,
         GLContextMode contextMode,
         ConstrainMode constrainMode,
         ViewportMode viewportMode)
    : impl(std::make_unique<Impl>(backend,
                                  pixelRatio,
                                  fileSource,
                                  scheduler,
                                  mapMode,
                                  contextMode,
                                  constrainMode,
                                  viewportMode)) {
    impl->transform.resize(size);
}

Map::Impl::Impl(Backend& backend_,
                float pixelRatio_,
                FileSource& fileSource_,
                Scheduler& scheduler_,
                MapMode mode_,
                GLContextMode contextMode_,
                ConstrainMode constrainMode_,
                ViewportMode viewportMode_)
    : backend(backend_),
      fileSource(fileSource_),
      scheduler(scheduler_),
      transform([this](MapChange change) { backend.notifyMapChange(change); },
                constrainMode_,
                viewportMode_),
      mode(mode_),
      contextMode(contextMode_),
      pixelRatio(pixelRatio_),
      asyncUpdate([this] { update(); }),
      annotationManager(std::make_unique<AnnotationManager>(pixelRatio)) {
}

Map::~Map() {
    impl->backend.activate();

    impl->styleRequest = nullptr;

    // Explicit resets currently necessary because these abandon resources that need to be
    // cleaned up by context.reset();
    impl->style.reset();
    impl->annotationManager.reset();
    impl->painter.reset();

    impl->backend.deactivate();
}

void Map::renderStill(View& view, StillImageCallback callback) {
    if (!callback) {
        Log::Error(Event::General, "StillImageCallback not set");
        return;
    }

    if (impl->mode != MapMode::Still) {
        callback(std::make_exception_ptr(util::MisuseException("Map is not in still image render mode")));
        return;
    }

    if (impl->stillImageRequest) {
        callback(std::make_exception_ptr(util::MisuseException("Map is currently rendering an image")));
        return;
    }

    if (!impl->style) {
        callback(std::make_exception_ptr(util::MisuseException("Map doesn't have a style")));
        return;
    }

    if (impl->style->getLastError()) {
        callback(impl->style->getLastError());
        return;
    }

    impl->stillImageRequest = std::make_unique<StillImageRequest>(view, std::move(callback));
    impl->updateFlags |= Update::RenderStill;
    impl->asyncUpdate.send();
}

void Map::render(View& view) {
    if (!impl->style) {
        return;
    }

    if (impl->renderState == RenderState::Never) {
        impl->backend.notifyMapChange(MapChangeWillStartRenderingMap);
    }

    impl->backend.notifyMapChange(MapChangeWillStartRenderingFrame);

    const Update flags = impl->transform.updateTransitions(Clock::now());

    impl->render(view);

    impl->backend.notifyMapChange(isFullyLoaded() ?
        MapChangeDidFinishRenderingFrameFullyRendered :
        MapChangeDidFinishRenderingFrame);

    if (!isFullyLoaded()) {
        impl->renderState = RenderState::Partial;
    } else if (impl->renderState != RenderState::Fully) {
        impl->renderState = RenderState::Fully;
        impl->backend.notifyMapChange(MapChangeDidFinishRenderingMapFullyRendered);
        if (impl->loading) {
            impl->loading = false;
            impl->backend.notifyMapChange(MapChangeDidFinishLoadingMap);
        }
    }

    // Triggers an asynchronous update, that eventually triggers a view
    // invalidation, causing renderSync to be called again if in transition.
    if (flags != Update::Nothing) {
        impl->onUpdate(flags);
    }
}

void Map::triggerRepaint() {
    impl->backend.invalidate();
}

void Map::Impl::update() {
    if (!style) {
        updateFlags = Update::Nothing;
    }

    if (updateFlags == Update::Nothing || (mode == MapMode::Still && !stillImageRequest)) {
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

    if (updateFlags & Update::Layout) {
        style->relayout();
    }

    if (updateFlags & Update::Classes) {
        style->cascade(timePoint, mode);
    }

    if (updateFlags & Update::Classes || updateFlags & Update::RecalculateStyle) {
        style->recalculate(transform.getZoom(), timePoint, mode);
    }

    style::UpdateParameters parameters(pixelRatio,
                                       debugOptions,
                                       transform.getState(),
                                       scheduler,
                                       fileSource,
                                       mode,
                                       *annotationManager,
                                       *style);

    style->updateTiles(parameters);

    if (mode == MapMode::Continuous) {
        backend.invalidate();
    } else if (stillImageRequest && style->isLoaded()) {
        // TODO: determine whether we need activate/deactivate
        backend.activate();
        render(stillImageRequest->view);
        backend.deactivate();
    }

    updateFlags = Update::Nothing;
}

void Map::Impl::render(View& view) {
    if (!painter) {
        painter = std::make_unique<Painter>(backend.getContext(), transform.getState());
    }

    FrameData frameData { timePoint,
                          pixelRatio,
                          mode,
                          contextMode,
                          debugOptions };

    painter->render(*style,
                    frameData,
                    view,
                    annotationManager->getSpriteAtlas());

    if (mode == MapMode::Still) {
        auto request = std::move(stillImageRequest);
        request->callback(nullptr);
    }

    painter->cleanup();

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

    impl->backend.notifyMapChange(MapChangeWillStartLoadingMap);

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

    impl->backend.notifyMapChange(MapChangeWillStartLoadingMap);

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
    impl->onUpdate(Update::Repaint);
}

void Map::setGestureInProgress(bool inProgress) {
    impl->transform.setGestureInProgress(inProgress);
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
    impl->onUpdate(camera.zoom ? Update::RecalculateStyle : Update::Repaint);
}

void Map::easeTo(const CameraOptions& camera, const AnimationOptions& animation) {
    impl->transform.easeTo(camera, animation);
    impl->onUpdate(camera.zoom ? Update::RecalculateStyle : Update::Repaint);
}

void Map::flyTo(const CameraOptions& camera, const AnimationOptions& animation) {
    impl->transform.flyTo(camera, animation);
    impl->onUpdate(Update::RecalculateStyle);
}

#pragma mark - Position

void Map::moveBy(const ScreenCoordinate& point, const Duration& duration) {
    impl->transform.moveBy(point, duration);
    impl->onUpdate(Update::Repaint);
}

void Map::setLatLng(const LatLng& latLng, const Duration& duration) {
    setLatLng(latLng, optional<ScreenCoordinate> {}, duration);
}

void Map::setLatLng(const LatLng& latLng, optional<EdgeInsets> padding, const Duration& duration) {
    impl->transform.setLatLng(latLng, padding, duration);
    impl->onUpdate(Update::Repaint);
}

void Map::setLatLng(const LatLng& latLng, optional<ScreenCoordinate> anchor, const Duration& duration) {
    impl->transform.setLatLng(latLng, anchor, duration);
    impl->onUpdate(Update::Repaint);
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
    impl->onUpdate(Update::RecalculateStyle);
}


#pragma mark - Scale

void Map::scaleBy(double ds, optional<ScreenCoordinate> anchor, const Duration& duration) {
    impl->transform.scaleBy(ds, anchor, duration);
    impl->onUpdate(Update::RecalculateStyle);
}

void Map::setScale(double scale, optional<ScreenCoordinate> anchor, const Duration& duration) {
    impl->transform.setScale(scale, anchor, duration);
    impl->onUpdate(Update::RecalculateStyle);
}

double Map::getScale() const {
    return impl->transform.getScale();
}

void Map::setZoom(double zoom, const Duration& duration) {
    setZoom(zoom, {}, duration);
}

void Map::setZoom(double zoom, optional<EdgeInsets> padding, const Duration& duration) {
    impl->transform.setZoom(zoom, padding, duration);
    impl->onUpdate(Update::RecalculateStyle);
}

double Map::getZoom() const {
    return impl->transform.getZoom();
}

void Map::setLatLngZoom(const LatLng& latLng, double zoom, const Duration& duration) {
    setLatLngZoom(latLng, zoom, {}, duration);
}

void Map::setLatLngZoom(const LatLng& latLng, double zoom, optional<EdgeInsets> padding, const Duration& duration) {
    impl->transform.setLatLngZoom(latLng, zoom, padding, duration);
    impl->onUpdate(Update::RecalculateStyle);
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
    double viewportHeight = getSize().height;
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
        double scaleX = double(getSize().width) / width;
        double scaleY = double(getSize().height) / height;
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

void Map::setSize(const Size size) {
    impl->transform.resize(size);
    impl->onUpdate(Update::Repaint);
}

Size Map::getSize() const {
    return impl->transform.getState().getSize();
}

#pragma mark - Rotation

void Map::rotateBy(const ScreenCoordinate& first, const ScreenCoordinate& second, const Duration& duration) {
    impl->transform.rotateBy(first, second, duration);
    impl->onUpdate(Update::Repaint);
}

void Map::setBearing(double degrees, const Duration& duration) {
    setBearing(degrees, EdgeInsets(), duration);
}

void Map::setBearing(double degrees, optional<ScreenCoordinate> anchor, const Duration& duration) {
    impl->transform.setAngle(-degrees * util::DEG2RAD, anchor, duration);
    impl->onUpdate(Update::Repaint);
}

void Map::setBearing(double degrees, optional<EdgeInsets> padding, const Duration& duration) {
    impl->transform.setAngle(-degrees * util::DEG2RAD, padding, duration);
    impl->onUpdate(Update::Repaint);
}

double Map::getBearing() const {
    return -impl->transform.getAngle() * util::RAD2DEG;
}

void Map::resetNorth(const Duration& duration) {
    impl->transform.setAngle(0, duration);
    impl->onUpdate(Update::Repaint);
}

#pragma mark - Pitch

void Map::setPitch(double pitch, const Duration& duration) {
    setPitch(pitch, {}, duration);
}

void Map::setPitch(double pitch, optional<ScreenCoordinate> anchor, const Duration& duration) {
    impl->transform.setPitch(pitch * util::DEG2RAD, anchor, duration);
    impl->onUpdate(Update::Repaint);
}

double Map::getPitch() const {
    return impl->transform.getPitch() * util::RAD2DEG;
}

#pragma mark - North Orientation

void Map::setNorthOrientation(NorthOrientation orientation) {
    impl->transform.setNorthOrientation(orientation);
    impl->onUpdate(Update::Repaint);
}

NorthOrientation Map::getNorthOrientation() const {
    return impl->transform.getNorthOrientation();
}

#pragma mark - Constrain mode

void Map::setConstrainMode(mbgl::ConstrainMode mode) {
    impl->transform.setConstrainMode(mode);
    impl->onUpdate(Update::Repaint);
}

ConstrainMode Map::getConstrainMode() const {
    return impl->transform.getConstrainMode();
}

#pragma mark - Viewport mode

void Map::setViewportMode(mbgl::ViewportMode mode) {
    impl->transform.setViewportMode(mode);
    impl->onUpdate(Update::Repaint);
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
    impl->onUpdate(Update::AnnotationStyle | Update::AnnotationData);
    return result;
}

void Map::updateAnnotation(AnnotationID id, const Annotation& annotation) {
    impl->onUpdate(impl->annotationManager->updateAnnotation(id, annotation, getMaxZoom()));
}

void Map::removeAnnotation(AnnotationID annotation) {
    impl->annotationManager->removeAnnotation(annotation);
    impl->onUpdate(Update::AnnotationStyle | Update::AnnotationData);
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
    std::set<AnnotationID> set;
    for (auto &feature : features) {
        assert(feature.id);
        assert(*feature.id <= std::numeric_limits<AnnotationID>::max());
        set.insert(static_cast<AnnotationID>(feature.id->get<uint64_t>()));
    }
    AnnotationIDs ids;
    ids.reserve(set.size());
    std::move(set.begin(), set.end(), std::back_inserter(ids));
    return ids;
}

#pragma mark - Style API

style::Source* Map::getSource(const std::string& sourceID) {
    if (impl->style) {
        impl->styleMutated = true;
        return impl->style->getSource(sourceID);
    }
    return nullptr;
}

void Map::addSource(std::unique_ptr<style::Source> source) {
    if (impl->style) {
        impl->styleMutated = true;
        impl->style->addSource(std::move(source));
    }
}

void Map::removeSource(const std::string& sourceID) {
    if (impl->style) {
        impl->styleMutated = true;
        impl->style->removeSource(sourceID);
    }
}

style::Layer* Map::getLayer(const std::string& layerID) {
    if (impl->style) {
        impl->styleMutated = true;
        return impl->style->getLayer(layerID);
    }
    return nullptr;
}

void Map::addLayer(std::unique_ptr<Layer> layer, const optional<std::string>& before) {
    if (!impl->style) {
        return;
    }

    impl->styleMutated = true;
    impl->backend.activate();

    impl->style->addLayer(std::move(layer), before);
    impl->onUpdate(Update::Classes);

    impl->backend.deactivate();
}

void Map::removeLayer(const std::string& id) {
    if (!impl->style) {
        return;
    }

    impl->styleMutated = true;
    impl->backend.activate();

    impl->style->removeLayer(id);
    impl->onUpdate(Update::Classes);

    impl->backend.deactivate();
}

void Map::addImage(const std::string& name, std::unique_ptr<const SpriteImage> image) {
    if (!impl->style) {
        return;
    }

    impl->styleMutated = true;
    impl->style->spriteAtlas->setSprite(name, std::move(image));
    impl->style->spriteAtlas->updateDirty();

    impl->onUpdate(Update::Repaint);
}

void Map::removeImage(const std::string& name) {
    if (!impl->style) {
        return;
    }

    impl->styleMutated = true;
    impl->style->spriteAtlas->removeSprite(name);
    impl->style->spriteAtlas->updateDirty();

    impl->onUpdate(Update::Repaint);
}

#pragma mark - Defaults

std::string Map::getStyleName() const {
    if (impl->style) {
        return impl->style->getName();
    }
    return {};
}

LatLng Map::getDefaultLatLng() const {
    if (impl->style) {
        return impl->style->getDefaultLatLng();
    }
    return {};
}

double Map::getDefaultZoom() const {
    if (impl->style) {
        return impl->style->getDefaultZoom();
    }
    return {};
}

double Map::getDefaultBearing() const {
    if (impl->style) {
        return impl->style->getDefaultBearing();
    }
    return {};
}

double Map::getDefaultPitch() const {
    if (impl->style) {
        return impl->style->getDefaultPitch();
    }
    return {};
}

#pragma mark - Toggles

void Map::setDebug(MapDebugOptions debugOptions) {
    impl->debugOptions = debugOptions;
    impl->onUpdate(Update::Repaint);
}

void Map::cycleDebugOptions() {
#if not MBGL_USE_GLES2
    if (impl->debugOptions & MapDebugOptions::StencilClip)
        impl->debugOptions = MapDebugOptions::NoDebug;
    else if (impl->debugOptions & MapDebugOptions::Overdraw)
        impl->debugOptions = MapDebugOptions::StencilClip;
#else
    if (impl->debugOptions & MapDebugOptions::Overdraw)
        impl->debugOptions = MapDebugOptions::NoDebug;
#endif // MBGL_USE_GLES2
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

    impl->onUpdate(Update::Repaint);
}

MapDebugOptions Map::getDebug() const {
    return impl->debugOptions;
}

bool Map::isFullyLoaded() const {
    return impl->style ? impl->style->isLoaded() : false;
}

void Map::addClass(const std::string& className) {
    if (impl->style && impl->style->addClass(className)) {
        impl->onUpdate(Update::Classes);
    }
}

void Map::removeClass(const std::string& className) {
    if (impl->style && impl->style->removeClass(className)) {
        impl->onUpdate(Update::Classes);
    }
}

void Map::setClasses(const std::vector<std::string>& classNames) {
    if (impl->style) {
        impl->style->setClasses(classNames);
        impl->onUpdate(Update::Classes);
    }
}

style::TransitionOptions Map::getTransitionOptions() const {
    if (impl->style) {
        return impl->style->getTransitionOptions();
    }
    return {};
}

void Map::setTransitionOptions(const style::TransitionOptions& options) {
    if (impl->style) {
        impl->style->setTransitionOptions(options);
    }
}

bool Map::hasClass(const std::string& className) const {
    return impl->style ? impl->style->hasClass(className) : false;
}

std::vector<std::string> Map::getClasses() const {
    if (impl->style) {
        return impl->style->getClasses();
    }
    return {};
}

void Map::setSourceTileCacheSize(size_t size) {
    if (size != impl->sourceCacheSize) {
        impl->sourceCacheSize = size;
        if (!impl->style) return;
        impl->style->setSourceTileCacheSize(size);
        impl->backend.invalidate();
    }
}

void Map::onLowMemory() {
    if (impl->painter) {
        impl->painter->cleanup();
    }
    if (impl->style) {
        impl->style->onLowMemory();
        impl->backend.invalidate();
    }
}

void Map::Impl::onSourceAttributionChanged(style::Source&, const std::string&) {
    backend.notifyMapChange(MapChangeSourceDidChange);
}

void Map::Impl::onUpdate(Update flags) {
    updateFlags |= flags;
    asyncUpdate.send();
}
    
void Map::Impl::onStyleLoaded() {
    backend.notifyMapChange(MapChangeDidFinishLoadingStyle);
}

void Map::Impl::onStyleError() {
    backend.notifyMapChange(MapChangeDidFailLoadingMap);
}

void Map::Impl::onResourceError(std::exception_ptr error) {
    if (mode == MapMode::Still && stillImageRequest) {
        auto request = std::move(stillImageRequest);
        request->callback(error);
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
