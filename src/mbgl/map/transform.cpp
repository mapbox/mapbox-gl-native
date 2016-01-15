#include <mbgl/map/camera.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/unitbezier.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>

#include <cstdio>
#include <cmath>

using namespace mbgl;

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

inline bool _validPoint(const PrecisionPoint& point) {
    return !std::isnan(point.x) && !std::isnan(point.y);
}

Transform::Transform(View &view_, ConstrainMode constrainMode)
    : view(view_)
    , state(constrainMode)
{
}

#pragma mark - Map View

bool Transform::resize(const std::array<uint16_t, 2> size) {
    if (state.width != size[0] || state.height != size[1]) {

        view.notifyMapChange(MapChangeRegionWillChange);

        state.width = size[0];
        state.height = size[1];
        state.constrain(state.scale, state.x, state.y);

        view.notifyMapChange(MapChangeRegionDidChange);

        return true;
    } else {
        return false;
    }
}

#pragma mark - Camera

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
    LatLng latLng = camera.center ? *camera.center : getLatLng();
    double zoom = camera.zoom ? *camera.zoom : getZoom();
    double angle = camera.angle ? *camera.angle : getAngle();
    double pitch = camera.pitch ? *camera.pitch : getPitch();
    
    if (!latLng || std::isnan(zoom)) {
        return;
    }
    
    // Determine endpoints.
    EdgeInsets padding;
    if (camera.padding) {
        padding = *camera.padding;
    }
    const LatLng startLatLng = getLatLng(padding);
    padding.flip();
    const PrecisionPoint startPoint = {
        state.lngX(startLatLng.longitude),
        state.latY(startLatLng.latitude),
    };
    unwrapLatLng(latLng);
    const PrecisionPoint endPoint = {
        state.lngX(latLng.longitude),
        state.latY(latLng.latitude),
    };
    
    // Constrain camera options.
    zoom = util::clamp(zoom, state.getMinZoom(), state.getMaxZoom());
    const double scale = state.zoomScale(zoom);
    pitch = util::clamp(pitch, 0., util::PITCH_MAX);
    
    Update update = state.getZoom() == zoom ? Update::Repaint : Update::Zoom;
    
    // Minimize rotation by taking the shorter path around the circle.
    angle = _normalizeAngle(angle, state.angle);
    state.angle = _normalizeAngle(state.angle, angle);

    Duration duration = animation.duration ? *animation.duration : Duration::zero();
    
    const double startWorldSize = state.worldSize();
    state.Bc = startWorldSize / 360;
    state.Cc = startWorldSize / util::M2PI;
    
    const double startScale = state.scale;
    const double startAngle = state.angle;
    const double startPitch = state.pitch;
    state.panning = latLng != startLatLng;
    state.scaling = scale != startScale;
    state.rotating = angle != startAngle;

    startTransition(camera, animation, [=](double t) {
        PrecisionPoint framePoint = util::interpolate(startPoint, endPoint, t);
        LatLng frameLatLng = {
            state.yLat(framePoint.y, startWorldSize),
            state.xLng(framePoint.x, startWorldSize),
        };
        double frameScale = util::interpolate(startScale, scale, t);
        state.setLatLngZoom(frameLatLng, state.scaleZoom(frameScale));
        
        if (angle != startAngle) {
            state.angle = util::wrap(util::interpolate(startAngle, angle, t), -M_PI, M_PI);
        }
        if (pitch != startPitch) {
            state.pitch = util::interpolate(startPitch, pitch, t);
        }
        
        if (padding) {
            state.moveLatLng(frameLatLng, padding.getCenter(state.width, state.height));
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
    LatLng latLng = camera.center ? *camera.center : getLatLng();
    double zoom = camera.zoom ? *camera.zoom : getZoom();
    double angle = camera.angle ? *camera.angle : getAngle();
    double pitch = camera.pitch ? *camera.pitch : getPitch();

    if (!latLng || std::isnan(zoom)) {
        return;
    }
    
    // Determine endpoints.
    EdgeInsets padding;
    if (camera.padding) {
        padding = *camera.padding;
    }
    const LatLng startLatLng = getLatLng(padding);
    padding.flip();
    const PrecisionPoint startPoint = {
        state.lngX(startLatLng.longitude),
        state.latY(startLatLng.latitude),
    };
    unwrapLatLng(latLng);
    const PrecisionPoint endPoint = {
        state.lngX(latLng.longitude),
        state.latY(latLng.latitude),
    };
    
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
    double w0 = std::max(state.width - padding.left - padding.right,
                         state.height - padding.top - padding.bottom);
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
        duration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double, std::chrono::seconds::period>(S / velocity));
    }
    if (duration == Duration::zero()) {
        // Perform an instantaneous transition.
        jumpTo(camera);
        return;
    }
    
    const double startWorldSize = state.worldSize();
    state.Bc = startWorldSize / 360;
    state.Cc = startWorldSize / util::M2PI;
    
    state.panning = true;
    state.scaling = true;
    state.rotating = angle != startAngle;
    
    startTransition(camera, animation, [=](double k) {
        /// s: The distance traveled along the flight path, measured in
        /// ρ-screenfuls.
        double s = k * S;
        double us = u(s);
        
        // Calculate the current point and zoom level along the flight path.
        PrecisionPoint framePoint = util::interpolate(startPoint, endPoint, us);
        double frameZoom = startZoom + state.scaleZoom(1 / w(s));
        
        // Convert to geographic coordinates and set the new viewpoint.
        LatLng frameLatLng = {
            state.yLat(framePoint.y, startWorldSize),
            state.xLng(framePoint.x, startWorldSize),
        };
        state.setLatLngZoom(frameLatLng, frameZoom);
        
        if (angle != startAngle) {
            state.angle = util::wrap(util::interpolate(startAngle, angle, k), -M_PI, M_PI);
        }
        if (pitch != startPitch) {
            state.pitch = util::interpolate(startPitch, pitch, k);
        }
        
        if (padding) {
            state.moveLatLng(frameLatLng, padding.getCenter(state.width, state.height));
        }
        return Update::Zoom;
    }, duration);
}

