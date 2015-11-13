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

    void jumpTo(const CameraOptions&);
    void easeTo(const CameraOptions&);

    // Position
    void moveBy(const PrecisionPoint&, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, const PrecisionPoint&, const Duration& = Duration::zero());
    void setLatLngZoom(const LatLng&, double zoom, const Duration& = Duration::zero());
    LatLng getLatLng() const { return state.getLatLng(); }

    // Zoom
    void scaleBy(double ds, const PrecisionPoint& center = { 0, 0 }, const Duration& = Duration::zero());
    void setScale(double scale, const PrecisionPoint& center = { 0, 0 }, const Duration& = Duration::zero());
    void setZoom(double zoom, const Duration& = Duration::zero());
    double getZoom() const;
    double getScale() const;

    // Angle
    void rotateBy(const PrecisionPoint& first, const PrecisionPoint& second, const Duration& = Duration::zero());
    void setAngle(double angle, const Duration& = Duration::zero());
    void setAngle(double angle, const PrecisionPoint& center);
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
    bool isGestureInProgress() const { return state.isGestureInProgress(); }

    // Transform state
    TransformState getState() const { return state; }
    bool isRotating() const { return state.isRotating(); }
    bool isScaling() const { return state.isScaling(); }
    bool isPanning() const { return state.isPanning(); }

private:
    void _moveBy(const PrecisionPoint&, const Duration& = Duration::zero());
    void _setScale(double scale, const PrecisionPoint& center, const Duration& = Duration::zero());
    void _setScaleXY(double new_scale, double xn, double yn, const Duration& = Duration::zero());
    void _easeTo(const CameraOptions&, double new_scale, double new_angle, double xn, double yn);
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
