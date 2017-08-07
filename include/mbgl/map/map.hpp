#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/map/camera.hpp>

#include <cstdint>
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace mbgl {

class FileSource;
class Scheduler;
class RendererFrontend;

namespace style {
class Image;
class Style;
} // namespace style

class Map : private util::noncopyable {
public:
    explicit Map(RendererFrontend&,
                 MapObserver&,
                 Size size,
                 float pixelRatio,
                 FileSource&,
                 Scheduler&,
                 MapMode mapMode = MapMode::Continuous,
                 ConstrainMode constrainMode = ConstrainMode::HeightOnly,
                 ViewportMode viewportMode = ViewportMode::Default);
    ~Map();

    // Register a callback that will get called (on the render thread) when all resources have
    // been loaded and a complete render occurs.
    using StillImageCallback = std::function<void (std::exception_ptr)>;
    void renderStill(StillImageCallback callback);

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
    void addAnnotationImage(std::unique_ptr<style::Image>);
    void removeAnnotationImage(const std::string&);
    double getTopOffsetPixelsForAnnotationImage(const std::string&);

    AnnotationID addAnnotation(const Annotation&);
    void updateAnnotation(AnnotationID, const Annotation&);
    void removeAnnotation(AnnotationID);

    // Tile prefetching
    //
    // When loading a map, if `PrefetchZoomDelta` is set to any number greater than 0, the map will
    // first request a tile for `zoom = getZoom() - delta` in a attempt to display a full map at
    // lower resolution as quick as possible. It will get clamped at the tile source minimum zoom.
    // The default `delta` is 4.
    void setPrefetchZoomDelta(uint8_t delta);
    uint8_t getPrefetchZoomDelta() const;

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
