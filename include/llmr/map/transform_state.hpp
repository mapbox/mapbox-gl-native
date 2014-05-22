#ifndef LLMR_MAP_TRANSFORM_STATE
#define LLMR_MAP_TRANSFORM_STATE

#include <llmr/util/mat4.hpp>
#include <llmr/map/tile.hpp>

#include <cstdint>

namespace llmr {

class Transform;

class TransformState {
    friend class Transform;

public:
    // Matrix
    void matrixFor(mat4& matrix, const Tile::ID& id) const;
    box cornersToBox(uint32_t z) const;

    // Dimensions
    uint16_t getWidth() const;
    uint16_t getHeight() const;
    uint16_t getFramebufferWidth() const;
    uint16_t getFramebufferHeight() const;
    const std::array<uint16_t, 2> getFramebufferDimensions() const;
    float getPixelRatio() const;

    // Zoom
    float getNormalizedZoom() const;
    int32_t getIntegerZoom() const;
    double getZoom() const;
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
    double scale = std::numeric_limits<double>::infinity();
};

}

#endif