/** If a path crossing the antemeridian would be shorter, extend the final
 coordinate so that interpolating between the two endpoints will cross it. */
void Transform::unwrapLatLng(LatLng& latLng) {
    LatLng startLatLng = getLatLng();
    if (std::abs(startLatLng.longitude) + std::abs(latLng.longitude) > 180) {
        if (startLatLng.longitude > 0 && latLng.longitude < 0) {
            latLng.longitude += 360;
        } else if (startLatLng.longitude < 0 && latLng.longitude > 0) {
            latLng.longitude -= 360;
        }
    }
}

#pragma mark - Position

void Transform::moveBy(const PrecisionPoint& offset, const Duration& duration) {
    if (!_validPoint(offset)) {
        return;
    }

    PrecisionPoint centerOffset = {
        offset.x,
        -offset.y,
    };
    PrecisionPoint centerPoint = state.latLngToPoint(state.getLatLng()) - centerOffset;

    CameraOptions camera;
    camera.center = state.pointToLatLng(centerPoint);
    easeTo(camera, duration);
}

void Transform::setLatLng(const LatLng& latLng, const Duration& duration) {
    setLatLng(latLng, EdgeInsets(), duration);
}

void Transform::setLatLng(const LatLng& latLng, const EdgeInsets& padding, const Duration& duration) {
    if (!latLng) {
        return;
    }

    CameraOptions camera;
    camera.center = latLng;
    if (padding) {
        camera.padding = padding;
    }
    easeTo(camera, duration);
}

void Transform::setLatLng(const LatLng& latLng, const PrecisionPoint& point, const Duration& duration) {
    if (!latLng || !point) {
        return;
    }

    auto coord = state.latLngToCoordinate(latLng);
    auto coordAtPoint = state.pointToCoordinate(point);
    auto coordCenter = state.pointToCoordinate({ state.width / 2.0f, state.height / 2.0f });

    float columnDiff = coordAtPoint.column - coord.column;
    float rowDiff = coordAtPoint.row - coord.row;

    auto newLatLng = state.coordinateToLatLng({
        coordCenter.column - columnDiff,
        coordCenter.row - rowDiff,
        coordCenter.zoom
    });

    setLatLng(newLatLng, duration);
}

void Transform::setLatLngZoom(const LatLng& latLng, double zoom, const Duration& duration) {
    if (!latLng || std::isnan(zoom)) {
        return;
    }

    CameraOptions camera;
    camera.center = latLng;
    camera.zoom = zoom;
    easeTo(camera, duration);
}

LatLng Transform::getLatLng(const EdgeInsets& padding) const {
    if (padding) {
        EdgeInsets flippedPadding = padding;
        flippedPadding.flip();
        return state.pointToLatLng(flippedPadding.getCenter(state.width, state.height));
    } else {
        return state.getLatLng();
    }
}


#pragma mark - Zoom

