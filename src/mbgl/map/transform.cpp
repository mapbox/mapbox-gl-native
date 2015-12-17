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

void Transform::jumpTo(const CameraOptions& options) {
    CameraOptions jumpOptions(options);
    jumpOptions.duration.reset();
    easeTo(jumpOptions);
}

void Transform::easeTo(const CameraOptions& options) {
    CameraOptions easeOptions(options);
    LatLng latLng = easeOptions.center ? *easeOptions.center : getLatLng();
    double zoom = easeOptions.zoom ? *easeOptions.zoom : getZoom();
    double angle = easeOptions.angle ? *easeOptions.angle : getAngle();
    if (!latLng || std::isnan(zoom) || std::isnan(angle)) {
        return;
    }

    double new_scale = std::pow(2.0, zoom);

    const double s = new_scale * util::tileSize;
    state.Bc = s / 360;
    state.Cc = s / util::M2PI;

    const double m = 1 - 1e-15;
    const double f = ::fmin(::fmax(std::sin(util::DEG2RAD * latLng.latitude), -m), m);

    double xn = -latLng.longitude * state.Bc;
    double yn = 0.5 * state.Cc * std::log((1 + f) / (1 - f));

    easeOptions.center.reset();
    easeOptions.zoom.reset();
    easeOptions.angle.reset();
    _easeTo(easeOptions, new_scale, angle, xn, yn);
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

    CameraOptions options;
    options.duration = duration;
    _easeTo(options, state.scale, state.angle, x, y);
}

void Transform::setLatLng(const LatLng& latLng, const Duration& duration) {
    if (!latLng) {
        return;
    }

    CameraOptions options;
    options.center = latLng;
    options.duration = duration;
    easeTo(options);
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

    CameraOptions options;
    options.center = latLng;
    options.zoom = zoom;
    options.duration = duration;
    easeTo(options);
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
    // Ensure that we don't zoom in further than the maximum allowed.
    if (new_scale < state.min_scale) {
        new_scale = state.min_scale;
    } else if (new_scale > state.max_scale) {
        new_scale = state.max_scale;
    }

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

    _setScaleXY(new_scale, xn, yn, duration);
}

void Transform::_setScaleXY(double new_scale, double xn, double yn,
                            const Duration& duration) {
    CameraOptions options;
    options.duration = duration;
    _easeTo(options, new_scale, state.angle, xn, yn);
}

void Transform::_easeTo(const CameraOptions& options, double new_scale, double new_angle, double xn, double yn) {
    CameraOptions easeOptions(options);
    Update update = state.scale == new_scale ? Update::Repaint : Update::Zoom;
    double scale = new_scale;
    double x = xn;
    double y = yn;

    state.constrain(scale, x, y);
    
    double angle = _normalizeAngle(new_angle, state.angle);
    state.angle = _normalizeAngle(state.angle, angle);
    double pitch = easeOptions.pitch ? *easeOptions.pitch : state.pitch;

    if (!easeOptions.duration) {
        easeOptions.duration = Duration::zero();
    }
    if (!easeOptions.duration || *easeOptions.duration == Duration::zero()) {
        view.notifyMapChange(MapChangeRegionWillChange);

        state.scale = scale;
        state.x = x;
        state.y = y;
        const double s = state.scale * util::tileSize;
        state.Bc = s / 360;
        state.Cc = s / util::M2PI;
        
        state.angle = angle;
        state.pitch = pitch;

        if (options.transitionFinishFn) {
            options.transitionFinishFn();
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
                util::UnitBezier ease = easeOptions.easing ? *easeOptions.easing : util::UnitBezier(0, 0, 0.25, 1);
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
                    if (options.transitionFrameFn) {
                        options.transitionFrameFn(t);
                    }
                    view.notifyMapChange(MapChangeRegionIsChanging);
                }
                return update;
            },
            [=] {
                state.panning = false;
                state.scaling = false;
                state.rotating = false;
                if (options.transitionFinishFn) {
                    options.transitionFinishFn();
                }
                view.notifyMapChange(MapChangeRegionDidChangeAnimated);
            }, *easeOptions.duration);
    }
}

