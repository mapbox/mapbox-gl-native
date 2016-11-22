#include <mbgl/map/camera.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/map/view.hpp>
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

Transform::Transform(std::function<void(MapChange)> callback_,
                     ConstrainMode constrainMode,
                     ViewportMode viewportMode)
    : callback(std::move(callback_)), state(constrainMode, viewportMode) {
}

#pragma mark - Map View

bool Transform::resize(const Size size) {
    if (state.size == size) {
        return false;
    }

    if (callback) {
        callback(MapChangeRegionWillChange);
    }

    state.size = size;
    state.constrain(state.scale, state.x, state.y);

    if (callback) {
        callback(MapChangeRegionDidChange);
    }

    return true;
}

#pragma mark - Camera

CameraOptions Transform::getCameraOptions(optional<EdgeInsets> padding) const {
    CameraOptions camera;
    camera.center = getLatLng(padding);
    camera.padding = padding;
    camera.zoom = getZoom();
    camera.angle = getAngle();
    camera.pitch = getPitch();
    return camera;
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
 * Change any combination of center, zoom, bearing, and pitch, with a smooth animation
 * between old and new values. The map will retain the current values for any options
 * not included in `options`.
 */
void Transform::easeTo(const CameraOptions& camera, const AnimationOptions& animation) {
    const LatLng unwrappedLatLng = camera.center.value_or(getLatLng());
    const LatLng latLng = unwrappedLatLng.wrapped();
    double zoom = camera.zoom.value_or(getZoom());
    double angle = camera.angle.value_or(getAngle());
    double pitch = camera.pitch.value_or(getPitch());

    if (!latLng || std::isnan(zoom)) {
        return;
    }

    // Determine endpoints.
    EdgeInsets padding;
    if (camera.padding) padding = *camera.padding;
    LatLng startLatLng = getLatLng(padding);
    // If gesture in progress, we transfer the world rounds from the end
    // longitude into start, so we can guarantee the "scroll effect" of rounding
    // the world while assuring the end longitude remains wrapped.
    if (isGestureInProgress()) startLatLng.longitude -= unwrappedLatLng.longitude - latLng.longitude;
    // Find the shortest path otherwise.
    else startLatLng.unwrapForShortestPath(latLng);

    const Point<double> startPoint = Projection::project(startLatLng, state.scale);
    const Point<double> endPoint = Projection::project(latLng, state.scale);

    ScreenCoordinate center = getScreenCoordinate(padding);
    center.y = state.size.height - center.y;

    // Constrain camera options.
    zoom = util::clamp(zoom, state.getMinZoom(), state.getMaxZoom());
    const double scale = state.zoomScale(zoom);
    pitch = util::clamp(pitch, 0., util::PITCH_MAX);

    Update update = state.getZoom() == zoom ? Update::Repaint : Update::RecalculateStyle;

    // Minimize rotation by taking the shorter path around the circle.
    angle = _normalizeAngle(angle, state.angle);
    state.angle = _normalizeAngle(state.angle, angle);

    Duration duration = animation.duration ? *animation.duration : Duration::zero();

    const double startScale = state.scale;
    const double startAngle = state.angle;
    const double startPitch = state.pitch;
    state.panning = latLng != startLatLng;
    state.scaling = scale != startScale;
    state.rotating = angle != startAngle;

    startTransition(camera, animation, [=](double t) {
        Point<double> framePoint = util::interpolate(startPoint, endPoint, t);
        LatLng frameLatLng = Projection::unproject(framePoint, startScale);
        double frameScale = util::interpolate(startScale, scale, t);
        state.setLatLngZoom(frameLatLng, state.scaleZoom(frameScale));

        if (angle != startAngle) {
            state.angle = util::wrap(util::interpolate(startAngle, angle, t), -M_PI, M_PI);
        }
        if (pitch != startPitch) {
            state.pitch = util::interpolate(startPitch, pitch, t);
        }

        if (padding) {
            state.moveLatLng(frameLatLng, center);
        }
        return update;
    }, duration);
}

/** This method implements an “optimal path” animation, as detailed in:

    Van Wijk, Jarke J.; Nuij, Wim A. A. “Smooth and efficient zooming and
        panning.” INFOVIS ’03. pp. 15–22.
        <https://www.win.tue.nl/~vanwijk/zoompan.pdf#page=5>.

    Where applicable, local variable documentation begins with the associated
    variable or function in van Wijk (2003). */
void Transform::flyTo(const CameraOptions &camera, const AnimationOptions &animation) {
    const LatLng latLng = camera.center.value_or(getLatLng()).wrapped();
    double zoom = camera.zoom.value_or(getZoom());
    double angle = camera.angle.value_or(getAngle());
    double pitch = camera.pitch.value_or(getPitch());

    if (!latLng || std::isnan(zoom)) {
        return;
    }

    // Determine endpoints.
    EdgeInsets padding;
    if (camera.padding) padding = *camera.padding;
    LatLng startLatLng = getLatLng(padding).wrapped();
    startLatLng.unwrapForShortestPath(latLng);

    const Point<double> startPoint = Projection::project(startLatLng, state.scale);
    const Point<double> endPoint = Projection::project(latLng, state.scale);

    ScreenCoordinate center = getScreenCoordinate(padding);
    center.y = state.size.height - center.y;

    // Constrain camera options.
    zoom = util::clamp(zoom, state.getMinZoom(), state.getMaxZoom());
    pitch = util::clamp(pitch, 0., util::PITCH_MAX);

    // Minimize rotation by taking the shorter path around the circle.
    angle = _normalizeAngle(angle, state.angle);
    state.angle = _normalizeAngle(state.angle, angle);

    const double startZoom = state.scaleZoom(state.scale);
    const double startAngle = state.angle;
    const double startPitch = state.pitch;

    /// w₀: Initial visible span, measured in pixels at the initial scale.
    /// Known henceforth as a <i>screenful</i>.
    double w0 = padding ? std::max(state.size.width, state.size.height)
                        : std::max(state.size.width - padding.left - padding.right,
                                   state.size.height - padding.top - padding.bottom);
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
        root mean squared average velocity, V<sub>RMS</sub>. A value of 1 would
        produce a circular motion. */
    double rho = 1.42;
    if (animation.minZoom) {
        double minZoom = util::min(*animation.minZoom, startZoom, zoom);
        minZoom = util::clamp(minZoom, state.getMinZoom(), state.getMaxZoom());
        /// w<sub>m</sub>: Maximum visible span, measured in pixels with respect
        /// to the initial scale.
        double wMax = w0 / state.zoomScale(minZoom - startZoom);
        rho = std::sqrt(wMax / u1 * 2);
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

    // When u₀ = u₁, the optimal path doesn’t require both ascent and descent.
    bool isClose = std::abs(u1) < 0.000001;
    // Perform a more or less instantaneous transition if the path is too short.
    if (isClose && std::abs(w0 - w1) < 0.000001) {
        easeTo(camera, animation);
        return;
    }

    /// r₀: Zoom-out factor during ascent.
    double r0 = r(0);
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
                : ((r(1) - r0) / rho));

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
        return;
    }

    const double startScale = state.scale;
    state.panning = true;
    state.scaling = true;
    state.rotating = angle != startAngle;

    startTransition(camera, animation, [=](double k) {
        /// s: The distance traveled along the flight path, measured in
        /// ρ-screenfuls.
        double s = k * S;
        double us = u(s);

        // Calculate the current point and zoom level along the flight path.
        Point<double> framePoint = util::interpolate(startPoint, endPoint, us);
        double frameZoom = startZoom + state.scaleZoom(1 / w(s));

        // Convert to geographic coordinates and set the new viewpoint.
        LatLng frameLatLng = Projection::unproject(framePoint, startScale);
        state.setLatLngZoom(frameLatLng, frameZoom);

        if (angle != startAngle) {
            state.angle = util::wrap(util::interpolate(startAngle, angle, k), -M_PI, M_PI);
        }
        if (pitch != startPitch) {
            state.pitch = util::interpolate(startPitch, pitch, k);
        }

        if (padding) {
            state.moveLatLng(frameLatLng, center);
        }
        return Update::RecalculateStyle;
    }, duration);
}

