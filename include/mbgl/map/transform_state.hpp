#ifndef MBGL_MAP_TRANSFORM_STATE
#define MBGL_MAP_TRANSFORM_STATE

#include <mbgl/map/tile.hpp>
#include <mbgl/geometry/lat_lng.hpp>
#include <mbgl/geometry/point.hpp>

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/vec.hpp>

#include <cstdint>
#include <array>

namespace mbgl {

class LatLng;
class Point;

class TransformState {
    friend class Transform;

public:
    // Matrix
    void matrixFor(mat4& matrix, const Tile::ID& id) const;
    box cornersToBox(uint32_t z) const;

    // Dimensions
    bool hasSize() const;
    uint16_t getWidth() const;
    uint16_t getHeight() const;
    uint16_t getFramebufferWidth() const;
    uint16_t getFramebufferHeight() const;
    const std::array<uint16_t, 2> getFramebufferDimensions() const;
    float getPixelRatio() const;
    Point centerPoint() const;

    // Center
    LatLng getCenter() const;
    Point getPoint() const;

    // Zoom
    float getNormalizedZoom() const;
    int32_t getIntegerZoom() const;
    double getZoom() const;
    double getScale() const;

    // Bearing
    double getBearing() const;
    double getAngle() const;

    // Projection
    Point project(const LatLng&) const;
    LatLng unproject(const Point&) const;

    // Changing
    bool isChanging() const;

private:
    // logical dimensions
    uint16_t width = 0, height = 0;

    // physical (framebuffer) dimensions
    std::array<uint16_t, 2> framebuffer = {{ 0, 0 }};

    // map scale factor
    float pixelRatio = 0;

    // animation state
    bool rotating = false;
    bool scaling = false;
    bool panning = false;

    // map position
    LatLng center;
    double bearing = 0;
    double zoom = 0;
};

}

#endif
