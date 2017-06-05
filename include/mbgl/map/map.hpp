#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/query.hpp>

#include <cstdint>
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace mbgl {

class Backend;
class View;
class FileSource;
class Scheduler;

namespace style {
class Image;
class Source;
class Layer;
class Light;
} // namespace style

class Map : private util::noncopyable {
public:
    explicit Map(Backend&,
                 Size size,
                 float pixelRatio,
                 FileSource&,
                 Scheduler&,
                 MapMode mapMode = MapMode::Continuous,
                 GLContextMode contextMode = GLContextMode::Unique,
                 ConstrainMode constrainMode = ConstrainMode::HeightOnly,
                 ViewportMode viewportMode = ViewportMode::Default,
                 const optional<std::string>& programCacheDir = {});
    ~Map();

    // Register a callback that will get called (on the render thread) when all resources have
    // been loaded and a complete render occurs.
    using StillImageCallback = std::function<void (std::exception_ptr)>;
    void renderStill(View&, StillImageCallback callback);

    // Triggers a repaint.
    void triggerRepaint();

    // Main render function.
    void render(View&);

    // Styling
    void addClass(const std::string&);
    void removeClass(const std::string&);
    void setClasses(const std::vector<std::string>&);

    style::TransitionOptions getTransitionOptions() const;
    void setTransitionOptions(const style::TransitionOptions&);

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
    CameraOptions getCameraOptions(const EdgeInsets&) const;
    void jumpTo(const CameraOptions&);
    void easeTo(const CameraOptions&, const AnimationOptions&);
    void flyTo(const CameraOptions&, const AnimationOptions&);

    // Position
    void moveBy(const ScreenCoordinate&, const AnimationOptions& = {});
    void setLatLng(const LatLng&, optional<ScreenCoordinate>, const AnimationOptions& = {});
    void setLatLng(const LatLng&, const EdgeInsets&, const AnimationOptions& = {});
    void setLatLng(const LatLng&, const AnimationOptions& = {});
    LatLng getLatLng(const EdgeInsets& = {}) const;
    void resetPosition(const EdgeInsets& = {});

    // Zoom
    void setZoom(double zoom, const AnimationOptions& = {});
    void setZoom(double zoom, optional<ScreenCoordinate>, const AnimationOptions& = {});
    void setZoom(double zoom, const EdgeInsets&, const AnimationOptions& = {});
    double getZoom() const;
    void setLatLngZoom(const LatLng&, double zoom, const AnimationOptions& = {});
    void setLatLngZoom(const LatLng&, double zoom, const EdgeInsets&, const AnimationOptions& = {});
    CameraOptions cameraForLatLngBounds(const LatLngBounds&, const EdgeInsets&) const;
    CameraOptions cameraForLatLngs(const std::vector<LatLng>&, const EdgeInsets&) const;
    LatLngBounds latLngBoundsForCamera(const CameraOptions&) const;
    void resetZoom();

    // Bounds
    void setLatLngBounds(optional<LatLngBounds>);
    optional<LatLngBounds> getLatLngBounds() const;
    void setMinZoom(double);
    double getMinZoom() const;
    void setMaxZoom(double);
    double getMaxZoom() const;
    void setMinPitch(double);
    double getMinPitch() const;
    void setMaxPitch(double);
    double getMaxPitch() const;

    // Rotation
    void rotateBy(const ScreenCoordinate& first, const ScreenCoordinate& second, const AnimationOptions& = {});
    void setBearing(double degrees, const AnimationOptions& = {});
    void setBearing(double degrees, optional<ScreenCoordinate>, const AnimationOptions& = {});
    void setBearing(double degrees, const EdgeInsets&, const AnimationOptions& = {});
    double getBearing() const;
    void resetNorth(const AnimationOptions& = {{mbgl::Milliseconds(500)}});
    void resetNorth(const EdgeInsets&, const AnimationOptions& = {{mbgl::Milliseconds(500)}});

    // Pitch
    void setPitch(double pitch, const AnimationOptions& = {});
    void setPitch(double pitch, optional<ScreenCoordinate>, const AnimationOptions& = {});
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
    void setSize(Size);
    Size getSize() const;

    // Projection
    ScreenCoordinate pixelForLatLng(const LatLng&) const;
    LatLng latLngForPixel(const ScreenCoordinate&) const;

    // Annotations
    void addAnnotationImage(const std::string&, std::unique_ptr<style::Image>);
    void removeAnnotationImage(const std::string&);
    double getTopOffsetPixelsForAnnotationImage(const std::string&);

    AnnotationID addAnnotation(const Annotation&);
    void updateAnnotation(AnnotationID, const Annotation&);
    void removeAnnotation(AnnotationID);

    // Sources
    std::vector<style::Source*> getSources();
    style::Source* getSource(const std::string& sourceID);
    void addSource(std::unique_ptr<style::Source>);
    std::unique_ptr<style::Source> removeSource(const std::string& sourceID);

    // Layers
    std::vector<style::Layer*> getLayers();
    style::Layer* getLayer(const std::string& layerID);
    void addLayer(std::unique_ptr<style::Layer>, const optional<std::string>& beforeLayerID = {});
    std::unique_ptr<style::Layer> removeLayer(const std::string& layerID);

    // Images
    void addImage(const std::string&, std::unique_ptr<style::Image>);
    void removeImage(const std::string&);
    const style::Image* getImage(const std::string&);

    // Light
    void setLight(std::unique_ptr<style::Light>);
    style::Light* getLight();

    // Defaults
    std::string getStyleName() const;
    LatLng getDefaultLatLng() const;
    double getDefaultZoom() const;
    double getDefaultBearing() const;
    double getDefaultPitch() const;

    // Feature queries
    std::vector<Feature> queryRenderedFeatures(const ScreenCoordinate&, const RenderedQueryOptions& options = {});
    std::vector<Feature> queryRenderedFeatures(const ScreenBox&,        const RenderedQueryOptions& options = {});
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions& options = {});

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