#pragma mark - Position

void Transform::moveBy(const ScreenCoordinate& offset, const Duration& duration) {
    ScreenCoordinate centerOffset = {
        offset.x,
        -offset.y,
    };
    ScreenCoordinate centerPoint = getScreenCoordinate() - centerOffset;

    CameraOptions camera;
    camera.center = state.screenCoordinateToLatLng(centerPoint);
    easeTo(camera, duration);
}

void Transform::setLatLng(const LatLng& latLng, const Duration& duration) {
    setLatLng(latLng, optional<ScreenCoordinate> {}, duration);
}

void Transform::setLatLng(const LatLng& latLng, optional<EdgeInsets> padding, const Duration& duration) {
    if (!latLng) return;
    CameraOptions camera;
    camera.center = latLng;
    camera.padding = padding;
    easeTo(camera, duration);
}

void Transform::setLatLng(const LatLng& latLng, optional<ScreenCoordinate> anchor, const Duration& duration) {
    if (!latLng) return;
    CameraOptions camera;
    camera.center = latLng;
    if (anchor) {
        EdgeInsets padding;
        padding.top = anchor->y;
        padding.left = anchor->x;
        padding.bottom = state.size.height - anchor->y;
        padding.right = state.size.width - anchor->x;
        if (padding) camera.padding = padding;
    }
    easeTo(camera, duration);
}

