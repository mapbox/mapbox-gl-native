#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/transition_options.hpp>

#include <cstdint>
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace mbgl {

class FileSource;
class View;
class SpriteImage;
struct CameraOptions;
struct AnimationOptions;

namespace style {
class Source;
class Layer;
} // namespace style

class Map : private util::noncopyable {
public:
    explicit Map(View&, FileSource&,
                 MapMode mapMode = MapMode::Continuous,
                 GLContextMode contextMode = GLContextMode::Unique,
                 ConstrainMode constrainMode = ConstrainMode::HeightOnly,
                 ViewportMode viewportMode = ViewportMode::Default);
    ~Map();

    // Register a callback that will get called (on the render thread) when all resources have
    // been loaded and a complete render occurs.
    using StillImageCallback = std::function<void (std::exception_ptr, PremultipliedImage&&)>;
    void renderStill(StillImageCallback callback);

    // Main render function.
    void render();

    // Notifies the Map that the state has changed and an update might be necessary.
    void update(Update update);

    // Styling
    void addClass(const std::string&, const style::TransitionOptions& = {});
    void removeClass(const std::string&, const style::TransitionOptions& = {});
    void setClasses(const std::vector<std::string>&, const style::TransitionOptions& = {});

    bool hasClass(const std::string&) const;
    std::vector<std::string> getClasses() const;

    void setStyleURL(const std::string&);
    void setStyleJSON(const std::string&);
    std::string getStyleURL() const;
    std::string getStyleJSON() const;

    // Transition
    void cancelTransitions();
    void setGestureInProgress(bool);
    bool isGestureInProgress() const;
    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;

    // Camera
    CameraOptions getCameraOptions(optional<EdgeInsets>) const;
    void jumpTo(const CameraOptions&);
    void easeTo(const CameraOptions&, const AnimationOptions&);
    void flyTo(const CameraOptions&, const AnimationOptions&);

    // Position
    void moveBy(const ScreenCoordinate&, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, optional<ScreenCoordinate>, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, optional<EdgeInsets>, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, const Duration& = Duration::zero());
    LatLng getLatLng(optional<EdgeInsets> = {}) const;
    void resetPosition(optional<EdgeInsets> = {});

    // Scale
    void scaleBy(double ds, optional<ScreenCoordinate> = {}, const Duration& = Duration::zero());
    void setScale(double scale, optional<ScreenCoordinate> = {}, const Duration& = Duration::zero());
    double getScale() const;
    void setZoom(double zoom, const Duration& = Duration::zero());
    void setZoom(double zoom, optional<EdgeInsets>, const Duration& = Duration::zero());
    double getZoom() const;
    void setLatLngZoom(const LatLng&, double zoom, const Duration& = Duration::zero());
    void setLatLngZoom(const LatLng&, double zoom, optional<EdgeInsets>, const Duration& = Duration::zero());
    CameraOptions cameraForLatLngBounds(const LatLngBounds&, optional<EdgeInsets>) const;
    CameraOptions cameraForLatLngs(const std::vector<LatLng>&, optional<EdgeInsets>) const;
    void resetZoom();
    void setMinZoom(const double minZoom);
    double getMinZoom() const;
    void setMaxZoom(const double maxZoom);
    double getMaxZoom() const;

    // Rotation
    void rotateBy(const ScreenCoordinate& first, const ScreenCoordinate& second, const Duration& = Duration::zero());
    void setBearing(double degrees, const Duration& = Duration::zero());
    void setBearing(double degrees, optional<ScreenCoordinate>, const Duration& = Duration::zero());
    void setBearing(double degrees, optional<EdgeInsets>, const Duration& = Duration::zero());
    double getBearing() const;
    void resetNorth(const Duration& = Milliseconds(500));
    void resetNorth(optional<EdgeInsets>, const Duration& = Milliseconds(500));

    // Pitch
    void setPitch(double pitch, const Duration& = Duration::zero());
    void setPitch(double pitch, optional<ScreenCoordinate>, const Duration& = Duration::zero());
    double getPitch() const;

    // North Orientation
    void setNorthOrientation(NorthOrientation);
    NorthOrientation getNorthOrientation() const;

    // Constrain mode
    void setConstrainMode(ConstrainMode);
    ConstrainMode getConstrainMode() const;

    // Viewport mode
    void setViewportMode(ViewportMode);
    ViewportMode getViewportMode() const;

    // Size
    uint16_t getWidth() const;
    uint16_t getHeight() const;

    // Projection
    double getMetersPerPixelAtLatitude(double lat, double zoom) const;
    ProjectedMeters projectedMetersForLatLng(const LatLng&) const;
    LatLng latLngForProjectedMeters(const ProjectedMeters&) const;
    ScreenCoordinate pixelForLatLng(const LatLng&) const;
    LatLng latLngForPixel(const ScreenCoordinate&) const;

    // Annotations
    void addAnnotationIcon(const std::string&, std::shared_ptr<const SpriteImage>);
    void removeAnnotationIcon(const std::string&);
    double getTopOffsetPixelsForAnnotationIcon(const std::string&);

    AnnotationID addAnnotation(const Annotation&);
    void updateAnnotation(AnnotationID, const Annotation&);
    void removeAnnotation(AnnotationID);

    // Sources
    style::Source* getSource(const std::string& sourceID);
    void addSource(std::unique_ptr<style::Source>);
    void removeSource(const std::string& sourceID);

    // Layers
    style::Layer* getLayer(const std::string& layerID);
    void addLayer(std::unique_ptr<style::Layer>, const optional<std::string>& beforeLayerID = {});
    void removeLayer(const std::string& layerID);

    // Defaults
    std::string getStyleName() const;
    LatLng getDefaultLatLng() const;
    double getDefaultZoom() const;
    double getDefaultBearing() const;
    double getDefaultPitch() const;

    // Feature queries
    std::vector<Feature> queryRenderedFeatures(const ScreenCoordinate&, const optional<std::vector<std::string>>& layerIDs = {});
    std::vector<Feature> queryRenderedFeatures(const ScreenBox&,        const optional<std::vector<std::string>>& layerIDs = {});
    AnnotationIDs queryPointAnnotations(const ScreenBox&);

    // Memory
    void setSourceTileCacheSize(size_t);
    void onLowMemory();

    // Debug
    void setDebug(MapDebugOptions);
    void cycleDebugOptions();
    MapDebugOptions getDebug() const;

    bool isFullyLoaded() const;
    void dumpDebugLogs() const;

private:
    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace mbgl
