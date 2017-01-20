#pragma once

#include <mbgl/map/camera.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/map/change.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>

#include <cstdint>
#include <cmath>
#include <functional>

namespace mbgl {

class Transform : private util::noncopyable {
public:
    Transform(std::function<void(MapChange)> = nullptr,
              ConstrainMode = ConstrainMode::HeightOnly,
              ViewportMode = ViewportMode::Default);

    // Map view
    bool resize(Size size);

    // Camera
    /** Returns the current camera options. */
    CameraOptions getCameraOptions(optional<EdgeInsets>) const;

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
    void moveBy(const ScreenCoordinate& offset, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, optional<EdgeInsets>, const Duration& = Duration::zero());
    void setLatLng(const LatLng&, optional<ScreenCoordinate>, const Duration& = Duration::zero());
    void setLatLngZoom(const LatLng&, double zoom, const Duration& = Duration::zero());
    void setLatLngZoom(const LatLng&, double zoom, optional<EdgeInsets>, const Duration& = Duration::zero());
    LatLng getLatLng(optional<EdgeInsets> = {}) const;
    ScreenCoordinate getScreenCoordinate(optional<EdgeInsets> = {}) const;

    // Zoom

    /** Scales the map, keeping the given point fixed within the view.
        @param ds The difference in scale factors to scale the map by. */
    void scaleBy(double ds, const Duration& = Duration::zero());
    /** Scales the map, keeping the given point fixed within the view.
        @param ds The difference in scale factors to scale the map by.
        @param anchor A point relative to the top-left corner of the view.
            If unspecified, the center point is fixed within the view. */
    void scaleBy(double ds, optional<ScreenCoordinate> anchor, const Duration& = Duration::zero());
    /** Sets the scale factor, keeping the given point fixed within the view.
        @param scale The new scale factor. */
    void setScale(double scale, const Duration& = Duration::zero());
    /** Sets the scale factor, keeping the given point fixed within the view.
        @param scale The new scale factor.
        @param anchor A point relative to the top-left corner of the view.
            If unspecified, the center point is fixed within the view. */
    void setScale(double scale, optional<ScreenCoordinate> anchor, const Duration& = Duration::zero());
    /** Sets the scale factor, keeping the center point fixed within the inset view.
        @param scale The new scale factor.
        @param padding The viewport padding that affects the fixed center point. */
    void setScale(double scale, optional<EdgeInsets> padding, const Duration& = Duration::zero());
    /** Sets the zoom level, keeping the given point fixed within the view.
        @param zoom The new zoom level. */
    void setZoom(double zoom, const Duration& = Duration::zero());
    /** Sets the zoom level, keeping the given point fixed within the view.
        @param zoom The new zoom level.
        @param anchor A point relative to the top-left corner of the view.
            If unspecified, the center point is fixed within the view. */
    void setZoom(double zoom, optional<ScreenCoordinate> anchor, const Duration& = Duration::zero());
    /** Sets the zoom level, keeping the center point fixed within the inset view.
        @param zoom The new zoom level.
        @param padding The viewport padding that affects the fixed center point. */
    void setZoom(double zoom, optional<EdgeInsets> padding, const Duration& = Duration::zero());
    /** Returns the zoom level. */
    double getZoom() const;
    /** Returns the scale factor. */
    double getScale() const;

    void setMinZoom(const double minZoom);
    void setMaxZoom(const double maxZoom);

    // Angle

    void rotateBy(const ScreenCoordinate& first, const ScreenCoordinate& second, const Duration& = Duration::zero());
    /** Sets the angle of rotation.
        @param angle The new angle of rotation, measured in radians
            counterclockwise from true north. */
    void setAngle(double angle, const Duration& = Duration::zero());
    /** Sets the angle of rotation, keeping the given point fixed within the view.
        @param angle The new angle of rotation, measured in radians
            counterclockwise from true north.
        @param anchor A point relative to the top-left corner of the view. */
    void setAngle(double angle, optional<ScreenCoordinate> anchor, const Duration& = Duration::zero());
    /** Sets the angle of rotation, keeping the center point fixed within the inset view.
        @param angle The new angle of rotation, measured in radians
            counterclockwise from true north.
        @param padding The viewport padding that affects the fixed center point. */
    void setAngle(double angle, optional<EdgeInsets> padding, const Duration& = Duration::zero());
    /** Returns the angle of rotation.
        @return The angle of rotation, measured in radians counterclockwise from
            true north. */
    double getAngle() const;

    // Pitch
    /** Sets the pitch angle.
        @param angle The new pitch angle, measured in radians toward the
            horizon. */
    void setPitch(double pitch, const Duration& = Duration::zero());
    /** Sets the pitch angle, keeping the given point fixed within the view.
        @param angle The new pitch angle, measured in radians toward the
            horizon.
        @param anchor A point relative to the top-left corner of the view. */
    void setPitch(double pitch, optional<ScreenCoordinate> anchor, const Duration& = Duration::zero());
    double getPitch() const;

    // North Orientation
    void setNorthOrientation(NorthOrientation);
    NorthOrientation getNorthOrientation() const;

    // Constrain mode
    void setConstrainMode(ConstrainMode);
    ConstrainMode getConstrainMode() const;

    // Viewport mode
    void setViewportMode(ViewportMode);
    ViewportMode getViewportMode() const;

    // Transitions
    bool inTransition() const;
    Update updateTransitions(const TimePoint& now);
    TimePoint getTransitionStart() const { return transitionStart; }
    Duration getTransitionDuration() const { return transitionDuration; }
    void cancelTransitions();

    // Gesture
    void setGestureInProgress(bool);
    bool isGestureInProgress() const { return state.isGestureInProgress(); }

    // Transform state
    const TransformState& getState() const { return state; }
    bool isRotating() const { return state.isRotating(); }
    bool isScaling() const { return state.isScaling(); }
    bool isPanning() const { return state.isPanning(); }

    // Conversion and projection
    ScreenCoordinate latLngToScreenCoordinate(const LatLng&) const;
    LatLng screenCoordinateToLatLng(const ScreenCoordinate&) const;

private:
    std::function<void(MapChange)> callback;

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
