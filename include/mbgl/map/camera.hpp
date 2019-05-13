#pragma once

#include <mbgl/util/geo.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/unitbezier.hpp>
#include <mbgl/util/optional.hpp>

#include <functional>

namespace mbgl {

/** Various options for describing the viewpoint of a map. All fields are
    optional.
    Anchor and center points are mutually exclusive, with preference for the
    center point when both are set.
    */
struct CameraOptions {
    CameraOptions& withCenter(const optional<LatLng>& o) { center = o; return *this; }
    CameraOptions& withPadding(const optional<EdgeInsets>& p) { padding = p; return *this; }
    CameraOptions& withAnchor(const optional<ScreenCoordinate>& o) { anchor = o; return *this; }
    CameraOptions& withZoom(const optional<double>& o) { zoom = o; return *this; }
    CameraOptions& withBearing(const optional<double>& o) { bearing = o; return *this; }
    CameraOptions& withPitch(const optional<double>& o) { pitch = o; return *this; }

    /** Coordinate at the center of the map. */
    optional<LatLng> center;

    /** Padding around the interior of the view that affects the frame of
        reference for `center`. */
    optional<EdgeInsets> padding;

    /** Point of reference for `zoom` and `angle`, assuming an origin at the
        top-left corner of the view. */
    optional<ScreenCoordinate> anchor;

    /** Zero-based zoom level. Constrained to the minimum and maximum zoom
        levels. */
    optional<double> zoom;

    /** Bearing, measured in degrees from true north. Wrapped to [0, 360). */
    optional<double> bearing;

    /** Pitch toward the horizon measured in degrees , with 0 deg resulting in a
        two-dimensional map. */
    optional<double> pitch;
};

constexpr bool operator==(const CameraOptions& a, const CameraOptions& b) {
    return a.center == b.center
        && a.padding == b.padding
        && a.anchor == b.anchor
        && a.zoom == b.zoom
        && a.bearing == b.bearing
        && a.pitch == b.pitch;
}

constexpr bool operator!=(const CameraOptions& a, const CameraOptions& b) {
    return !(a == b);
}

/** Various options for describing a transition between viewpoints with
    animation. All fields are optional; the default values depend on how this
    struct is used. */
struct AnimationOptions {
    /** Time to animate to the viewpoint defined herein. */
    optional<Duration> duration;

    /** Average velocity of a flyTo() transition, measured in screenfuls per
        second, assuming a linear timing curve.

        A <i>screenful</i> is the visible span in pixels. It does not correspond
        to a fixed physical distance but rather varies by zoom level. */
    optional<double> velocity;

    /** Zero-based zoom level at the peak of the flyTo() transition’s flight
        path. */
    optional<double> minZoom;

    /** The easing timing curve of the transition. */
    optional<mbgl::util::UnitBezier> easing;

    /** A function that is called on each frame of the transition, just before a
        screen update, except on the last frame. The first parameter indicates
        the elapsed time as a percentage of the duration. */
    std::function<void(double)> transitionFrameFn;

    /** A function that is called once on the last frame of the transition, just
        before the corresponding screen update. */
    std::function<void()> transitionFinishFn;

    /** Creates an animation with no options specified. */
    AnimationOptions() {}

    /** Creates an animation with the specified duration. */
    AnimationOptions(Duration d)
        : duration(d) {}
};

} // namespace mbgl