void Transform::setLatLngZoom(const LatLng& latLng, double zoom, const Duration& duration) {
    setLatLngZoom(latLng, zoom, EdgeInsets {}, duration);
}

void Transform::setLatLngZoom(const LatLng& latLng, double zoom, optional<EdgeInsets> padding, const Duration& duration) {
    if (!latLng || std::isnan(zoom)) return;

    CameraOptions camera;
    camera.center = latLng;
    camera.padding = padding;
    camera.zoom = zoom;
    easeTo(camera, duration);
}

LatLng Transform::getLatLng(optional<EdgeInsets> padding) const {
    if (padding && *padding) {
        return screenCoordinateToLatLng(padding->getCenter(state.size.width, state.size.height));
    } else {
        return state.getLatLng();
    }
}

ScreenCoordinate Transform::getScreenCoordinate(optional<EdgeInsets> padding) const {
    if (padding && *padding) {
        return padding->getCenter(state.size.width, state.size.height);
    } else {
        return { state.size.width / 2., state.size.height / 2. };
    }
}


#pragma mark - Zoom

void Transform::scaleBy(double ds, const Duration& duration) {
    scaleBy(ds, optional<ScreenCoordinate> {}, duration);
}

void Transform::scaleBy(double ds, optional<ScreenCoordinate> anchor, const Duration& duration) {
    if (std::isnan(ds)) return;
    double scale = util::clamp(state.scale * ds, state.min_scale, state.max_scale);
    setScale(scale, anchor, duration);
}

void Transform::setZoom(double zoom, const Duration& duration) {
    setZoom(zoom, optional<ScreenCoordinate> {}, duration);
}

void Transform::setZoom(double zoom, optional<ScreenCoordinate> anchor, const Duration& duration) {
    setScale(state.zoomScale(zoom), anchor, duration);
}

void Transform::setZoom(double zoom, optional<EdgeInsets> padding, const Duration& duration) {
    setScale(state.zoomScale(zoom), padding, duration);
}

