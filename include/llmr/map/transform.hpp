#ifndef LLMR_MAP_TRANSFORM
#define LLMR_MAP_TRANSFORM


#include <llmr/util/vec.hpp>
#include <llmr/util/animation.hpp>

#include <cstdint>
#include <forward_list>

namespace llmr {

struct box;

class tile;

class transform {
public:
    transform();

    // Animations
    bool needsAnimation() const;
    void updateAnimations();

    // Relative changes
    void moveBy(double dx, double dy);
    void scaleBy(double ds, double cx, double cy);
    void rotateBy(double cx, double cy, double sx, double sy, double ex, double ey);

    // Absolute changes
    void setScale(double scale);
    void setAngle(double angle, double duration = 0);
    void setZoom(double zoom);
    void setLonLat(double lon, double lat);

    // Getters
    void matrixFor(float matrix[16], const vec3<int32_t>& id) const;
    int32_t getZoom() const;
    double getScale() const;
    double getAngle() const;
    void getLonLat(double& lon, double& lat) const;


    // Temporary
    void mapCornersToBox(uint32_t z, box& b) const;

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

    double min_scale, max_scale;

    // tile size
    const int32_t size = 512;

    // cache values for spherical mercator math
    double zc, Bc, Cc;

    std::forward_list<util::animation> animations;
};

}

#endif
