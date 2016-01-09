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

#pragma mark - Position

/*
 * Change any combination of center, zoom, bearing, and pitch, without
 * a transition. The map will retain the current values for any options
 * not included in `options`.
 */
void Transform::jumpTo(const CameraOptions& camera) {
    easeTo(camera, {});
}

/*
 * Change any combination of center, zoom, bearing, and pitch, with a smooth animation
 * between old and new values. The map will retain the current values for any options
 * not included in `options`.
 */
void Transform::easeTo(const CameraOptions& camera, const AnimationOptions& animation) {
    CameraOptions easeCamera(camera);
    LatLng latLng = easeCamera.center ? *easeCamera.center : getLatLng();
    double zoom = easeCamera.zoom ? *easeCamera.zoom : getZoom();
    double angle = easeCamera.angle ? *easeCamera.angle : getAngle();
    if (!latLng || std::isnan(zoom) || std::isnan(angle)) {
        return;
    }

    double new_scale = state.zoomScale(zoom);

    const double s = new_scale * util::tileSize;
    state.Bc = s / 360;
    state.Cc = s / util::M2PI;

    const double m = 1 - 1e-15;
    const double f = ::fmin(::fmax(std::sin(util::DEG2RAD * latLng.latitude), -m), m);

    double xn = -latLng.longitude * state.Bc;
    double yn = 0.5 * state.Cc * std::log((1 + f) / (1 - f));

    easeCamera.center.reset();
    easeCamera.zoom.reset();
    easeCamera.angle.reset();
    _easeTo(easeCamera, animation, new_scale, angle, xn, yn);
}

void Transform::moveBy(const PrecisionPoint& point, const Duration& duration) {
    if (!point) {
        return;
    }

    _moveBy(point, duration);
}

void Transform::_moveBy(const PrecisionPoint& point, const Duration& duration) {
    double x = state.x + std::cos(state.angle) * point.x + std::sin( state.angle) * point.y;
    double y = state.y + std::cos(state.angle) * point.y + std::sin(-state.angle) * point.x;

    state.constrain(state.scale, x, y);

    AnimationOptions animation;
    animation.duration = duration;
    _easeTo({}, animation, state.scale, state.angle, x, y);
}

void Transform::setLatLng(const LatLng& latLng, const Duration& duration) {
    if (!latLng) {
        return;
    }

    CameraOptions camera;
    camera.center = latLng;
    AnimationOptions animation;
    animation.duration = duration;
    easeTo(camera, animation);
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
    AnimationOptions animation;
    animation.duration = duration;
    easeTo(camera, animation);
}


#pragma mark - Zoom

void Transform::scaleBy(double ds, const PrecisionPoint& center, const Duration& duration) {
    if (std::isnan(ds) || !center) {
        return;
    }

    // clamp scale to min/max values
    double new_scale = state.scale * ds;
    if (new_scale < state.min_scale) {
        new_scale = state.min_scale;
    } else if (new_scale > state.max_scale) {
        new_scale = state.max_scale;
    }

    _setScale(new_scale, center, duration);
}

void Transform::setScale(double scale, const PrecisionPoint& center, const Duration& duration) {
    if (std::isnan(scale) || !center) {
        return;
    }

    _setScale(scale, center, duration);
}

void Transform::setZoom(double zoom, const Duration& duration) {
    if (std::isnan(zoom)) {
        return;
    }

    _setScale(std::pow(2.0, zoom), { 0, 0 }, duration);
}

double Transform::getZoom() const {
    return state.getZoom();
}

double Transform::getScale() const {
    return state.scale;
}

void Transform::_setScale(double new_scale, const PrecisionPoint& center, const Duration& duration) {
    const double factor = new_scale / state.scale;
    double dx = 0;
    double dy = 0;

    if (center.x > 0 || center.y > 0) {
        auto coord = state.pointToCoordinate({ center.x, state.getHeight() - center.y }).zoomTo(state.getZoom());
        auto centerCoord = state.pointToCoordinate({ state.width / 2.0f, state.height / 2.0f }).zoomTo(state.getZoom());
        auto coordDiff = centerCoord - coord;
        dx = coordDiff.column * util::tileSize * (1.0 - factor);
        dy = coordDiff.row * util::tileSize * (1.0 - factor);
    }

    const double xn = state.x * factor - dx;
    const double yn = state.y * factor - dy;

    AnimationOptions animation;
    animation.duration = duration;
    _easeTo({}, animation, new_scale, state.angle, xn, yn);
}

