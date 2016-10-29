#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/size.hpp>

#include <cstdint>
#include <array>
#include <limits>

namespace mbgl {

class UnwrappedTileID;

class TransformState {
    friend class Transform;

public:
    TransformState(ConstrainMode = ConstrainMode::HeightOnly, ViewportMode = ViewportMode::Default);

    // Matrix
    void matrixFor(mat4&, const UnwrappedTileID&) const;
    void getProjMatrix(mat4& matrix) const;

    // Dimensions
    Size getSize() const;

    // North Orientation
    NorthOrientation getNorthOrientation() const;
    double getNorthOrientationAngle() const;

    // Constrain mode
    ConstrainMode getConstrainMode() const;

    // Viewport mode
    ViewportMode getViewportMode() const;

    // Position
    LatLng getLatLng(LatLng::WrapMode = LatLng::Unwrapped) const;
    double pixel_x() const;
    double pixel_y() const;

    // Zoom
    double getScale() const;
    double getZoom() const;
    int32_t getIntegerZoom() const;
    double getZoomFraction() const;
    void setMinZoom(const double minZoom);
    double getMinZoom() const;
    void setMaxZoom(const double maxZoom);
    double getMaxZoom() const;

    // Rotation
    float getAngle() const;
    float getAltitude() const;
    float getPitch() const;

    // State
    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;
    bool isGestureInProgress() const;

    // Conversion
    ScreenCoordinate latLngToScreenCoordinate(const LatLng&) const;
    LatLng screenCoordinateToLatLng(const ScreenCoordinate&, LatLng::WrapMode = LatLng::Unwrapped) const;

    double zoomScale(double zoom) const;
    double scaleZoom(double scale) const;

private:
    bool rotatedNorth() const;
    void constrain(double& scale, double& x, double& y) const;

    // Limit the amount of zooming possible on the map.
    double min_scale = std::pow(2, 0);
    double max_scale = std::pow(2, 20);

    NorthOrientation orientation = NorthOrientation::Upwards;

    // logical dimensions
    Size size;

    mat4 coordinatePointMatrix(double z) const;
    mat4 getPixelMatrix() const;

    /** Recenter the map so that the given coordinate is located at the given
        point on screen. */
    void moveLatLng(const LatLng&, const ScreenCoordinate&);
    void setLatLngZoom(const LatLng &latLng, double zoom);
    void setScalePoint(const double scale, const ScreenCoordinate& point);

private:
    ConstrainMode constrainMode;
    ViewportMode viewportMode;

    // animation state
    bool rotating = false;
    bool scaling = false;
    bool panning = false;
    bool gestureInProgress = false;

    // map position
    double x = 0, y = 0;
    double angle = 0;
    double scale = 1;
    double altitude = 1.5;
    double pitch = 0.0;

    // cache values for spherical mercator math
    double Bc = Projection::worldSize(scale) / util::DEGREES_MAX;
    double Cc = Projection::worldSize(scale) / util::M2PI;
};

} // namespace mbgl
