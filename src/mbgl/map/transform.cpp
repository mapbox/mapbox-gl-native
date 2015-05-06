#include <mbgl/map/transform.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/std.hpp>
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

bool Transform::resize(const uint16_t w, const uint16_t h, const float ratio,
                       const uint16_t fb_w, const uint16_t fb_h) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (final.width != w || final.height != h || final.pixelRatio != ratio ||
        final.framebuffer[0] != fb_w || final.framebuffer[1] != fb_h) {

        view.notifyMapChange(MapChangeRegionWillChange);

        current.width = final.width = w;
        current.height = final.height = h;
        current.pixelRatio = final.pixelRatio = ratio;
        current.framebuffer[0] = final.framebuffer[0] = fb_w;
        current.framebuffer[1] = final.framebuffer[1] = fb_h;
        constrain(current.scale, current.y);

        view.notifyMapChange(MapChangeRegionDidChange);

        return true;
    } else {
        return false;
    }
}

#pragma mark - Position

void Transform::moveBy(const double dx, const double dy, const Duration duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _moveBy(dx, dy, duration);
}

void Transform::_moveBy(const double dx, const double dy, const Duration duration) {
    // This is only called internally, so we don't need a lock here.

    view.notifyMapChange(duration != Duration::zero() ?
                           MapChangeRegionWillChangeAnimated :
                           MapChangeRegionWillChange);

    final.x = current.x + std::cos(current.angle) * dx + std::sin(current.angle) * dy;
    final.y = current.y + std::cos(current.angle) * dy + std::sin(-current.angle) * dx;

    constrain(final.scale, final.y);

    if (duration == Duration::zero()) {
        current.x = final.x;
        current.y = final.y;
    } else {
        const double startX = current.x;
        const double startY = current.y;
        current.panning = true;

        startTransition(
            [=](double t) {
                current.x = util::interpolate(startX, final.x, t);
                current.y = util::interpolate(startY, final.y, t);
                return Update::Nothing;
            },
            [=] {
                current.panning = false;
            }, duration);
    }

    view.notifyMapChange(duration != Duration::zero() ?
                           MapChangeRegionDidChangeAnimated :
                           MapChangeRegionDidChange,
                           duration);
}

void Transform::setLatLng(const LatLng latLng, const Duration duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    const double m = 1 - 1e-15;
    const double f = std::fmin(std::fmax(std::sin(util::DEG2RAD * latLng.latitude), -m), m);

    double xn = -latLng.longitude * current.Bc;
    double yn = 0.5 * current.Cc * std::log((1 + f) / (1 - f));

    _setScaleXY(current.scale, xn, yn, duration);
}

void Transform::setLatLngZoom(const LatLng latLng, const double zoom, const Duration duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    double new_scale = std::pow(2.0, zoom);

    const double s = new_scale * util::tileSize;
    current.Bc = s / 360;
    current.Cc = s / util::M2PI;

    const double m = 1 - 1e-15;
    const double f = std::fmin(std::fmax(std::sin(util::DEG2RAD * latLng.latitude), -m), m);

    double xn = -latLng.longitude * current.Bc;
    double yn = 0.5 * current.Cc * std::log((1 + f) / (1 - f));

    _setScaleXY(new_scale, xn, yn, duration);
}


#pragma mark - Zoom

void Transform::scaleBy(const double ds, const double cx, const double cy, const Duration duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    // clamp scale to min/max values
    double new_scale = current.scale * ds;
    if (new_scale < min_scale) {
        new_scale = min_scale;
    } else if (new_scale > max_scale) {
        new_scale = max_scale;
    }

    _setScale(new_scale, cx, cy, duration);
}

void Transform::setScale(const double scale, const double cx, const double cy,
                         const Duration duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _setScale(scale, cx, cy, duration);
}

void Transform::setZoom(const double zoom, const Duration duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _setScale(std::pow(2.0, zoom), -1, -1, duration);
}

double Transform::getZoom() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    return final.getZoom();
}

double Transform::getScale() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    return final.scale;
}

double Transform::getMinZoom() const {
    double test_scale = current.scale;
    double test_y = current.y;
    constrain(test_scale, test_y);

    return std::log2(std::fmin(min_scale, test_scale));
}

double Transform::getMaxZoom() const {
    return std::log2(max_scale);
}

void Transform::_setScale(double new_scale, double cx, double cy, const Duration duration) {
    // This is only called internally, so we don't need a lock here.

    // Ensure that we don't zoom in further than the maximum allowed.
    if (new_scale < min_scale) {
        new_scale = min_scale;
    } else if (new_scale > max_scale) {
        new_scale = max_scale;
    }

    // Zoom in on the center if we don't have click or gesture anchor coordinates.
    if (cx < 0 || cy < 0) {
        cx = static_cast<double>(current.width) / 2.0;
        cy = static_cast<double>(current.height) / 2.0;
    }

    // Account for the x/y offset from the center (= where the user clicked or pinched)
    const double factor = new_scale / current.scale;
    const double dx = (cx - static_cast<double>(current.width) / 2.0) * (1.0 - factor);
    const double dy = (cy - static_cast<double>(current.height) / 2.0) * (1.0 - factor);

    // Account for angle
    const double angle_sin = std::sin(-current.angle);
    const double angle_cos = std::cos(-current.angle);
    const double ax = angle_cos * dx - angle_sin * dy;
    const double ay = angle_sin * dx + angle_cos * dy;

    const double xn = current.x * factor + ax;
    const double yn = current.y * factor + ay;

    _setScaleXY(new_scale, xn, yn, duration);
}

