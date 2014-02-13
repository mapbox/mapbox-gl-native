#ifndef LLMR_MAP_TRANSFORM
#define LLMR_MAP_TRANSFORM

#include <llmr/util/vec.hpp>
#include <llmr/util/mat4.hpp>
#include <llmr/util/animation.hpp>
#include <llmr/util/noncopyable.hpp>

#include <forward_list>

namespace llmr {

struct box;

    class Transform : private util::noncopyable {
public:
    Transform();

    // Animations
    bool needsAnimation() const;
    void updateAnimations();
    void cancelAnimations();

    // Relative changes
    void moveBy(double dx, double dy, double duration = 0);
    void scaleBy(double ds, double cx = -1, double cy = -1, double duration = 0);
    void rotateBy(double cx, double cy, double sx, double sy, double ex, double ey, double duration = 0);

    // Absolute changes
    void setScale(double scale, double cx = -1, double cy = -1, double duration = 0);
    void setAngle(double angle, double duration = 0);
    void setZoom(double zoom, double duration = 0);
    void setLonLat(double lon, double lat, double duration = 0);
    void setLonLatZoom(double lon, double lat, double zoom, double duration = 0);

    // Getters
    void matrixFor(mat4& matrix, const vec3<int32_t>& id) const;
    float getZoom() const;
    int32_t getIntegerZoom() const;
    double getScale() const;
    double getAngle() const;
    void getLonLat(double& lon, double& lat) const;
    void getLonLatZoom(double& lon, double& lat, double& zoom) const;

    // Temporary
    void mapCornersToBox(uint32_t z, box& b) const;

private:
    void setScaleXY(double new_scale, double xn, double yn, double duration = 0);
    double pixel_x() const;
    double pixel_y() const;

public:
    // logical dimensions
    uint32_t width = 0;
    uint32_t height = 0;

    // physical (framebuffer) dimensions
    float fb_width = 0;
    float fb_height = 0;

    float pixelRatio = 1;

private:
    double x = 0, y = 0; // pixel values of the map center in the current scale
    double angle = 0;
    double scale = 1;

    double min_scale = pow(2, 0);
    double max_scale = pow(2, 20);

    // tile size
    const int32_t size = 512;

    // cache values for spherical mercator math
    double zc, Bc, Cc;

    std::forward_list<util::animation> animations;
};

}

#endif
