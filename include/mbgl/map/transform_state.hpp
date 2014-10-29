#ifndef MBGL_MAP_TRANSFORM_STATE
#define MBGL_MAP_TRANSFORM_STATE

#include <mbgl/map/tile.hpp>

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/vec.hpp>

#include <cstdint>
#include <array>
#include <limits>

namespace mbgl {

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

    float worldSize() const;
    float lngX(float lon) const;
    float latY(float lat) const;
    std::array<float, 2> locationCoordinate(float lon, float lat) const;
    void getLonLat(double &lon, double &lat) const;

    // Zoom
    float getNormalizedZoom() const;
    double getZoom() const;
    int32_t getIntegerZoom() const;
    double getZoomFraction() const;
    double getScale() const;

    // Rotation
    float getAngle() const;

    // Changing
    bool isChanging() const;

private:
    double pixel_x() const;
    double pixel_y() const;

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
    double x = 0, y = 0;
    double angle = 0;
    double scale = 1;
};

}

#endif
