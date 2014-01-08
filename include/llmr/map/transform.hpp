#ifndef LLMR_MAP_TRANSFORM
#define LLMR_MAP_TRANSFORM

#include <cstdint>

namespace llmr {

class tile;

class transform {
public:
    transform();

    // Relative changes
    void moveBy(double dx, double dy);
    void scaleBy(double ds, double cx, double cy);
    void rotateBy(double cx, double cy, double dx, double dy);

    // Absolute changes
    void setScale(double scale);
    void setAngle(double angle);
    void setZoom(double zoom);
    void setLonLat(double lon, double lat);

    // Getters
    void matrixFor(float matrix[16], uint32_t z, uint32_t x, uint32_t y) const;
    void getLonLat(double& lon, double& lat) const;

private:
    double pixel_x() const;
    double pixel_y() const;

public:
    uint32_t width;
    uint32_t height;


private:
    double x, y; // pixel values of the map center in the current scale
    double angle;
    double scale;

    // tile size
    const int32_t size = 512;

    // cache values for spherical mercator math
    double zc, Bc, Cc;
};

}

#endif
