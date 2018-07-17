#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/size.hpp>

#include <cstdint>
#include <array>
#include <limits>

namespace mbgl {

class UnwrappedTileID;

class TransformState {
public:
    TransformState(ConstrainMode = ConstrainMode::HeightOnly, ViewportMode = ViewportMode::Default);

    // Matrix
    void matrixFor(mat4&, const UnwrappedTileID&) const;
    void getProjMatrix(mat4& matrix, uint16_t nearZ = 1, bool aligned = false) const;

    // Dimensions
    Size getSize() const;
    void setSize(const Size& size);

    // North Orientation
    NorthOrientation getNorthOrientation() const;
    void setNorthOrientation(NorthOrientation);
    double getNorthOrientationAngle() const;

    // Constrain mode
    ConstrainMode getConstrainMode() const;
    void setConstrainMode(ConstrainMode);

    // Viewport mode
    ViewportMode getViewportMode() const;
    void setViewportMode(ViewportMode);

    // Projection mode
    bool getAxonometric() const;
    void setAxonometric(bool axonometric);

    double getXSkew() const;
    void setXSkew(double xSkew);

    double getYSkew() const;
    void setYSkew(double ySkew);

    CameraOptions getCameraOptions(const EdgeInsets&) const;

    // Position
    LatLng getLatLng(LatLng::WrapMode = LatLng::Unwrapped) const;

    // Zoom
    double getZoom() const;
    double getScale() const;
    uint8_t getIntegerZoom() const;

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
    float getBearing() const;
    void setBearing(double bearing);

    // Camera
    float getFieldOfView() const;
    float getCameraToCenterDistance() const;

    // Tilt
    float getPitch() const;
    void setPitch(double pitch);

    // State
    bool isChanging() const;

    bool isGestureInProgress() const;
    void setGestureInProgress(bool gestureInProgress);

    bool isTransitionInProgress() const;
    void setTransitionInProgress(bool transitionInProgress);

    // Conversion
    ScreenCoordinate latLngToScreenCoordinate(const LatLng&) const;
    LatLng screenCoordinateToLatLng(const ScreenCoordinate&, LatLng::WrapMode = LatLng::Unwrapped) const;

    double zoomScale(double zoom) const;
    double scaleZoom(double scale) const;

    bool valid() const {
        return !size.isEmpty() && (scale >= min_scale && scale <= max_scale);
    }

    float getCameraToTileDistance(const UnwrappedTileID&) const;
    float maxPitchScaleFactor() const;

    /** Recenter the map so that the given coordinate is located at the given
        point on screen. */
    void moveLatLng(const LatLng&, const ScreenCoordinate&);
    void setScalePoint(const double scale, const ScreenCoordinate& point);
    void setLatLngZoom(const LatLng &latLng, double zoom);

private:
    bool rotatedNorth() const;
    void constrain(double& scale, double& x, double& y) const;

    optional<LatLngBounds> bounds;

    // Limit the amount of zooming possible on the map.
    double min_scale = std::pow(2, 0);
    double max_scale = std::pow(2, util::DEFAULT_MAX_ZOOM);
    double min_pitch = 0.0;
    double max_pitch = util::PITCH_MAX;

    NorthOrientation orientation = NorthOrientation::Upwards;

    // logical dimensions
    Size size;

    mat4 coordinatePointMatrix(double z) const;
    mat4 getPixelMatrix() const;

    ConstrainMode constrainMode;
    ViewportMode viewportMode;

    // State
    bool gestureInProgress = false;
    bool transitionInProgress = false;

    // map position
    Point<double> position { 0, 0 };
    double bearing = 0;
    double scale = 1;
    // This fov value is somewhat arbitrary. The altitude of the camera used
    // to be defined as 1.5 screen heights above the ground, which was an
    // arbitrary choice. This is the fov equivalent to that value calculated with:
    // `fov = 2 * arctan((height / 2) / (height * 1.5))`
    double fov = 0.6435011087932844;
    double pitch = 0.0;
    double xSkew = 0.0;
    double ySkew = 1.0;
    bool axonometric = false;

    // cache values for spherical mercator math
    double Bc = Projection::worldSize(scale) / util::DEGREES_MAX;
    double Cc = Projection::worldSize(scale) / util::M2PI;
};

} // namespace mbgl
