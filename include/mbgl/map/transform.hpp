#ifndef MBGL_MAP_TRANSFORM
#define MBGL_MAP_TRANSFORM

#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/vec.hpp>

#include <cstdint>
#include <cmath>
#include <forward_list>
#include <mutex>
#include <chrono>

namespace mbgl {

class View;
namespace util { class transition; }

class Transform : private util::noncopyable {
public:
    Transform(View &view);

    // Map view
    // Note: width * ratio does not necessarily equal fb_width
    bool resize(uint16_t width, uint16_t height, float ratio,
                uint16_t fb_width, uint16_t fb_height);

    // Position
    void moveBy(double dx, double dy, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setLatLng(LatLng latLng, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setLatLngZoom(LatLng latLng, double zoom, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    inline const LatLng getLatLng() const { return current.getLatLng(); }
    void startPanning();
    void stopPanning();

    // Zoom
    void scaleBy(double ds, double cx = -1, double cy = -1, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setScale(double scale, double cx = -1, double cy = -1, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setZoom(double zoom, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    double getZoom() const;
    double getScale() const;
    void startScaling();
    void stopScaling();
    double getMinZoom() const;
    double getMaxZoom() const;

    // Angle
    void rotateBy(double sx, double sy, double ex, double ey, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setAngle(double angle, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void setAngle(double angle, double cx, double cy);
    double getAngle() const;
    void startRotating();
    void stopRotating();

    // Transitions
    bool needsTransition() const;
    void updateTransitions(std::chrono::steady_clock::time_point now);
    void cancelTransitions();

    // Transform state
    const TransformState currentState() const;
    const TransformState finalState() const;

private:
    // Functions prefixed with underscores will *not* perform any locks. It is the caller's
    // responsibility to lock this object.
    void _moveBy(double dx, double dy, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void _setScale(double scale, double cx, double cy, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void _setScaleXY(double new_scale, double xn, double yn, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void _setAngle(double angle, std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void _clearPanning();
    void _clearRotating();
    void _clearScaling();

    void constrain(double& scale, double& y) const;

private:
    View &view;

    mutable std::recursive_mutex mtx;

    // This reflects the current state of the transform, representing the actual position of the
    // map. After calling a transform function with a timer, this will likely remain the same until
    // you render a new frame.
    TransformState current;

    // This reflects the final position of the transform, after all possible transition took place.
    TransformState final;

    // Limit the amount of zooming possible on the map.
    const double min_scale = std::pow(2, 0);
    const double max_scale = std::pow(2, 18);

    std::forward_list<util::ptr<util::transition>> transitions;
    util::ptr<util::transition> scale_timeout;
    util::ptr<util::transition> rotate_timeout;
    util::ptr<util::transition> pan_timeout;
};

}

#endif
