#include <mbgl/map/transform.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/unitbezier.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/platform/platform.hpp>

#include <cstdio>

using namespace mbgl;

/** Converts the given angle (in radians) to be numerically close to the anchor angle, allowing it to be interpolated properly without sudden jumps. */
static double _normalizeAngle(double angle, double anchorAngle)
{
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

Transform::Transform(View &view_)
    : view(view_)
{
}

#pragma mark - Map View

bool Transform::resize(const std::array<uint16_t, 2> size) {
    if (state.width != size[0] || state.height != size[1]) {

        view.notifyMapChange(MapChangeRegionWillChange);

        state.width = size[0];
        state.height = size[1];
        state.constrain(state.scale, state.y);

        view.notifyMapChange(MapChangeRegionDidChange);

        return true;
    } else {
        return false;
    }
}

#pragma mark - Position

void Transform::moveBy(const double dx, const double dy, const Duration& duration) {
    if (std::isnan(dx) || std::isnan(dy)) {
        return;
    }

    _moveBy(dx, dy, duration);
}

void Transform::_moveBy(const double dx, const double dy, const Duration& duration) {
    double x = state.x + std::cos(state.angle) * dx + std::sin( state.angle) * dy;
    double y = state.y + std::cos(state.angle) * dy + std::sin(-state.angle) * dx;

    state.constrain(state.scale, y);

    if (duration == Duration::zero()) {
        view.notifyMapChange(MapChangeRegionWillChange);

        state.x = x;
        state.y = y;

        view.notifyMapChange(MapChangeRegionDidChange);
    } else {
        view.notifyMapChange(MapChangeRegionWillChangeAnimated);

        const double startX = state.x;
        const double startY = state.y;
        state.panning = true;

        startTransition(
            [=](double t) {
                state.x = util::interpolate(startX, x, t);
                state.y = util::interpolate(startY, y, t);
                view.notifyMapChange(MapChangeRegionIsChanging);
                return Update::Nothing;
            },
            [=] {
                state.panning = false;
                view.notifyMapChange(MapChangeRegionDidChangeAnimated);
            }, duration);
    }
}

void Transform::setLatLng(const LatLng latLng, const Duration& duration) {
    if (std::isnan(latLng.latitude) || std::isnan(latLng.longitude)) {
        return;
    }

    const double m = 1 - 1e-15;
    const double f = ::fmin(::fmax(std::sin(util::DEG2RAD * latLng.latitude), -m), m);

    double xn = -latLng.longitude * state.Bc;
    double yn = 0.5 * state.Cc * std::log((1 + f) / (1 - f));

    _setScaleXY(state.scale, xn, yn, duration);
}

void Transform::setLatLngZoom(const LatLng latLng, const double zoom, const Duration& duration) {
    if (std::isnan(latLng.latitude) || std::isnan(latLng.longitude) || std::isnan(zoom)) {
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

    _setScaleXY(new_scale, xn, yn, duration);
}


#pragma mark - Zoom

void Transform::scaleBy(const double ds, const double cx, const double cy, const Duration& duration) {
    if (std::isnan(ds) || std::isnan(cx) || std::isnan(cy)) {
        return;
    }

    // clamp scale to min/max values
    double new_scale = state.scale * ds;
    if (new_scale < state.min_scale) {
        new_scale = state.min_scale;
    } else if (new_scale > state.max_scale) {
        new_scale = state.max_scale;
    }

    _setScale(new_scale, cx, cy, duration);
}

void Transform::setScale(const double scale, const double cx, const double cy,
                         const Duration& duration) {
    if (std::isnan(scale) || std::isnan(cx) || std::isnan(cy)) {
        return;
    }

    _setScale(scale, cx, cy, duration);
}

void Transform::setZoom(const double zoom, const Duration& duration) {
    if (std::isnan(zoom)) {
        return;
    }

    _setScale(std::pow(2.0, zoom), -1, -1, duration);
}

double Transform::getZoom() const {
    return state.getZoom();
}

double Transform::getScale() const {
    return state.scale;
}

void Transform::_setScale(double new_scale, double cx, double cy, const Duration& duration) {
    // Ensure that we don't zoom in further than the maximum allowed.
    if (new_scale < state.min_scale) {
        new_scale = state.min_scale;
    } else if (new_scale > state.max_scale) {
        new_scale = state.max_scale;
    }

    // Zoom in on the center if we don't have click or gesture anchor coordinates.
    if (cx < 0 || cy < 0) {
        cx = static_cast<double>(state.width) / 2.0;
        cy = static_cast<double>(state.height) / 2.0;
    }

    // Account for the x/y offset from the center (= where the user clicked or pinched)
    const double factor = new_scale / state.scale;
    const double dx = (cx - static_cast<double>(state.width) / 2.0) * (1.0 - factor);
    const double dy = (cy - static_cast<double>(state.height) / 2.0) * (1.0 - factor);

    // Account for angle
    const double angle_sin = std::sin(-state.angle);
    const double angle_cos = std::cos(-state.angle);
    const double ax = angle_cos * dx - angle_sin * dy;
    const double ay = angle_sin * dx + angle_cos * dy;

    const double xn = state.x * factor + ax;
    const double yn = state.y * factor + ay;

    _setScaleXY(new_scale, xn, yn, duration);
}

void Transform::_setScaleXY(const double new_scale, const double xn, const double yn,
                            const Duration& duration) {
    double scale = new_scale;
    double x = xn;
    double y = yn;

    state.constrain(scale, y);

    if (duration == Duration::zero()) {
        view.notifyMapChange(MapChangeRegionWillChange);

        state.scale = scale;
        state.x = x;
        state.y = y;
        const double s = state.scale * util::tileSize;
        state.Bc = s / 360;
        state.Cc = s / util::M2PI;

        view.notifyMapChange(MapChangeRegionDidChange);
    } else {
        view.notifyMapChange(MapChangeRegionWillChangeAnimated);

        const double startS = state.scale;
        const double startX = state.x;
        const double startY = state.y;
        state.panning = true;
        state.scaling = true;

        startTransition(
            [=](double t) {
                state.scale = util::interpolate(startS, scale, t);
                state.x = util::interpolate(startX, x, t);
                state.y = util::interpolate(startY, y, t);
                const double s = state.scale * util::tileSize;
                state.Bc = s / 360;
                state.Cc = s / util::M2PI;
                view.notifyMapChange(MapChangeRegionIsChanging);
                return Update::Zoom;
            },
            [=] {
                state.panning = false;
                state.scaling = false;
                view.notifyMapChange(MapChangeRegionDidChangeAnimated);
            }, duration);
    }
}

#pragma mark - Angle

void Transform::rotateBy(const double start_x, const double start_y, const double end_x,
                         const double end_y, const Duration& duration) {
    if (std::isnan(start_x) || std::isnan(start_y) || std::isnan(end_x) || std::isnan(end_y)) {
        return;
    }

    double center_x = static_cast<double>(state.width) / 2.0, center_y = static_cast<double>(state.height) / 2.0;

    const double begin_center_x = start_x - center_x;
    const double begin_center_y = start_y - center_y;

    const double beginning_center_dist =
        std::sqrt(begin_center_x * begin_center_x + begin_center_y * begin_center_y);

    // If the first click was too close to the center, move the center of rotation by 200 pixels
    // in the direction of the click.
    if (beginning_center_dist < 200) {
        const double offset_x = -200, offset_y = 0;
        const double rotate_angle = std::atan2(begin_center_y, begin_center_x);
        const double rotate_angle_sin = std::sin(rotate_angle);
        const double rotate_angle_cos = std::cos(rotate_angle);
        center_x = start_x + rotate_angle_cos * offset_x - rotate_angle_sin * offset_y;
        center_y = start_y + rotate_angle_sin * offset_x + rotate_angle_cos * offset_y;
    }

    const double first_x = start_x - center_x, first_y = start_y - center_y;
    const double second_x = end_x - center_x, second_y = end_y - center_y;

    const double ang = state.angle + util::angle_between(first_x, first_y, second_x, second_y);

    _setAngle(ang, duration);
}

void Transform::setAngle(const double new_angle, const Duration& duration) {
    if (std::isnan(new_angle)) {
        return;
    }

    _setAngle(new_angle, duration);
}

void Transform::setAngle(const double new_angle, const double cx, const double cy) {
    if (std::isnan(new_angle) || std::isnan(cx) || std::isnan(cy)) {
        return;
    }

    double dx = 0, dy = 0;

    if (cx >= 0 && cy >= 0) {
        dx = (static_cast<double>(state.width) / 2.0) - cx;
        dy = (static_cast<double>(state.height) / 2.0) - cy;
        _moveBy(dx, dy, Duration::zero());
    }

    _setAngle(new_angle, Duration::zero());

    if (cx >= 0 && cy >= 0) {
        _moveBy(-dx, -dy, Duration::zero());
    }
}

void Transform::_setAngle(double new_angle, const Duration& duration) {
    double angle = _normalizeAngle(new_angle, state.angle);
    state.angle = _normalizeAngle(state.angle, angle);

    if (duration == Duration::zero()) {
        view.notifyMapChange(MapChangeRegionWillChange);

        state.angle = angle;

        view.notifyMapChange(MapChangeRegionDidChange);
    } else {
        view.notifyMapChange(MapChangeRegionWillChangeAnimated);

        const double startA = state.angle;
        state.rotating = true;

        startTransition(
            [=](double t) {
                state.angle = util::wrap(util::interpolate(startA, angle, t), -M_PI, M_PI);
                view.notifyMapChange(MapChangeRegionIsChanging);
                return Update::Nothing;
            },
            [=] {
                state.rotating = false;
                view.notifyMapChange(MapChangeRegionDidChangeAnimated);
            }, duration);
    }
}

double Transform::getAngle() const {
    return state.angle;
}


#pragma mark - Transition

void Transform::startTransition(std::function<Update(double)> frame,
                                std::function<void()> finish,
                                const Duration& duration) {
    if (transitionFinishFn) {
        transitionFinishFn();
    }

    transitionStart = Clock::now();
    transitionDuration = duration;

    transitionFrameFn = [frame, this](const TimePoint now) {
        float t = std::chrono::duration<float>(now - transitionStart) / transitionDuration;
        if (t >= 1.0) {
            Update result = frame(1.0);
            transitionFinishFn();
            transitionFrameFn = nullptr;
            transitionFinishFn = nullptr;
            return result;
        } else {
            util::UnitBezier ease(0, 0, 0.25, 1);
            return frame(ease.solve(t, 0.001));
        }
    };

    transitionFinishFn = finish;
}

UpdateType Transform::updateTransitions(const TimePoint& now) {
    return static_cast<UpdateType>(transitionFrameFn ? transitionFrameFn(now) : Update::Nothing);
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