double Transform::getZoom() const {
    return state.getZoom();
}

double Transform::getScale() const {
    return state.scale;
}

void Transform::setScale(double scale, const Duration& duration) {
    setScale(scale, optional<ScreenCoordinate> {}, duration);
}

void Transform::setScale(double scale, optional<ScreenCoordinate> anchor, const Duration& duration) {
    if (std::isnan(scale)) return;
    CameraOptions camera;
    camera.zoom = state.scaleZoom(scale);
    camera.anchor = anchor;
    easeTo(camera, duration);
}

void Transform::setScale(double scale, optional<EdgeInsets> padding, const Duration& duration) {
    optional<ScreenCoordinate> anchor;
    if (padding) anchor = getScreenCoordinate(padding);
    setScale(scale, anchor, duration);
}

void Transform::setMinZoom(const double minZoom) {
    if (std::isnan(minZoom)) return;
    state.setMinZoom(minZoom);
}

void Transform::setMaxZoom(const double maxZoom) {
    if (std::isnan(maxZoom)) return;
    state.setMaxZoom(maxZoom);
}

#pragma mark - Angle

void Transform::rotateBy(const ScreenCoordinate& first, const ScreenCoordinate& second, const Duration& duration) {
    ScreenCoordinate center = getScreenCoordinate();
    const ScreenCoordinate offset = first - center;
    const double distance = std::sqrt(std::pow(2, offset.x) + std::pow(2, offset.y));

    // If the first click was too close to the center, move the center of rotation by 200 pixels
    // in the direction of the click.
    if (distance < 200) {
        const double heightOffset = -200;
        const double rotateAngle = std::atan2(offset.y, offset.x);
        center.x = first.x + std::cos(rotateAngle) * heightOffset;
        center.y = first.y + std::sin(rotateAngle) * heightOffset;
    }

    CameraOptions camera;
    camera.angle = state.angle + util::angle_between(first - center, second - center);
    easeTo(camera, duration);
}

void Transform::setAngle(double angle, const Duration& duration) {
    setAngle(angle, optional<ScreenCoordinate> {}, duration);
}

void Transform::setAngle(double angle, optional<ScreenCoordinate> anchor, const Duration& duration) {
    if (std::isnan(angle)) return;
    CameraOptions camera;
    camera.angle = angle;
    camera.anchor = anchor;
    easeTo(camera, duration);
}

void Transform::setAngle(double angle, optional<EdgeInsets> padding, const Duration& duration) {
    optional<ScreenCoordinate> anchor;
    if (padding && *padding) anchor = getScreenCoordinate(padding);
    setAngle(angle, anchor, duration);
}

double Transform::getAngle() const {
    return state.angle;
}

#pragma mark - Pitch

void Transform::setPitch(double pitch, const Duration& duration) {
    setPitch(pitch, optional<ScreenCoordinate> {}, duration);
}

void Transform::setPitch(double pitch, optional<ScreenCoordinate> anchor, const Duration& duration) {
    if (std::isnan(pitch)) return;
    CameraOptions camera;
    camera.pitch = pitch;
    camera.anchor = anchor;
    easeTo(camera, duration);
}

double Transform::getPitch() const {
    return state.pitch;
}

#pragma mark - North Orientation

void Transform::setNorthOrientation(NorthOrientation orientation) {
    state.orientation = orientation;
    state.constrain(state.scale, state.x, state.y);
}

NorthOrientation Transform::getNorthOrientation() const {
    return state.getNorthOrientation();
}

#pragma mark - Constrain mode

void Transform::setConstrainMode(mbgl::ConstrainMode mode) {
    state.constrainMode = mode;
    state.constrain(state.scale, state.x, state.y);
}

ConstrainMode Transform::getConstrainMode() const {
    return state.getConstrainMode();
}

#pragma mark - Viewport mode

void Transform::setViewportMode(mbgl::ViewportMode mode) {
    state.viewportMode = mode;
}

