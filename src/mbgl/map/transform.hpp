#ifndef MBGL_MAP_TRANSFORM
#define MBGL_MAP_TRANSFORM

#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <cstdint>
#include <cmath>
#include <functional>

namespace mbgl {

class View;

class Transform : private util::noncopyable {
public:
    Transform(View&);

    // Map view
    bool resize(std::array<uint16_t, 2> size);

    void jumpTo(const CameraOptions options);
    void easeTo(const CameraOptions options);

    // Position
    void moveBy(double dx, double dy, const Duration& = Duration::zero());
    void setLatLng(LatLng latLng, const Duration& = Duration::zero());
    void setLatLngZoom(LatLng latLng, double zoom, const Duration& = Duration::zero());
    inline const LatLng getLatLng() const { return state.getLatLng(); }

    // Zoom
    void scaleBy(double ds, double cx = -1, double cy = -1, const Duration& = Duration::zero());
    void setScale(double scale, double cx = -1, double cy = -1, const Duration& = Duration::zero());
    void setZoom(double zoom, const Duration& = Duration::zero());
    double getZoom() const;
    double getScale() const;

    // Angle
    void rotateBy(double sx, double sy, double ex, double ey, const Duration& = Duration::zero());
    void setAngle(double angle, const Duration& = Duration::zero());
    void setAngle(double angle, double cx, double cy);
    double getAngle() const;

    // Pitch
    void setPitch(double pitch, const Duration& = Duration::zero());
    double getPitch() const;

    // Transitions
    bool inTransition() const;
    Update updateTransitions(const TimePoint& now);
    void cancelTransitions();

    // Gesture
    void setGestureInProgress(bool);

    // Transform state
    const TransformState getState() const { return state; }

private:
    void _moveBy(double dx, double dy, const Duration& = Duration::zero());
    void _setScale(double scale, double cx, double cy, const Duration& = Duration::zero());
    void _setScaleXY(double new_scale, double xn, double yn, const Duration& = Duration::zero());
    void _easeTo(CameraOptions options, const double new_scale, const double new_angle,
                 const double xn, const double yn);
    void _setAngle(double angle, const Duration& = Duration::zero());

    View &view;

    TransformState state;

    void startTransition(std::function<double(double)> easing,
                         std::function<Update(double)> frame,
                         std::function<void()> finish,
                         const Duration& duration);

    TimePoint transitionStart;
    Duration transitionDuration;
    std::function<Update(const TimePoint)> transitionFrameFn;
    std::function<void()> transitionFinishFn;
};

}

#endif
