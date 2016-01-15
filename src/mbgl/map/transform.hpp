#ifndef MBGL_MAP_TRANSFORM
#define MBGL_MAP_TRANSFORM

#include <mbgl/map/camera.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <cstdint>
#include <cmath>
#include <functional>

namespace mbgl {

class View;

class Transform : private util::noncopyable {
public:
    Transform(View&, ConstrainMode);

    // Map view
    bool resize(std::array<uint16_t, 2> size);

    // Camera
    
    /** Instantaneously, synchronously applies the given camera options. */
    void jumpTo(const CameraOptions&);
    /** Asynchronously transitions all specified camera options linearly along
        an optional time curve. */
    void easeTo(const CameraOptions&, const AnimationOptions& = {});
    /** Asynchronously zooms out, pans, and zooms back into the given camera
        along a great circle, as though the viewer is riding a supersonic
        jetcopter. */
    void flyTo(const CameraOptions&, const AnimationOptions& = {});

    // Position
    
    /** Pans the map by the given amount.
        @param offset The distance to pan the map by, measured in pixels from
            top to bottom and from left to right. */
    void moveBy(const PrecisionPoint& offset, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, const EdgeInsets&, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, const PrecisionPoint&, const Duration& = Duration::zero());
    void setLatLngZoom(const LatLng&, double zoom, const Duration& = Duration::zero());
    void setLatLngZoom(const LatLng&, double zoom, const EdgeInsets&, const Duration& = Duration::zero());
    LatLng getLatLng(const EdgeInsets& = {}) const;

    // Zoom

    /** Scales the map, keeping the given point fixed within the view.
        @param ds The difference in scale factors to scale the map by.
        @param anchor A point relative to the top-left corner of the view.
            If unspecified, the center point is fixed within the view. */
    void scaleBy(double ds, const PrecisionPoint& anchor = {NAN, NAN}, const Duration& = Duration::zero());
    /** Sets the scale factor, keeping the given point fixed within the view.
        @param scale The new scale factor.
        @param anchor A point relative to the top-left corner of the view.
            If unspecified, the center point is fixed within the view. */
    void setScale(double scale, const PrecisionPoint& anchor = {NAN, NAN}, const Duration& = Duration::zero());
    /** Sets the scale factor, keeping the center point fixed within the inset view.
        @param scale The new scale factor.
        @param padding The viewport padding that affects the fixed center point. */
    void setScale(double scale, const EdgeInsets& padding, const Duration& = Duration::zero());
    /** Sets the zoom level, keeping the given point fixed within the view.
        @param zoom The new zoom level.
        @param anchor A point relative to the top-left corner of the view.
            If unspecified, the center point is fixed within the view. */
    void setZoom(double zoom, const PrecisionPoint& anchor = {NAN, NAN}, const Duration& = Duration::zero());
    /** Sets the zoom level, keeping the center point fixed within the inset view.
        @param zoom The new zoom level.
        @param padding The viewport padding that affects the fixed center point. */
    void setZoom(double zoom, const EdgeInsets& padding, const Duration& = Duration::zero());
    /** Returns the zoom level. */
    double getZoom() const;
    /** Returns the scale factor. */
    double getScale() const;

    // Angle

    void rotateBy(const PrecisionPoint& first, const PrecisionPoint& second, const Duration& = Duration::zero());
    /** Sets the angle of rotation.
        @param angle The new angle of rotation, measured in radians
            counterclockwise from true north. */
    void setAngle(double angle, const Duration& = Duration::zero());
    /** Sets the angle of rotation, keeping the given point fixed within the view.
        @param angle The new angle of rotation, measured in radians
            counterclockwise from true north.
        @param anchor A point relative to the top-left corner of the view. */
    void setAngle(double angle, const PrecisionPoint& anchor, const Duration& = Duration::zero());
    /** Sets the angle of rotation, keeping the center point fixed within the inset view.
        @param angle The new angle of rotation, measured in radians
            counterclockwise from true north.
        @param padding The viewport padding that affects the fixed center point. */
    void setAngle(double angle, const EdgeInsets& padding, const Duration& = Duration::zero());
    /** Returns the angle of rotation.
        @return The angle of rotation, measured in radians counterclockwise from
            true north. */
    double getAngle() const;

    // Pitch
    void setPitch(double pitch, const Duration& = Duration::zero());
    double getPitch() const;

    // North Orientation
    void setNorthOrientation(NorthOrientation);
    NorthOrientation getNorthOrientation() const;

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
    void unwrapLatLng(LatLng&);
    
    View &view;

    TransformState state;

    void startTransition(const CameraOptions&,
                         const AnimationOptions&,
                         std::function<Update(double)>,
                         const Duration&);

    TimePoint transitionStart;
    Duration transitionDuration;
    std::function<Update(const TimePoint)> transitionFrameFn;
    std::function<void()> transitionFinishFn;
};

} // namespace mbgl

#endif