void Transform::_easeTo(const CameraOptions& camera, const AnimationOptions& animation,
                        double new_scale, double new_angle, double xn, double yn) {
    Update update = state.scale == new_scale ? Update::Repaint : Update::Zoom;
    double scale = new_scale;
    double x = xn;
    double y = yn;

    state.constrain(scale, x, y);
    
    double angle = _normalizeAngle(new_angle, state.angle);
    state.angle = _normalizeAngle(state.angle, angle);
    double pitch = camera.pitch ? *camera.pitch : state.pitch;
    pitch = util::clamp(pitch, 0., util::PITCH_MAX);

    Duration duration = animation.duration ? *animation.duration : Duration::zero();
    if (duration == Duration::zero()) {
        view.notifyMapChange(MapChangeRegionWillChange);

        state.scale = scale;
        state.x = x;
        state.y = y;
        const double s = state.scale * util::tileSize;
        state.Bc = s / 360;
        state.Cc = s / util::M2PI;
        
        state.angle = angle;
        state.pitch = pitch;

        if (animation.transitionFinishFn) {
            animation.transitionFinishFn();
        }
        view.notifyMapChange(MapChangeRegionDidChange);
    } else {
        view.notifyMapChange(MapChangeRegionWillChangeAnimated);

        const double startS = state.scale;
        const double startA = state.angle;
        const double startP = state.pitch;
        const double startX = state.x;
        const double startY = state.y;
        state.panning = true;
        state.scaling = true;
        state.rotating = true;

        startTransition(
            [=](double t) {
                util::UnitBezier ease = animation.easing ? *animation.easing : util::UnitBezier(0, 0, 0.25, 1);
                return ease.solve(t, 0.001);
            },
            [=](double t) {
                state.scale = util::interpolate(startS, scale, t);
                state.x = util::interpolate(startX, x, t);
                state.y = util::interpolate(startY, y, t);
                const double s = state.scale * util::tileSize;
                state.Bc = s / 360;
                state.Cc = s / util::M2PI;
                state.angle = util::wrap(util::interpolate(startA, angle, t), -M_PI, M_PI);
                state.pitch = util::interpolate(startP, pitch, t);
                // At t = 1.0, a DidChangeAnimated notification should be sent from finish().
                if (t < 1.0) {
                    if (animation.transitionFrameFn) {
                        animation.transitionFrameFn(t);
                    }
                    view.notifyMapChange(MapChangeRegionIsChanging);
                }
                return update;
            },
            [=] {
                state.panning = false;
                state.scaling = false;
                state.rotating = false;
                if (animation.transitionFinishFn) {
                    animation.transitionFinishFn();
                }
                view.notifyMapChange(MapChangeRegionDidChangeAnimated);
            }, duration);
    }
}

/** This method implements an “optimal path” animation, as detailed in:
    
    Van Wijk, Jarke J.; Nuij, Wim A. A. “Smooth and efficient zooming and
        panning.” INFOVIS ’03. pp. 15–22.
        <https://www.win.tue.nl/~vanwijk/zoompan.pdf#page=5>.
    
    Where applicable, local variable documentation begins with the associated
    variable or function in van Wijk (2003). */
