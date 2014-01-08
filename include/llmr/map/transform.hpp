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

    // Absolute changes
    void setScale(double scale);
    void setZoom(double zoom);
    void setLonLat(double lon, double lat);
    // void setAngle(double angle);


    // Getters
    void matrixFor(float matrix[16], uint32_t z, uint32_t x, uint32_t y) const;
    void getLonLat(double& lon, double& lat) const;

private:
    double pixel_x() const;
    double pixel_y() const;

public:
    // double x, y;
    uint32_t width;
    uint32_t height;


private:
    double x, y; // spherical mercator meters of the view center
    double angle;
    double scale;


    const int32_t size = 512;

    double zc, Bc, Cc;
};

}

#endif
