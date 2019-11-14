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
class TileCoordinate;

class TransformState {
public:
    TransformState(ConstrainMode = ConstrainMode::HeightOnly, ViewportMode = ViewportMode::Default);

    // Matrix
    void matrixFor(mat4&, const UnwrappedTileID&) const;
    void getProjMatrix(mat4& matrix, uint16_t nearZ = 1, bool aligned = false) const;

    // Dimensions
    Size getSize() const;
    void setSize(const Size& size_);

    // North Orientation
    NorthOrientation getNorthOrientation() const;
    double getNorthOrientationAngle() const;
    void setNorthOrientation(const NorthOrientation&);

    // Constrain mode
    ConstrainMode getConstrainMode() const;
    void setConstrainMode(const ConstrainMode&);

    // Viewport mode
    ViewportMode getViewportMode() const;
    void setViewportMode(ViewportMode val) { viewportMode = val; }

    CameraOptions getCameraOptions(optional<EdgeInsets>) const;

    EdgeInsets getEdgeInsets() const { return edgeInsets; }
    void setEdgeInsets(const EdgeInsets&);
    // Position
    LatLng getLatLng(LatLng::WrapMode = LatLng::Unwrapped) const;
    double pixel_x() const;
    double pixel_y() const;

    // Zoom
    double getZoom() const;
    uint8_t getIntegerZoom() const;
    double getZoomFraction() const;

    // Scale
    double getScale() const { return scale; }
    void setScale(double val) { scale = val; }

    // Bounds
    void setLatLngBounds(LatLngBounds);
    LatLngBounds getLatLngBounds() const;
    void setMinZoom(double);
    double getMinZoom() const;
    void setMaxZoom(double);
    double getMaxZoom() const;

    // Rotation
    float getBearing() const;
    void setBearing(float);
    float getFieldOfView() const;
    float getCameraToCenterDistance() const;
    float getPitch() const;
    void setPitch(float);

    double getXSkew() const { return xSkew; }
    void setXSkew(double val);
    double getYSkew() const { return ySkew; }
    void setYSkew(double val);
    bool getAxonometric() const { return axonometric; }
    void setAxonometric(bool val) { axonometric = val; }

    // State
    bool isChanging() const;
    bool isRotating() const;
    void setRotating(bool val) { rotating = val; }
    bool isScaling() const;
    void setScaling(bool val) { scaling = val; }
    bool isPanning() const;
    void setPanning(bool val) { panning = val; }
    bool isGestureInProgress() const;
    void setGestureInProgress(bool val) { gestureInProgress = val; }

    // Conversion
    ScreenCoordinate latLngToScreenCoordinate(const LatLng&) const;
    LatLng screenCoordinateToLatLng(const ScreenCoordinate&, LatLng::WrapMode = LatLng::Unwrapped) const;
    // Implements mapbox-gl-js pointCoordinate() : MercatorCoordinate.
    TileCoordinate screenCoordinateToTileCoordinate(const ScreenCoordinate&, uint8_t atZoom) const;

    double zoomScale(double zoom) const;
    double scaleZoom(double scale) const;

    bool valid() const { return !size.isEmpty() && (scale >= min_scale && scale <= max_scale); }

    float getCameraToTileDistance(const UnwrappedTileID&) const;
    float maxPitchScaleFactor() const;

    void moveLatLng(const LatLng&, const ScreenCoordinate&);

    void setLatLngZoom(const LatLng& latLng, double zoom);

private:
    void updateMatrix();
    bool rotatedNorth() const;
    void constrain();
    void constrain(double& scale, double& x, double& y) const;

    // Viewport center offset, from [size.width / 2, size.height / 2], defined
    // by |edgeInsets| in screen coordinates, with top left origin.
    ScreenCoordinate getCenterOffset() const;

    LatLngBounds bounds;

    // Limit the amount of zooming possible on the map.
    double min_scale = std::pow(2, 0);
    double max_scale = std::pow(2, util::DEFAULT_MAX_ZOOM);

    NorthOrientation orientation = NorthOrientation::Upwards;

    // logical dimensions
    Size size;

    mat4 coordinatePointMatrix() const;
    mat4 getPixelMatrix() const;

    /** Recenter the map so that the given coordinate is located at the given
        point on screen. */

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

    EdgeInsets edgeInsets;

    // cache values for spherical mercator math
    double Bc = Projection::worldSize(scale) / util::DEGREES_MAX;
    double Cc = Projection::worldSize(scale) / util::M2PI;

    mat4 coordiMatrix;
    mat4 invertedMatrix;
};

} // namespace mbgl