void Transform::flyTo(const CameraOptions &camera, const AnimationOptions &animation) {
    LatLng latLng = camera.center ? *camera.center : getLatLng();
    LatLng startLatLng = getLatLng();
    double zoom = camera.zoom ? *camera.zoom : getZoom();
    double angle = camera.angle ? *camera.angle : getAngle();
    double pitch = camera.pitch ? *camera.pitch : getPitch();

    if (std::isnan(latLng.latitude) || std::isnan(latLng.longitude) || std::isnan(zoom)) {
        return;
    }
    
    // If a path crossing the antemeridian would be shorter, extend the final
    // coordinate so that interpolating between the two endpoints will cross it.
    if (std::abs(startLatLng.longitude) + std::abs(latLng.longitude) > 180) {
        if (startLatLng.longitude > 0 && latLng.longitude < 0) {
            latLng.longitude += 360;
        } else if (startLatLng.longitude < 0 && latLng.longitude > 0) {
            latLng.longitude -= 360;
        }
    }
    
    const PrecisionPoint startPoint = {
        state.lngX(startLatLng.longitude),
        state.latY(startLatLng.latitude),
    };
    const PrecisionPoint endPoint = {
        state.lngX(latLng.longitude),
        state.latY(latLng.latitude),
    };
    
    zoom = util::clamp(zoom, state.getMinZoom(), state.getMaxZoom());
    
    // Minimize rotation by taking the shorter path around the circle.
    double normalizedAngle = _normalizeAngle(angle, state.angle);
    state.angle = _normalizeAngle(state.angle, normalizedAngle);
    
    pitch = util::clamp(pitch, 0., util::PITCH_MAX);
    
    const double startZoom = state.scaleZoom(state.scale);
    const double startAngle = state.angle;
    const double startPitch = state.pitch;
    
    /// w₀: Initial visible span, measured in pixels at the initial scale.
    /// Known henceforth as a <i>screenful</i>.
    double w0 = std::max(state.width, state.height);
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
    
    view.notifyMapChange(MapChangeRegionWillChangeAnimated);
    
    const double startWorldSize = state.worldSize();
    state.Bc = startWorldSize / 360;
    state.Cc = startWorldSize / util::M2PI;
    
    state.panning = true;
    state.scaling = true;
    state.rotating = angle != startAngle;
    
    startTransition(
        [=](double t) {
            util::UnitBezier ease = animation.easing ? *animation.easing : util::UnitBezier(0, 0, 0.25, 1);
            return ease.solve(t, 0.001);
        },
        [=](double k) {
            /// s: The distance traveled along the flight path, measured in
            /// ρ-screenfuls.
            double s = k * S;
            double us = u(s);
            
            // Calculate the current point and zoom level along the flight path.
            PrecisionPoint framePoint = startPoint + (endPoint - startPoint) * us;
            double frameZoom = startZoom + state.scaleZoom(1 / w(s));
            
            // Convert to geographic coordinates and set the new viewpoint.
            LatLng frameLatLng = {
                state.yLat(framePoint.y, startWorldSize),
                state.xLng(framePoint.x, startWorldSize),
            };
            state.setLatLngZoom(frameLatLng, frameZoom);
            
            if (angle != startAngle) {
                state.angle = util::wrap(util::interpolate(startAngle, normalizedAngle, k), -M_PI, M_PI);
            }
            if (pitch != startPitch) {
                state.pitch = util::interpolate(startPitch, pitch, k);
            }
            
            // At k = 1.0, a DidChangeAnimated notification should be sent from finish().
            if (k < 1.0) {
                if (animation.transitionFrameFn) {
                    animation.transitionFrameFn(k);
                }
                view.notifyMapChange(MapChangeRegionIsChanging);
            }
            return Update::Zoom;
        },
        [=] {
            state.panning = false;
            state.scaling = false;
            state.rotating = false;
            if (animation.transitionFinishFn) {
                animation.transitionFinishFn();
            }
            view.notifyMapChange(MapChangeRegionDidChangeAnimated);
        }, duration);
};

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

    _setAngle(ang, duration);
}

void Transform::setAngle(double new_angle, const Duration& duration) {
    if (std::isnan(new_angle)) {
        return;
    }

    _setAngle(new_angle, duration);
}

void Transform::setAngle(double new_angle, const PrecisionPoint& center) {
    if (std::isnan(new_angle) || !center) {
        return;
    }

    LatLng rotationCenter;

    if (center.x > 0 || center.y > 0) {
        rotationCenter = state.pointToLatLng(center);
        setLatLng(rotationCenter, Duration::zero());
    }

    _setAngle(new_angle);

    if (center.x > 0 && center.y > 0) {
        setLatLng(rotationCenter, center, Duration::zero());
    }
}

void Transform::_setAngle(double new_angle, const Duration& duration) {
    CameraOptions camera;
    camera.angle = new_angle;
    AnimationOptions animation;
    animation.duration = duration;
    easeTo(camera, animation);
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
    AnimationOptions animation;
    animation.duration = duration;
    easeTo(camera, animation);
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

void Transform::startTransition(std::function<double(double)> easing,
                                std::function<Update(double)> frame,
                                std::function<void()> finish,
                                const Duration& duration) {
    if (transitionFinishFn) {
        transitionFinishFn();
    }

    transitionStart = Clock::now();
    transitionDuration = duration;

    transitionFrameFn = [easing, frame, this](const TimePoint now) {
        float t = std::chrono::duration<float>(now - transitionStart) / transitionDuration;
        if (t >= 1.0) {
            Update result = frame(1.0);
            transitionFinishFn();
            transitionFrameFn = nullptr;
            transitionFinishFn = nullptr;
            return result;
        } else {
            return frame(easing(t));
        }
    };

    transitionFinishFn = finish;
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
