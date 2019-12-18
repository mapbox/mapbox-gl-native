#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/map/bound_options.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/map/projection_mode.hpp>
#include <mbgl/storage/resource_options.hpp>

#include <cstdint>
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace mbgl {

class RendererFrontend;

namespace style {
class Image;
class Style;
} // namespace style

class Map : private util::noncopyable {
public:
    explicit Map(RendererFrontend&,
                 MapObserver&,
                 const MapOptions&,
                 const ResourceOptions&);
    ~Map();

    // Register a callback that will get called (on the render thread) when all resources have
    // been loaded and a complete render occurs.
    using StillImageCallback = std::function<void (std::exception_ptr)>;
    void renderStill(StillImageCallback);
    void renderStill(const CameraOptions&, MapDebugOptions, StillImageCallback);

    // Triggers a repaint.
    void triggerRepaint();

          style::Style& getStyle();
    const style::Style& getStyle() const;

    void setStyle(std::unique_ptr<style::Style>);

    // Transition
    void cancelTransitions();
    void setGestureInProgress(bool);
    bool isGestureInProgress() const;
    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;

    // Camera
    CameraOptions getCameraOptions(optional<EdgeInsets> = {}) const;
    void jumpTo(const CameraOptions&);
    void easeTo(const CameraOptions&, const AnimationOptions&);
    void flyTo(const CameraOptions&, const AnimationOptions&);
    void moveBy(const ScreenCoordinate&, const AnimationOptions& = {});
    void scaleBy(double scale, optional<ScreenCoordinate> anchor, const AnimationOptions& animation = {});
    void pitchBy(double pitch, const AnimationOptions& animation = {});
    void rotateBy(const ScreenCoordinate& first, const ScreenCoordinate& second, const AnimationOptions& = {});
    CameraOptions cameraForLatLngBounds(const LatLngBounds&, const EdgeInsets&, optional<double> bearing = {}, optional<double> pitch = {}) const;
    CameraOptions cameraForLatLngs(const std::vector<LatLng>&, const EdgeInsets&, optional<double> bearing = {}, optional<double> pitch = {}) const;
    CameraOptions cameraForGeometry(const Geometry<double>&, const EdgeInsets&, optional<double> bearing = {}, optional<double> pitch = {}) const;
    LatLngBounds latLngBoundsForCamera(const CameraOptions&) const;

    /// @name Bounds
    /// @{

    void setBounds(const BoundOptions& options);

    /// Returns the current map bound options. All optional fields in BoundOptions are set.
    BoundOptions getBounds() const;

    /// @}

    // Map Options
    void setNorthOrientation(NorthOrientation);
    void setConstrainMode(ConstrainMode);
    void setViewportMode(ViewportMode);
    void setSize(Size);
    MapOptions getMapOptions() const;

    //Projection Mode
    void setProjectionMode(const ProjectionMode&);
    ProjectionMode getProjectionMode() const;

    // Projection
    ScreenCoordinate pixelForLatLng(const LatLng&) const;
    LatLng latLngForPixel(const ScreenCoordinate&) const;
    std::vector<ScreenCoordinate> pixelsForLatLngs(const std::vector<LatLng>&) const;
    std::vector<LatLng> latLngsForPixels(const std::vector<ScreenCoordinate>&) const;

    // Annotations
    void addAnnotationImage(std::unique_ptr<style::Image>);
    void removeAnnotationImage(const std::string&);
    double getTopOffsetPixelsForAnnotationImage(const std::string&);

    AnnotationID addAnnotation(const Annotation&);
    void updateAnnotation(AnnotationID, const Annotation&);
    void removeAnnotation(AnnotationID);

    // Tile prefetching
    //
    // When loading a map, if `PrefetchZoomDelta` is set to any number greater than 0, the map will
    // first request a tile for `zoom - delta` in a attempt to display a full map at lower
    // resolution as quick as possible. It will get clamped at the tile source minimum zoom. The
    // default `delta` is 4.
    void setPrefetchZoomDelta(uint8_t delta);
    uint8_t getPrefetchZoomDelta() const;

    // Debug
    void setDebug(MapDebugOptions);
    void cycleDebugOptions();
    MapDebugOptions getDebug() const;

    bool isFullyLoaded() const;
    void dumpDebugLogs() const;

protected:
    class Impl;
    const std::unique_ptr<Impl> impl;

    // For testing only.
    Map(std::unique_ptr<Impl>);
};

} // namespace mbgl