ViewportMode Transform::getViewportMode() const {
    return state.getViewportMode();
}

#pragma mark - Transition

void Transform::startTransition(const CameraOptions& camera,
                                const AnimationOptions& animation,
                                std::function<Update(double)> frame,
                                const Duration& duration) {
    if (transitionFinishFn) {
        transitionFinishFn();
    }

    bool isAnimated = duration != Duration::zero();
    if (callback) {
        callback(isAnimated ? MapChangeRegionWillChangeAnimated : MapChangeRegionWillChange);
    }

    // Associate the anchor, if given, with a coordinate.
    optional<ScreenCoordinate> anchor = camera.anchor;
    LatLng anchorLatLng;
    if (anchor) {
        anchor->y = state.size.height - anchor->y;
        anchorLatLng = state.screenCoordinateToLatLng(*anchor);
    }

    transitionStart = Clock::now();
    transitionDuration = duration;

    transitionFrameFn = [isAnimated, animation, frame, anchor, anchorLatLng, this](const TimePoint now) {
        float t = isAnimated ? (std::chrono::duration<float>(now - transitionStart) / transitionDuration) : 1.0;
        Update result;
        if (t >= 1.0) {
            result = frame(1.0);
        } else {
            util::UnitBezier ease = animation.easing ? *animation.easing : util::DEFAULT_TRANSITION_EASE;
            result = frame(ease.solve(t, 0.001));
        }

        if (anchor) state.moveLatLng(anchorLatLng, *anchor);

        // At t = 1.0, a DidChangeAnimated notification should be sent from finish().
        if (t < 1.0) {
            if (animation.transitionFrameFn) {
                animation.transitionFrameFn(t);
            }
            if (callback) {
                callback(MapChangeRegionIsChanging);
            }
        } else {
            transitionFinishFn();
            transitionFinishFn = nullptr;

            // This callback gets destroyed here,
            // we can only return after this point.
            transitionFrameFn = nullptr;
        }
        return result;
    };

    transitionFinishFn = [isAnimated, animation, this] {
        state.panning = false;
        state.scaling = false;
        state.rotating = false;
        if (animation.transitionFinishFn) {
            animation.transitionFinishFn();
        }
        if (callback) {
            callback(isAnimated ? MapChangeRegionDidChangeAnimated : MapChangeRegionDidChange);
        }
    };

    if (!isAnimated) {
        transitionFrameFn(Clock::now());
    }
}

bool Transform::inTransition() const {
    return transitionFrameFn != nullptr;
}

Update Transform::updateTransitions(const TimePoint& now) {
    return transitionFrameFn ? transitionFrameFn(now) : Update::Nothing;
}

void Transform::cancelTransitions() {
    if (transitionFinishFn) {
        transitionFinishFn();
    }

    transitionFrameFn = nullptr;
    transitionFinishFn = nullptr;
}

void Transform::setGestureInProgress(bool inProgress) {
    state.gestureInProgress = inProgress;
}

#pragma mark Conversion and projection

ScreenCoordinate Transform::latLngToScreenCoordinate(const LatLng& latLng) const {
    // If the center and point longitudes are not in the same side of the
    // antimeridian, we unwrap the point longitude so it would be seen if
    // e.g. the next antimeridian side is visible.
    LatLng unwrappedLatLng = latLng.wrapped();
    unwrappedLatLng.unwrapForShortestPath(getLatLng());
    ScreenCoordinate point = state.latLngToScreenCoordinate(unwrappedLatLng);
    point.y = state.size.height - point.y;
    return point;
}

LatLng Transform::screenCoordinateToLatLng(const ScreenCoordinate& point) const {
    ScreenCoordinate flippedPoint = point;
    flippedPoint.y = state.size.height - flippedPoint.y;
    return state.screenCoordinateToLatLng(flippedPoint).wrapped();
}

} // namespace mbgl
