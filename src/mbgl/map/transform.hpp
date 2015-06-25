#ifndef MBGL_MAP_TRANSFORM
#define MBGL_MAP_TRANSFORM

#include <mbgl/map/transform_state.hpp>
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
    // Note: width * ratio does not necessarily equal fb_width
    bool resize(uint16_t width, uint16_t height, float ratio,
                uint16_t fb_width, uint16_t fb_height);

    // Position
    void moveBy(double dx, double dy, Duration = Duration::zero());
    void setLatLng(LatLng latLng, Duration = Duration::zero());
    void setLatLngZoom(LatLng latLng, double zoom, Duration = Duration::zero());
    inline const LatLng getLatLng() const { return state.getLatLng(); }

    // Zoom
    void scaleBy(double ds, double cx = -1, double cy = -1, Duration = Duration::zero());
    void setScale(double scale, double cx = -1, double cy = -1, Duration = Duration::zero());
    void setZoom(double zoom, Duration = Duration::zero());
    double getZoom() const;
    double getScale() const;

    // Angle
    void rotateBy(double sx, double sy, double ex, double ey, Duration = Duration::zero());
    void setAngle(double angle, Duration = Duration::zero());
    void setAngle(double angle, double cx, double cy);
    double getAngle() const;

    // Transitions
    bool needsTransition() const;
    UpdateType updateTransitions(TimePoint now);
    void cancelTransitions();

    // Gesture
    void setGestureInProgress(bool);

    // Transform state
    const TransformState currentState() const;

private:
    void _moveBy(double dx, double dy, Duration = Duration::zero());
    void _setScale(double scale, double cx, double cy, Duration = Duration::zero());
    void _setScaleXY(double new_scale, double xn, double yn, Duration = Duration::zero());
    void _setAngle(double angle, Duration = Duration::zero());

    View &view;

    TransformState state;

    void startTransition(std::function<Update(double)> frame,
                         std::function<void()> finish,
                         Duration);

    TimePoint transitionStart;
    Duration transitionDuration;
    std::function<Update(TimePoint)> transitionFrameFn;
    std::function<void()> transitionFinishFn;
};

}

#endif