void Transform::flyTo(const CameraOptions &options) {

    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - getZoom(): %f", getZoom());
    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - getAngle(): %f", getAngle());
    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - getPitch(): %f", getPitch());

    CameraOptions flyOptions(options);
    LatLng latLng = options.center ? *options.center : getLatLng();
    LatLng startLatLng = getLatLng();
    double zoom = flyOptions.zoom ? *flyOptions.zoom : getZoom();
    double angle = flyOptions.angle ? *flyOptions.angle : getAngle();
    double pitch = flyOptions.pitch ? *flyOptions.pitch : getPitch();


    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - zoom: %f", zoom);
    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - angle: %f", angle);
    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - pitch: %f", pitch);

    if (std::isnan(latLng.latitude) || std::isnan(latLng.longitude) || std::isnan(zoom)) {
        return;
    }
    
    double new_scale = std::pow(2.0, zoom);
    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - new_scale: %f", new_scale);
    
    const double scaled_tile_size = new_scale * util::tileSize;
    state.Bc = scaled_tile_size / 360;
    state.Cc = scaled_tile_size / util::M2PI;

    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - scaled_tile_size: %f", scaled_tile_size);
    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - state.Bc: %f", state.Bc);
    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - state.Cc: %f", state.Cc);
    
    const double m = 1 - 1e-15;
    const double f = ::fmin(::fmax(std::sin(util::DEG2RAD * latLng.latitude), -m), m);
    
    double xn = -latLng.longitude * state.Bc;
    double yn = 0.5 * state.Cc * std::log((1 + f) / (1 - f));
    
    view.notifyMapChange(MapChangeRegionWillChangeAnimated);
    
    const double startZ = state.scaleZoom(state.scale);
    const double startA = state.angle;
    const double startP = state.pitch;
    state.panning = true;
    state.scaling = true;
    state.rotating = true;

    mbgl::Log::Info(mbgl::Event::JNI, "flyTo - startZ: %f", startZ);
    
    double rho = flyOptions.curve ? *flyOptions.curve : 1.42;
    double w0 = std::max(state.width, state.height);
    double w1 = w0 / new_scale;
    double u1 = ::hypot(xn, yn);
    double rho2 = rho * rho;
    
    auto r = [=](double i) {
        double b = (w1 * w1 - w0 * w0 + (i ? -1 : 1) * rho2 * rho2 * u1 * u1) / (2 * (i ? w1 : w0) * rho2 * u1);
        return std::log(std::sqrt(b * b + 1) - b);
    };
    
    bool is_close = std::abs(u1) < 0.000001;
    if (is_close && std::abs(w0 - w1) < 0.000001) {
        return;
    }
    
    double r0 = r(0);
    auto w = [=](double s) {
        return (is_close ? std::exp((w1 < w0 ? -1 : 1) * rho * s)
                : (std::cosh(r0) / std::cosh(r0 + rho * s)));
    };
    auto u = [=](double s) {
        return (is_close ? 0.
                : (w0 * ((std::cosh(r0) * std::tanh(r0 + rho * s) - std::sinh(r0)) / rho2) / u1));
    };
    double S = (is_close ? (std::abs(std::log(w1 / w0)) / rho)
                : ((r(1) - r0) / rho));
    
    Duration duration = flyOptions.duration ? *flyOptions.duration : Duration::zero();
    if (flyOptions.duration) {
        duration = *flyOptions.duration;
    } else {
        double speed = flyOptions.speed ? *flyOptions.speed : 1.2;
        duration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double, std::chrono::seconds::period>(S / speed));
    }
    startTransition(
        [=](double t) {
            util::UnitBezier ease = flyOptions.easing ? *flyOptions.easing : util::UnitBezier(0, 0, 0.25, 1);
            return ease.solve(t, 0.001);
        },
        [=](double k) {
            double s = k * S;
            double us = u(s);
            
            //First calculate the desired latlng
            double desiredLat = startLatLng.latitude + (latLng.latitude - startLatLng.latitude) * us;
            double desiredLng = startLatLng.longitude + (latLng.longitude - startLatLng.longitude) * us;
            
            //Now calculate desired zoom
            double desiredZoom = startZ + state.scaleZoom(1 / w(s));
            double desiredScale = state.zoomScale(desiredZoom);
            state.scale = ::fmax(::fmin(desiredScale, state.max_scale), state.min_scale);

            mbgl::Log::Info(mbgl::Event::JNI, "flyTo - desiredZoom: %f", desiredZoom);
            mbgl::Log::Info(mbgl::Event::JNI, "flyTo - desiredScale: %f", desiredScale);
            mbgl::Log::Info(mbgl::Event::JNI, "flyTo - state.scale: %f", state.scale);
            
            //Now set values
            const double new_scaled_tile_size = state.scale * util::tileSize;
            state.Bc = new_scaled_tile_size / 360;
            state.Cc = new_scaled_tile_size / util::M2PI;
            
            const double f2 = ::fmin(::fmax(std::sin(util::DEG2RAD * desiredLat), -m), m);
            state.x = -desiredLng * state.Bc;
            state.y = 0.5 * state.Cc * std::log((1 + f2) / (1 - f2));
            
            if (angle != startA) {
                state.angle = util::wrap(util::interpolate(startA, angle, k), -M_PI, M_PI);
            }
            if (pitch != startP) {
                state.pitch = util::clamp(util::interpolate(startP, pitch, k), 0., 60.);
            }
            // At k = 1.0, a DidChangeAnimated notification should be sent from finish().
            if (k < 1.0) {
                if (options.transitionFrameFn) {
                    options.transitionFrameFn(k);
                }
                view.notifyMapChange(MapChangeRegionIsChanging);
            }
            return Update::Zoom;
        },
        [=] {
            state.panning = false;
            state.scaling = false;
            state.rotating = false;
            if (options.transitionFinishFn) {
                options.transitionFinishFn();
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
    CameraOptions options;
    options.angle = new_angle;
    options.duration = duration;
    easeTo(options);
}

double Transform::getAngle() const {
    return state.angle;
}

#pragma mark - Pitch

void Transform::setPitch(double pitch, const Duration& duration) {
    if (std::isnan(pitch)) {
        return;
    }

    CameraOptions options;
    options.pitch = pitch;
    options.duration = duration;
    easeTo(options);
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
