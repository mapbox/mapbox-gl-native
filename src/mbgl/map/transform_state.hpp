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

struct TransformStateProperties {
    TransformStateProperties& withX(const optional<double>& val) {
        x = val;
        return *this;
    }
    TransformStateProperties& withY(const optional<double>& val) {
        y = val;
        return *this;
    }
    TransformStateProperties& withScale(const optional<double>& val) {
        scale = val;
        return *this;
    }
    TransformStateProperties& withBearing(const optional<double>& val) {
        bearing = val;
        return *this;
    }
    TransformStateProperties& withPitch(const optional<double>& val) {
        pitch = val;
        return *this;
    }
    TransformStateProperties& withXSkew(const optional<double>& val) {
        xSkew = val;
        return *this;
    }
    TransformStateProperties& withYSkew(const optional<double>& val) {
        ySkew = val;
        return *this;
    }
    TransformStateProperties& withAxonometric(const optional<bool>& val) {
        axonometric = val;
        return *this;
    }
    TransformStateProperties& withPanningInProgress(const optional<bool>& val) {
        panning = val;
        return *this;
    }
    TransformStateProperties& withScalingInProgress(const optional<bool>& val) {
        scaling = val;
        return *this;
    }
    TransformStateProperties& withRotatingInProgress(const optional<bool>& val) {
        rotating = val;
        return *this;
    }
    TransformStateProperties& withEdgeInsets(const optional<EdgeInsets>& val) {
        edgeInsets = val;
        return *this;
    }
    TransformStateProperties& withSize(const optional<Size>& val) {
        size = val;
        return *this;
    }
    TransformStateProperties& withConstrainMode(const optional<ConstrainMode>& val) {
        constrain = val;
        return *this;
    }
    TransformStateProperties& withNorthOrientation(const optional<NorthOrientation>& val) {
        northOrientation = val;
        return *this;
    }
    TransformStateProperties& withViewportMode(const optional<ViewportMode>& val) {
        viewPortMode = val;
        return *this;
    }

    optional<double> x;
    optional<double> y;
    optional<double> bearing;
    optional<double> scale;
    optional<double> pitch;
    optional<double> xSkew;
    optional<double> ySkew;
    optional<bool> axonometric;
    optional<bool> panning;
    optional<bool> scaling;
    optional<bool> rotating;
    optional<EdgeInsets> edgeInsets;
    optional<Size> size;
    optional<ConstrainMode> constrain;
    optional<NorthOrientation> northOrientation;
    optional<ViewportMode> viewPortMode;
};

class TransformState {
public:
    TransformState(ConstrainMode = ConstrainMode::HeightOnly, ViewportMode = ViewportMode::Default);

    void setProperties(const TransformStateProperties& properties);

    // Matrix
    void matrixFor(mat4&, const UnwrappedTileID&) const;
    void getProjMatrix(mat4& matrix, uint16_t nearZ = 1, bool aligned = false) const;

    // Dimensions
    Size getSize() const;
    void setSize(const Size& size_);

    // North Orientation
    NorthOrientation getNorthOrientation() const;
    double getNorthOrientationAngle() const;
    void setNorthOrientation(const NorthOrientation);

    // Constrain mode
    ConstrainMode getConstrainMode() const;
    void setConstrainMode(const ConstrainMode);

    // Viewport mode
    ViewportMode getViewportMode() const;
    void setViewportMode(ViewportMode val);

    CameraOptions getCameraOptions(optional<EdgeInsets>) const;

    // EdgeInsects
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
    double getScale() const;
    void setScale(double);

    // Positions
    double getX() const;
    void setX(double);
    double getY() const;
    void setY(double);

    // Bounds
    void setLatLngBounds(LatLngBounds);
    LatLngBounds getLatLngBounds() const;
    void setMinZoom(double);
    double getMinZoom() const;
    void setMaxZoom(double);
    double getMaxZoom() const;

    // Rotation
    double getBearing() const;
    void setBearing(double);
    float getFieldOfView() const;
    float getCameraToCenterDistance() const;
    double getPitch() const;
    void setPitch(double);

    double getXSkew() const;
    void setXSkew(double);
    double getYSkew() const;
    void setYSkew(double);
    bool getAxonometric() const;
    void setAxonometric(bool);

    // State
    bool isChanging() const;
    bool isRotating() const;
    void setRotatingInProgress(bool val) { rotating = val; }
    bool isScaling() const;
    void setScalingInProgress(bool val) { scaling = val; }
    bool isPanning() const;
    void setPanningInProgress(bool val) { panning = val; }
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

    /** Recenter the map so that the given coordinate is located at the given
        point on screen. */
    void moveLatLng(const LatLng&, const ScreenCoordinate&);
    void setLatLngZoom(const LatLng& latLng, double zoom);

    void constrain(double& scale, double& x, double& y) const;

private:
    bool rotatedNorth() const;

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

    mat4 coordinatePointMatrix(const mat4& projMatrix) const;
    mat4 getPixelMatrix() const;

    void setScalePoint(const double scale, const ScreenCoordinate& point);

    void updateMatricesIfNeeded() const;
    bool needsMatricesUpdate() const { return requestMatricesUpdate; }
    const mat4& getProjectionMatrix() const;
    const mat4& getCoordMatrix() const;
    const mat4& getInvertedMatrix() const;

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

    mutable bool requestMatricesUpdate{true};
    mutable mat4 projectionMatrix;
    mutable mat4 coordMatrix;
    mutable mat4 invertedMatrix;
};

} // namespace mbgl
