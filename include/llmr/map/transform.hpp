#ifndef LLMR_MAP_TRANSFORM
#define LLMR_MAP_TRANSFORM

#include <llmr/util/animation.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/util/uv.hpp>

#include "transform_state.hpp"

#include <forward_list>

namespace llmr {

struct box;

    class Transform : private util::noncopyable {
public:
    Transform();

    // Map view
    // Note: width * ratio does not necessarily equal fb_width
    bool resize(uint16_t width, uint16_t height, float ratio,
                uint16_t fb_width, uint16_t fb_height);

    // Position
    void moveBy(double dx, double dy, double duration = 0);
    void setLonLat(double lon, double lat, double duration = 0);
    void setLonLatZoom(double lon, double lat, double zoom, double duration = 0);
    void getLonLat(double& lon, double& lat) const;
    void getLonLatZoom(double& lon, double& lat, double& zoom) const;
    void startPanning();
    void stopPanning();

    // Zoom
    void scaleBy(double ds, double cx = -1, double cy = -1, double duration = 0);
    void setScale(double scale, double cx = -1, double cy = -1, double duration = 0);
    void setZoom(double zoom, double duration = 0);
    double getZoom() const;
    double getScale() const;
    void startRotating();
    void stopRotating();

    // Angle
    void rotateBy(double sx, double sy, double ex, double ey, double duration = 0);
    void setAngle(double angle, double duration = 0);
    void setAngle(double angle, double cx, double cy);
    double getAngle() const;
    void startScaling();
    void stopScaling();

    // Animations
    bool needsAnimation() const;
    void updateAnimations(double time);
    void cancelAnimations();

    // Transform state
    const TransformState currentState() const;
    const TransformState finalState() const;

private:
    // Functions prefixed with underscores will *not* perform any locks. It is the caller's
    // responsibility to lock this object.
    void _moveBy(double dx, double dy, double duration = 0);
    void _setScale(double scale, double cx, double cy, double duration = 0);
    void _setScaleXY(double new_scale, double xn, double yn, double duration = 0);
    void _setAngle(double angle, double duration = 0);
    void _clearPanning();
    void _clearRotating();
    void _clearScaling();

private:
    mutable uv::rwlock mtx;

    // This reflects the current state of the transform, representing the actual position of the
    // map. After calling a transform function with a timer, this will likely remain the same until
    // you render a new frame.
    TransformState current;

    // This reflects the final position of the transform, after all possible animations took place.
    TransformState final;

    // Limit the amount of zooming possible on the map.
    // TODO: make these modifiable from outside.
    const double min_scale = pow(2, 0);
    const double max_scale = pow(2, 20);

    // cache values for spherical mercator math
    double zc, Bc, Cc;

    std::forward_list<std::shared_ptr<util::animation>> animations;
    std::shared_ptr<util::animation> scale_timeout;
    std::shared_ptr<util::animation> rotate_timeout;
    std::shared_ptr<util::animation> pan_timeout;
};

}

#endif
