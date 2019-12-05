#include <mbgl/map/camera.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/unitbezier.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>

#include <cstdio>
#include <cmath>

namespace mbgl {

/** Converts the given angle (in radians) to be numerically close to the anchor angle, allowing it to be interpolated properly without sudden jumps. */
static double _normalizeAngle(double angle, double anchorAngle)
{
    if (std::isnan(angle) || std::isnan(anchorAngle)) {
        return 0;
    }

    angle = util::wrap(angle, -M_PI, M_PI);
    if (angle == -M_PI) angle = M_PI;
    double diff = std::abs(angle - anchorAngle);
    if (std::abs(angle - util::M2PI - anchorAngle) < diff) {
        angle -= util::M2PI;
    }
    if (std::abs(angle + util::M2PI - anchorAngle) < diff) {
        angle += util::M2PI;
    }

    return angle;
}

Transform::Transform(MapObserver& observer_,
                     ConstrainMode constrainMode,
                     ViewportMode viewportMode)
    : observer(observer_), state(constrainMode, viewportMode) {
}

#pragma mark - Map View

void Transform::resize(const Size size) {
    if (size.isEmpty()) {
        throw std::runtime_error("failed to resize: size is empty");
    }

    if (state.getSize() == size) {
        return;
    }

    observer.onCameraWillChange(MapObserver::CameraChangeMode::Immediate);
    state.setSize(size);
    double scale{state.getScale()};
    double x{state.getX()};
    double y{state.getY()};
    state.constrain(scale, x, y);
    state.setProperties(TransformStateProperties().withScale(scale).withX(x).withY(y));

    observer.onCameraDidChange(MapObserver::CameraChangeMode::Immediate);
}

#pragma mark - Camera

CameraOptions Transform::getCameraOptions(optional<EdgeInsets> padding) const {
    return state.getCameraOptions(padding);
}

/**
 * Change any combination of center, zoom, bearing, and pitch, without
 * a transition. The map will retain the current values for any options
 * not included in `options`.
 */
void Transform::jumpTo(const CameraOptions& camera) {
    easeTo(camera);
}

/**
 * Change any combination of center, zoom, bearing, pitch and edgeInsets, with a
 * smooth animation between old and new values. The map will retain the current
 * values for any options not included in `options`.
 */
void Transform::easeTo(const CameraOptions& camera, const AnimationOptions& animation) {
    Duration duration = animation.duration.value_or(Duration::zero());
    if (state.getLatLngBounds() == LatLngBounds() && !isGestureInProgress() && duration != Duration::zero()) {
        // reuse flyTo, without exaggerated animation, to achieve constant ground speed.
        return flyTo(camera, animation, true);
    }
    const EdgeInsets& padding = camera.padding.value_or(state.getEdgeInsets());
    LatLng startLatLng = getLatLng(LatLng::Unwrapped);
    const LatLng& unwrappedLatLng = camera.center.value_or(startLatLng);
    const LatLng& latLng = state.getLatLngBounds() != LatLngBounds() ? unwrappedLatLng : unwrappedLatLng.wrapped();
    double zoom = camera.zoom.value_or(getZoom());
    double bearing = camera.bearing ? -*camera.bearing * util::DEG2RAD : getBearing();
    double pitch = camera.pitch ? *camera.pitch * util::DEG2RAD : getPitch();

    if (std::isnan(zoom) || std::isnan(bearing) || std::isnan(pitch)) {
        if (animation.transitionFinishFn) {
            animation.transitionFinishFn();
        }
        return;
    }

    if (state.getLatLngBounds() == LatLngBounds()) {
        if (isGestureInProgress()) {
            // If gesture in progress, we transfer the wrap rounds from the end longitude into
            // start, so the "scroll effect" of rounding the world is the same while assuring the
            // end longitude remains wrapped.
            const double wrap = unwrappedLatLng.longitude() - latLng.longitude();
            startLatLng = LatLng(startLatLng.latitude(), startLatLng.longitude() - wrap);
        } else {
            // Find the shortest path otherwise.
            startLatLng.unwrapForShortestPath(latLng);
        }
    }

    const Point<double> startPoint = Projection::project(startLatLng, state.getScale());
    const Point<double> endPoint = Projection::project(latLng, state.getScale());

    // Constrain camera options.
    zoom = util::clamp(zoom, state.getMinZoom(), state.getMaxZoom());
    pitch = util::clamp(pitch, util::PITCH_MIN, util::PITCH_MAX);

    // Minimize rotation by taking the shorter path around the circle.
    bearing = _normalizeAngle(bearing, state.getBearing());
    state.setBearing(_normalizeAngle(state.getBearing(), bearing));

    const double startZoom = state.getZoom();
    const double startBearing = state.getBearing();
    const double startPitch = state.getPitch();
    state.setProperties(TransformStateProperties()
                            .withPanningInProgress(unwrappedLatLng != startLatLng)
                            .withScalingInProgress(zoom != startZoom)
                            .withRotatingInProgress(bearing != startBearing));
    const EdgeInsets startEdgeInsets = state.getEdgeInsets();

    startTransition(
        camera,
        animation,
        [=](double t) {
            Point<double> framePoint = util::interpolate(startPoint, endPoint, t);
            LatLng frameLatLng = Projection::unproject(framePoint, state.zoomScale(startZoom));
            double frameZoom = util::interpolate(startZoom, zoom, t);
            state.setLatLngZoom(frameLatLng, frameZoom);
            if (bearing != startBearing) {
                state.setBearing(util::wrap(util::interpolate(startBearing, bearing, t), -M_PI, M_PI));
            }
            if (padding != startEdgeInsets) {
                // Interpolate edge insets
                EdgeInsets edgeInsets;
                state.setEdgeInsets({util::interpolate(startEdgeInsets.top(), padding.top(), t),
                                     util::interpolate(startEdgeInsets.left(), padding.left(), t),
                                     util::interpolate(startEdgeInsets.bottom(), padding.bottom(), t),
                                     util::interpolate(startEdgeInsets.right(), padding.right(), t)});
            }
            double maxPitch = getMaxPitchForEdgeInsets(state.getEdgeInsets());
            if (pitch != startPitch || maxPitch < startPitch) {
                state.setPitch(std::min(maxPitch, util::interpolate(startPitch, pitch, t)));
            }
        },
        duration);
}

/** This method implements an “optimal path” animation, as detailed in:

    Van Wijk, Jarke J.; Nuij, Wim A. A. “Smooth and efficient zooming and
        panning.” INFOVIS ’03. pp. 15–22.
        <https://www.win.tue.nl/~vanwijk/zoompan.pdf#page=5>.

    Where applicable, local variable documentation begins with the associated
    variable or function in van Wijk (2003). */
void Transform::flyTo(const CameraOptions& camera, const AnimationOptions& animation, bool linearZoomInterpolation) {
    const EdgeInsets& padding = camera.padding.value_or(state.getEdgeInsets());
    const LatLng& latLng = camera.center.value_or(getLatLng(LatLng::Unwrapped)).wrapped();
    double zoom = camera.zoom.value_or(getZoom());
    double bearing = camera.bearing ? -*camera.bearing * util::DEG2RAD : getBearing();
    double pitch = camera.pitch ? *camera.pitch * util::DEG2RAD : getPitch();

    if (std::isnan(zoom) || std::isnan(bearing) || std::isnan(pitch) || state.getSize().isEmpty()) {
        if (animation.transitionFinishFn) {
            animation.transitionFinishFn();
        }
        return;
    }

    // Determine endpoints.
    LatLng startLatLng = getLatLng(LatLng::Unwrapped).wrapped();
    startLatLng.unwrapForShortestPath(latLng);

    const Point<double> startPoint = Projection::project(startLatLng, state.getScale());
    const Point<double> endPoint = Projection::project(latLng, state.getScale());

    // Constrain camera options.
    zoom = util::clamp(zoom, state.getMinZoom(), state.getMaxZoom());
    pitch = util::clamp(pitch, util::PITCH_MIN, util::PITCH_MAX);

    // Minimize rotation by taking the shorter path around the circle.
    bearing = _normalizeAngle(bearing, state.getBearing());
    state.setBearing(_normalizeAngle(state.getBearing(), bearing));
    const double startZoom = state.scaleZoom(state.getScale());
    const double startBearing = state.getBearing();
    const double startPitch = state.getPitch();

    /// w₀: Initial visible span, measured in pixels at the initial scale.
    /// Known henceforth as a <i>screenful</i>.

    double w0 = std::max(state.getSize().width - padding.left() - padding.right(),
                         state.getSize().height - padding.top() - padding.bottom());
    /// w₁: Final visible span, measured in pixels with respect to the initial
    /// scale.
    double w1 = w0 / state.zoomScale(zoom - startZoom);
    /// Length of the flight path as projected onto the ground plane, measured
    /// in pixels from the world image origin at the initial scale.
    double u1 = ::hypot((endPoint - startPoint).x, (endPoint - startPoint).y);

    /** ρ: The relative amount of zooming that takes place along the flight
        path. A high value maximizes zooming for an exaggerated animation, while
        a low value minimizes zooming for something closer to easeTo().

        1.42 is the average value selected by participants in the user study in
        van Wijk (2003). A value of 6<sup>¼</sup> would be equivalent to the
        root mean squared average velocity, V<sub>RMS</sub>. A value of 1
        produces a circular motion. */
    double rho = 1.42;
    if (animation.minZoom || linearZoomInterpolation) {
        double minZoom = util::min(animation.minZoom.value_or(startZoom), startZoom, zoom);
        minZoom = util::clamp(minZoom, state.getMinZoom(), state.getMaxZoom());
        /// w<sub>m</sub>: Maximum visible span, measured in pixels with respect
        /// to the initial scale.
        double wMax = w0 / state.zoomScale(minZoom - startZoom);
        rho = u1 != 0 ? std::sqrt(wMax / u1 * 2) : 1.0;
    }
    /// ρ²
    double rho2 = rho * rho;

    /** rᵢ: Returns the zoom-out factor at one end of the animation.

        @param i 0 for the ascent or 1 for the descent. */
    auto r = [=](double i) {
        /// bᵢ
        double b = (w1 * w1 - w0 * w0 + (i ? -1 : 1) * rho2 * rho2 * u1 * u1) / (2 * (i ? w1 : w0) * rho2 * u1);
        return std::log(std::sqrt(b * b + 1) - b);
    };

    /// r₀: Zoom-out factor during ascent.
    double r0 = u1 != 0 ? r(0) : INFINITY; // Silence division by 0 on sanitize bot.
    double r1 = u1 != 0 ? r(1) : INFINITY;

    // When u₀ = u₁, the optimal path doesn’t require both ascent and descent.
    bool isClose = std::abs(u1) < 0.000001 || !std::isfinite(r0) || !std::isfinite(r1);

    /** w(s): Returns the visible span on the ground, measured in pixels with
        respect to the initial scale.

        Assumes an angular field of view of 2 arctan ½ ≈ 53°. */
    auto w = [=](double s) {
        return (isClose ? std::exp((w1 < w0 ? -1 : 1) * rho * s)
                : (std::cosh(r0) / std::cosh(r0 + rho * s)));
    };
    /// u(s): Returns the distance along the flight path as projected onto the
    /// ground plane, measured in pixels from the world image origin at the
    /// initial scale.
    auto u = [=](double s) {
        return (isClose ? 0.
                : (w0 * (std::cosh(r0) * std::tanh(r0 + rho * s) - std::sinh(r0)) / rho2 / u1));
    };
    /// S: Total length of the flight path, measured in ρ-screenfuls.
    double S = (isClose ? (std::abs(std::log(w1 / w0)) / rho)
                : ((r1 - r0) / rho));

    Duration duration;
    if (animation.duration) {
        duration = *animation.duration;
    } else {
        /// V: Average velocity, measured in ρ-screenfuls per second.
        double velocity = 1.2;
        if (animation.velocity) {
            velocity = *animation.velocity / rho;
        }
        duration = std::chrono::duration_cast<Duration>(std::chrono::duration<double>(S / velocity));
    }
    if (duration == Duration::zero()) {
        // Perform an instantaneous transition.
        jumpTo(camera);
        if (animation.transitionFinishFn) {
            animation.transitionFinishFn();
        }
        return;
    }

    const double startScale = state.getScale();
    state.setProperties(
        TransformStateProperties().withPanningInProgress(true).withScalingInProgress(true).withRotatingInProgress(
            bearing != startBearing));
    const EdgeInsets startEdgeInsets = state.getEdgeInsets();

    startTransition(
        camera,
        animation,
        [=](double k) {
            /// s: The distance traveled along the flight path, measured in
            /// ρ-screenfuls.
            double s = k * S;
            double us = k == 1.0 ? 1.0 : u(s);

            // Calculate the current point and zoom level along the flight path.
            Point<double> framePoint = util::interpolate(startPoint, endPoint, us);
            double frameZoom =
                linearZoomInterpolation ? util::interpolate(startZoom, zoom, k) : startZoom + state.scaleZoom(1 / w(s));

            // Zoom can be NaN if size is empty.
            if (std::isnan(frameZoom)) {
                frameZoom = zoom;
            }

            // Convert to geographic coordinates and set the new viewpoint.
            LatLng frameLatLng = Projection::unproject(framePoint, startScale);
            state.setLatLngZoom(frameLatLng, frameZoom);
            if (bearing != startBearing) {
                state.setBearing(util::wrap(util::interpolate(startBearing, bearing, k), -M_PI, M_PI));
            }

            if (padding != startEdgeInsets) {
                // Interpolate edge insets
                state.setEdgeInsets({util::interpolate(startEdgeInsets.top(), padding.top(), k),
                                     util::interpolate(startEdgeInsets.left(), padding.left(), k),
                                     util::interpolate(startEdgeInsets.bottom(), padding.bottom(), k),
                                     util::interpolate(startEdgeInsets.right(), padding.right(), k)});
            }
            double maxPitch = getMaxPitchForEdgeInsets(state.getEdgeInsets());

            if (pitch != startPitch || maxPitch < startPitch) {
                state.setPitch(std::min(maxPitch, util::interpolate(startPitch, pitch, k)));
            }
        },
        duration);
}

#pragma mark - Position

void Transform::moveBy(const ScreenCoordinate& offset, const AnimationOptions& animation) {
    ScreenCoordinate centerOffset = {offset.x, offset.y};
    ScreenCoordinate pointOnScreen =
        state.getEdgeInsets().getCenter(state.getSize().width, state.getSize().height) - centerOffset;
    // Use unwrapped LatLng to carry information about moveBy direction.
    easeTo(CameraOptions().withCenter(screenCoordinateToLatLng(pointOnScreen, LatLng::Unwrapped)), animation);
}

LatLng Transform::getLatLng(LatLng::WrapMode wrap) const {
    return state.getLatLng(wrap);
}

#pragma mark - Zoom

double Transform::getZoom() const {
    return state.getZoom();
}

#pragma mark - Bounds

void Transform::setLatLngBounds(LatLngBounds bounds) {
    if (!bounds.valid()) {
        throw std::runtime_error("failed to set bounds: bounds are invalid");
    }
    state.setLatLngBounds(bounds);
}

void Transform::setMinZoom(const double minZoom) {
    if (std::isnan(minZoom)) return;
    state.setMinZoom(minZoom);
}

void Transform::setMaxZoom(const double maxZoom) {
    if (std::isnan(maxZoom)) return;
    state.setMaxZoom(maxZoom);
}

#pragma mark - Bearing

void Transform::rotateBy(const ScreenCoordinate& first,
                         const ScreenCoordinate& second,
                         const AnimationOptions& animation) {
    ScreenCoordinate center = state.getEdgeInsets().getCenter(state.getSize().width, state.getSize().height);
    const ScreenCoordinate offset = first - center;
    const double distance = std::sqrt(std::pow(2, offset.x) + std::pow(2, offset.y));

    // If the first click was too close to the center, move the center of rotation by 200 pixels
    // in the direction of the click.
    if (distance < 200) {
        const double heightOffset = -200;
        const double rotateBearing = std::atan2(offset.y, offset.x);
        center.x = first.x + std::cos(rotateBearing) * heightOffset;
        center.y = first.y + std::sin(rotateBearing) * heightOffset;
    }

    const double bearing = -(state.getBearing() + util::angle_between(first - center, second - center)) * util::RAD2DEG;
    easeTo(CameraOptions().withBearing(bearing), animation);
}

double Transform::getBearing() const {
    return state.getBearing();
}

#pragma mark - Pitch

double Transform::getPitch() const {
    return state.getPitch();
}

#pragma mark - North Orientation

void Transform::setNorthOrientation(NorthOrientation orientation) {
    state.setNorthOrientation(orientation);
    double scale{state.getScale()};
    double x{state.getX()};
    double y{state.getY()};
    state.constrain(scale, x, y);
    state.setProperties(TransformStateProperties().withScale(scale).withX(x).withY(y));
}

NorthOrientation Transform::getNorthOrientation() const {
    return state.getNorthOrientation();
}

#pragma mark - Constrain mode

void Transform::setConstrainMode(mbgl::ConstrainMode mode) {
    state.setConstrainMode(mode);
    double scale{state.getScale()};
    double x{state.getX()};
    double y{state.getY()};
    state.constrain(scale, x, y);
    state.setProperties(TransformStateProperties().withScale(scale).withX(x).withY(y));
}

ConstrainMode Transform::getConstrainMode() const {
    return state.getConstrainMode();
}

#pragma mark - Viewport mode

void Transform::setViewportMode(mbgl::ViewportMode mode) {
    state.setViewportMode(mode);
}

ViewportMode Transform::getViewportMode() const {
    return state.getViewportMode();
}

#pragma mark - Projection mode

void Transform::setProjectionMode(const ProjectionMode& options) {
    state.setProperties(TransformStateProperties()
                            .withAxonometric(options.axonometric.value_or(state.getAxonometric()))
                            .withXSkew(options.xSkew.value_or(state.getXSkew()))
                            .withYSkew(options.ySkew.value_or(state.getYSkew())));
}

ProjectionMode Transform::getProjectionMode() const {
    return ProjectionMode()
        .withAxonometric(state.getAxonometric())
        .withXSkew(state.getXSkew())
        .withYSkew(state.getYSkew());
}

#pragma mark - Transition

void Transform::startTransition(const CameraOptions& camera,
                                const AnimationOptions& animation,
                                std::function<void(double)> frame,
                                const Duration& duration) {
    if (transitionFinishFn) {
        transitionFinishFn();
    }

    bool isAnimated = duration != Duration::zero();
    observer.onCameraWillChange(isAnimated ? MapObserver::CameraChangeMode::Animated
                                           : MapObserver::CameraChangeMode::Immediate);

    // Associate the anchor, if given, with a coordinate.
    // Anchor and center points are mutually exclusive, with preference for the
    // center point when both are set.
    optional<ScreenCoordinate> anchor = camera.center ? nullopt : camera.anchor;
    LatLng anchorLatLng;
    if (anchor) {
        anchor->y = state.getSize().height - anchor->y;
        anchorLatLng = state.screenCoordinateToLatLng(*anchor);
    }

    transitionStart = Clock::now();
    transitionDuration = duration;

    transitionFrameFn = [isAnimated, animation, frame, anchor, anchorLatLng, this](const TimePoint now) {
        float t = isAnimated ? (std::chrono::duration<float>(now - transitionStart) / transitionDuration) : 1.0;
        if (t >= 1.0) {
            frame(1.0);
        } else {
            util::UnitBezier ease = animation.easing ? *animation.easing : util::DEFAULT_TRANSITION_EASE;
            frame(ease.solve(t, 0.001));
        }

        if (anchor) state.moveLatLng(anchorLatLng, *anchor);

        // At t = 1.0, a DidChangeAnimated notification should be sent from finish().
        if (t < 1.0) {
            if (animation.transitionFrameFn) {
                animation.transitionFrameFn(t);
            }
            observer.onCameraIsChanging();
            return false;
        } else {
            // Indicate that we need to terminate this transition
            return true;
        }
    };

    transitionFinishFn = [isAnimated, animation, this] {
        state.setProperties(
            TransformStateProperties().withPanningInProgress(false).withScalingInProgress(false).withRotatingInProgress(
                false));
        if (animation.transitionFinishFn) {
            animation.transitionFinishFn();
        }
        observer.onCameraDidChange(isAnimated ? MapObserver::CameraChangeMode::Animated
                                              : MapObserver::CameraChangeMode::Immediate);
    };

    if (!isAnimated) {
        auto update = std::move(transitionFrameFn);
        auto finish = std::move(transitionFinishFn);

        transitionFrameFn = nullptr;
        transitionFinishFn = nullptr;

        update(Clock::now());
        finish();
    }
}

bool Transform::inTransition() const {
    return transitionFrameFn != nullptr;
}

void Transform::updateTransitions(const TimePoint& now) {

    // Use a temporary function to ensure that the transitionFrameFn lambda is
    // called only once per update.

    // This addresses the symptoms of https://github.com/mapbox/mapbox-gl-native/issues/11180
    // where setting a shape source to nil (or similar) in the `onCameraIsChanging`
    // observer function causes `Map::Impl::onUpdate()` to be called which
    // in turn calls this function (before the current iteration has completed),
    // leading to an infinite loop. See https://github.com/mapbox/mapbox-gl-native/issues/5833
    // for a similar, related, issue.
    //
    // By temporarily nulling the `transitionFrameFn` (and then restoring it
    // after the temporary has been called) we stop this recursion.
    //
    // It's important to note that the scope of this change is stop the above
    // crashes. It doesn't address any potential deeper issue (for example
    // user error, how often and when transition callbacks are called).

    auto transition = std::move(transitionFrameFn);
    transitionFrameFn = nullptr;

    if (transition && transition(now)) {
        // If the transition indicates that it is complete, then we should call
        // the finish lambda (going via a temporary as above)
        auto finish = std::move(transitionFinishFn);

        transitionFinishFn = nullptr;
        transitionFrameFn = nullptr;

        if (finish) {
            finish();
        }
    } else if (!transitionFrameFn) {
        // We have to check `transitionFrameFn` is nil here, since a new transition
        // may have been triggered in a user callback (from the transition call
        // above)
        transitionFrameFn = std::move(transition);
    }
}

void Transform::cancelTransitions() {
    if (transitionFinishFn) {
        transitionFinishFn();
    }

    transitionFrameFn = nullptr;
    transitionFinishFn = nullptr;
}

void Transform::setGestureInProgress(bool inProgress) {
    state.setGestureInProgress(inProgress);
}

#pragma mark Conversion and projection

ScreenCoordinate Transform::latLngToScreenCoordinate(const LatLng& latLng) const {
    ScreenCoordinate point = state.latLngToScreenCoordinate(latLng);
    point.y = state.getSize().height - point.y;
    return point;
}

LatLng Transform::screenCoordinateToLatLng(const ScreenCoordinate& point, LatLng::WrapMode wrapMode) const {
    ScreenCoordinate flippedPoint = point;
    flippedPoint.y = state.getSize().height - flippedPoint.y;
    return state.screenCoordinateToLatLng(flippedPoint, wrapMode);
}

double Transform::getMaxPitchForEdgeInsets(const EdgeInsets& insets) const {
    double centerOffsetY = 0.5 * (insets.top() - insets.bottom()); // See TransformState::getCenterOffset.

    const auto height = state.getSize().height;
    assert(height);
    // For details, see description at https://github.com/mapbox/mapbox-gl-native/pull/15195
    // The definition of half of TransformState::fov with no inset, is: fov = arctan((height / 2) / (height * 1.5)).
    // We use half of fov, as it is field of view above perspective center.
    // With inset, this angle changes and tangentOfFovAboveCenterAngle = (h/2 + centerOffsetY) / (height * 1.5).
    // 1.03 is a bit extra added to prevent parallel ground to viewport clipping plane.
    const double tangentOfFovAboveCenterAngle = 1.03 * (height / 2.0 + centerOffsetY) / (1.5 * height);
    const double fovAboveCenter = std::atan(tangentOfFovAboveCenterAngle);
    return M_PI * 0.5 - fovAboveCenter;
    // e.g. Maximum pitch of 60 degrees is when perspective center's offset from the top is 84% of screen height.
}

} // namespace mbgl