void Transform::_setScaleXY(const double new_scale, const double xn, const double yn,
                            const Duration duration) {
    // This is only called internally, so we don't need a lock here.

    view.notifyMapChange(duration != Duration::zero() ?
                           MapChangeRegionWillChangeAnimated :
                           MapChangeRegionWillChange);

    final.scale = new_scale;
    final.x = xn;
    final.y = yn;

    constrain(final.scale, final.y);

    if (duration == Duration::zero()) {
        current.scale = final.scale;
        current.x = final.x;
        current.y = final.y;
        const double s = current.scale * util::tileSize;
        current.Bc = s / 360;
        current.Cc = s / util::M2PI;
    } else {
        const double startS = current.scale;
        const double startX = current.x;
        const double startY = current.y;
        current.panning = true;
        current.scaling = true;

        startTransition(
            [=](double t) {
                current.scale = util::interpolate(startS, final.scale, t);
                current.x = util::interpolate(startX, final.x, t);
                current.y = util::interpolate(startY, final.y, t);
                const double s = current.scale * util::tileSize;
                current.Bc = s / 360;
                current.Cc = s / util::M2PI;
                return Update::Zoom;
            },
            [=] {
                current.panning = false;
                current.scaling = false;
            }, duration);
    }

    view.notifyMapChange(duration != Duration::zero() ?
                           MapChangeRegionDidChangeAnimated :
                           MapChangeRegionDidChange,
                           duration);
}

#pragma mark - Constraints

void Transform::constrain(double& scale, double& y) const {
    // Constrain minimum zoom to avoid zooming out far enough to show off-world areas.
    if (scale < (current.height / util::tileSize)) scale = (current.height / util::tileSize);

    // Constrain min/max vertical pan to avoid showing off-world areas.
    double max_y = ((scale * util::tileSize) - current.height) / 2;

    if (y > max_y) y = max_y;
    if (y < -max_y) y = -max_y;
}

#pragma mark - Angle

void Transform::rotateBy(const double start_x, const double start_y, const double end_x,
                         const double end_y, const Duration duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    double center_x = static_cast<double>(current.width) / 2.0, center_y = static_cast<double>(current.height) / 2.0;

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

    const double ang = current.angle + util::angle_between(first_x, first_y, second_x, second_y);

    _setAngle(ang, duration);
}

void Transform::setAngle(const double new_angle, const Duration duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _setAngle(new_angle, duration);
}

void Transform::setAngle(const double new_angle, const double cx, const double cy) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    double dx = 0, dy = 0;

    if (cx >= 0 && cy >= 0) {
        dx = (static_cast<double>(final.width) / 2.0) - cx;
        dy = (static_cast<double>(final.height) / 2.0) - cy;
        _moveBy(dx, dy, Duration::zero());
    }

    _setAngle(new_angle, Duration::zero());

    if (cx >= 0 && cy >= 0) {
        _moveBy(-dx, -dy, Duration::zero());
    }
}

void Transform::_setAngle(double new_angle, const Duration duration) {
    // This is only called internally, so we don't need a lock here.

    view.notifyMapChange(duration != Duration::zero() ?
                           MapChangeRegionWillChangeAnimated :
                           MapChangeRegionWillChange);

    final.angle = _normalizeAngle(new_angle, current.angle);

    if (duration == Duration::zero()) {
        current.angle = final.angle;
    } else {
        const double startA = current.angle;
        current.rotating = true;

        startTransition(
            [=](double t) {
                current.angle = util::interpolate(startA, final.angle, t);
                return Update::Nothing;
            },
            [=] {
                current.rotating = false;
            }, duration);
    }

    view.notifyMapChange(duration != Duration::zero() ?
                           MapChangeRegionDidChangeAnimated :
                           MapChangeRegionDidChange,
                           duration);
}

double Transform::getAngle() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    return final.angle;
}


#pragma mark - Transition

void Transform::startTransition(std::function<Update(double)> frame,
                                std::function<void()> finish,
                                Duration duration) {
    if (transitionFinishFn) {
        transitionFinishFn();
    }

    transitionStart = Clock::now();
    transitionDuration = duration;

    transitionFrameFn = [frame, this](TimePoint now) {
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

bool Transform::needsTransition() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return !!transitionFrameFn;
}

UpdateType Transform::updateTransitions(const TimePoint now) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return static_cast<UpdateType>(transitionFrameFn ? transitionFrameFn(now) : Update::Nothing);
}

void Transform::cancelTransitions() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (transitionFinishFn) {
        transitionFinishFn();
    }

    transitionFrameFn = nullptr;
    transitionFinishFn = nullptr;
}

void Transform::setGestureInProgress(bool inProgress) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    current.gestureInProgress = inProgress;
}

#pragma mark - Transform state

const TransformState Transform::currentState() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    return current;
}

const TransformState Transform::finalState() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    return final;
}
