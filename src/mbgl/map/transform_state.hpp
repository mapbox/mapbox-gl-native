#ifndef MBGL_MAP_TRANSFORM_STATE
#define MBGL_MAP_TRANSFORM_STATE

#include <mbgl/map/mode.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/vec4.hpp>

#include <cstdint>
#include <array>
#include <limits>

namespace mbgl {

class TileID;
struct box;
struct TileCoordinate;

class TransformState {
    friend class Transform;

public:
    TransformState(ConstrainMode = ConstrainMode::HeightOnly);

    // Matrix
    void matrixFor(mat4& matrix, const TileID& id, const int8_t z) const;
    void getProjMatrix(mat4& matrix) const;
    box cornersToBox(uint32_t z) const;

    // Dimensions
    uint16_t getWidth() const;
    uint16_t getHeight() const;

    // North Orientation
    NorthOrientation getNorthOrientation() const;
    double getNorthOrientationAngle() const;

    // Position
    LatLng getLatLng() const;
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
    bool isChanging() const;
    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;
    bool isGestureInProgress() const;

    // Conversion and projection
    PrecisionPoint latLngToPoint(const LatLng&) const;
    LatLng pointToLatLng(const PrecisionPoint&) const;

    TileCoordinate latLngToCoordinate(const LatLng&) const;
    LatLng coordinateToLatLng(const TileCoordinate&) const;

    PrecisionPoint coordinateToPoint(const TileCoordinate&) const;
    TileCoordinate pointToCoordinate(const PrecisionPoint&) const;

private:
    bool rotatedNorth() const;
    void constrain(double& scale, double& x, double& y) const;

    // Limit the amount of zooming possible on the map.
    double min_scale = std::pow(2, 0);
    double max_scale = std::pow(2, 20);

    NorthOrientation orientation = NorthOrientation::Upwards;

    // logical dimensions
    uint16_t width = 0, height = 0;

    double xLng(double x, double worldSize) const;
    double yLat(double y, double worldSize) const;
    double lngX(double lon) const;
    double latY(double lat) const;
    double zoomScale(double zoom) const;
    double scaleZoom(double scale) const;
    double worldSize() const;

    mat4 coordinatePointMatrix(double z) const;
    mat4 getPixelMatrix() const;
    
    /** Recenter the map so that the given coordinate is located at the given
        point on screen. */
    void moveLatLng(const LatLng&, const PrecisionPoint&);
    void setLatLngZoom(const LatLng &latLng, double zoom);
    void setScalePoint(const double scale, const PrecisionPoint& point);

private:
    ConstrainMode constrainMode;

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
    double Bc = (scale * util::tileSize) / 360;
    double Cc = (scale * util::tileSize) / util::M2PI;
};

} // namespace mbgl

#endif // MBGL_MAP_TRANSFORM_STATE