void Transform::scaleBy(double ds, const PrecisionPoint& center, const Duration& duration) {
    if (std::isnan(ds)) {
        return;
    }

    double scale = util::clamp(state.scale * ds, state.min_scale, state.max_scale);
    setScale(scale, center, duration);
}

void Transform::setZoom(double zoom, const Duration& duration) {
    setScale(state.zoomScale(zoom), {NAN, NAN}, duration);
}

double Transform::getZoom() const {
    return state.getZoom();
}

double Transform::getScale() const {
    return state.scale;
}

void Transform::setScale(double scale, const PrecisionPoint& anchor, const Duration& duration) {
    if (std::isnan(scale)) {
        return;
    }
    
    CameraOptions camera;
    camera.zoom = state.scaleZoom(scale);
    camera.anchor = anchor;
    easeTo(camera, duration);
}

#pragma mark - Angle

void Transform::rotateBy(const PrecisionPoint& first, const PrecisionPoint& second, const Duration& duration) {
    if (!first || !second) {
        return;
    }

    PrecisionPoint center(state.width, state.height);
    center /= 2;

    const PrecisionPoint offset = first - center;
    const double distance = std::sqrt(std::pow(2, offset.x) + std::pow(2, offset.y));

    // If the first click was too close to the center, move the center of rotation by 200 pixels
    // in the direction of the click.
    if (distance < 200) {
        const double heightOffset = -200;
        const double rotateAngle = std::atan2(offset.y, offset.x);
        center.x = first.x + std::cos(rotateAngle) * heightOffset;
        center.y = first.y + std::sin(rotateAngle) * heightOffset;
    }

    const PrecisionPoint newFirst = first - center;
    const PrecisionPoint newSecond = second - center;
    const double ang = state.angle + util::angle_between(newFirst.x, newFirst.y, newSecond.x, newSecond.y);
    
    CameraOptions camera;
    camera.angle = ang;
    easeTo(camera, duration);
}

void Transform::setAngle(double angle, const Duration& duration) {
    setAngle(angle, {NAN, NAN}, duration);
}

void Transform::setAngle(double angle, const PrecisionPoint& anchor, const Duration& duration) {
    if (std::isnan(angle)) {
        return;
    }
    
    CameraOptions camera;
    camera.angle = angle;
    camera.anchor = anchor;
    easeTo(camera, duration);
}

double Transform::getAngle() const {
    return state.angle;
}

#pragma mark - Pitch

void Transform::setPitch(double pitch, const Duration& duration) {
    if (std::isnan(pitch)) {
        return;
    }

    CameraOptions camera;
    camera.pitch = pitch;
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

#pragma mark - Transition

void Transform::startTransition(const CameraOptions& camera,
                                const AnimationOptions& animation,
                                std::function<Update(double)> frame,
                                const Duration& duration) {
    if (transitionFinishFn) {
        transitionFinishFn();
    }
    
    bool isAnimated = duration != Duration::zero();
    view.notifyMapChange(isAnimated ? MapChangeRegionWillChangeAnimated : MapChangeRegionWillChange);
    
    // Associate the anchor, if given, with a coordinate.
    PrecisionPoint anchor = camera.anchor ? *camera.anchor : PrecisionPoint(NAN, NAN);
    LatLng anchorLatLng;
    if (_validPoint(anchor)) {
        anchor.y = state.getHeight() - anchor.y;
        anchorLatLng = state.pointToLatLng(anchor);
    }

    transitionStart = Clock::now();
    transitionDuration = duration;

    transitionFrameFn = [isAnimated, animation, frame, anchor, anchorLatLng, this](const TimePoint now) {
        float t = isAnimated ? (std::chrono::duration<float>(now - transitionStart) / transitionDuration) : 1.0;
        Update result;
        if (t >= 1.0) {
            result = frame(1.0);
        } else {
            util::UnitBezier ease = animation.easing ? *animation.easing : util::UnitBezier(0, 0, 0.25, 1);
            result = frame(ease.solve(t, 0.001));
        }
        
        if (_validPoint(anchor)) {
            state.moveLatLng(anchorLatLng, anchor);
        }
        
        // At t = 1.0, a DidChangeAnimated notification should be sent from finish().
        if (t < 1.0) {
            if (animation.transitionFrameFn) {
                animation.transitionFrameFn(t);
            }
            view.notifyMapChange(MapChangeRegionIsChanging);
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
        view.notifyMapChange(isAnimated ? MapChangeRegionDidChangeAnimated : MapChangeRegionDidChange);
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
