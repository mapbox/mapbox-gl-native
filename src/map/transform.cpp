#include <mbgl/map/transform.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/transition.hpp>
#include <mbgl/platform/platform.hpp>

#include <cstdio>

using namespace mbgl;

const double D2R = M_PI / 180.0;
const double M2PI = 2 * M_PI;

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

        view.notify_map_change(MapChangeRegionWillChange);

        current.width = final.width = w;
        current.height = final.height = h;
        current.pixelRatio = final.pixelRatio = ratio;
        current.framebuffer[0] = final.framebuffer[0] = fb_w;
        current.framebuffer[1] = final.framebuffer[1] = fb_h;
        constrain(current.scale, current.y);

        view.notify_map_change(MapChangeRegionDidChange);

        return true;
    } else {
        return false;
    }
}

#pragma mark - Position

void Transform::moveBy(const double dx, const double dy, const timestamp duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _moveBy(dx, dy, duration);
}

void Transform::_moveBy(const double dx, const double dy, const timestamp duration) {
    // This is only called internally, so we don't need a lock here.

    view.notify_map_change(duration ?
                           MapChangeRegionWillChangeAnimated :
                           MapChangeRegionWillChange);

    final.x = current.x + std::cos(current.angle) * dx + std::sin(current.angle) * dy;
    final.y = current.y + std::cos(current.angle) * dy + std::sin(-current.angle) * dx;

    constrain(final.scale, final.y);

    if (duration == 0) {
        current.x = final.x;
        current.y = final.y;
    } else {
        // Use a common start time for all of the transitions to avoid divergent transitions.
        timestamp start = util::now();
        transitions.emplace_front(
            std::make_shared<util::ease_transition<double>>(current.x, final.x, current.x, start, duration));
        transitions.emplace_front(
            std::make_shared<util::ease_transition<double>>(current.y, final.y, current.y, start, duration));
    }

    view.notify_map_change(duration ?
                           MapChangeRegionDidChangeAnimated :
                           MapChangeRegionDidChange,
                           duration);
}

void Transform::setLonLat(const double lon, const double lat, const timestamp duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    const double f = std::fmin(std::fmax(std::sin(D2R * lat), -0.9999), 0.9999);
    double xn = -lon * Bc;
    double yn = 0.5 * Cc * std::log((1 + f) / (1 - f));

    _setScaleXY(current.scale, xn, yn, duration);
}

void Transform::setLonLatZoom(const double lon, const double lat, const double zoom,
                              const timestamp duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    double new_scale = std::pow(2.0, zoom);

    const double s = new_scale * util::tileSize;
    Bc = s / 360;
    Cc = s / (2 * M_PI);

    const double f = std::fmin(std::fmax(std::sin(D2R * lat), -0.9999), 0.9999);
    double xn = -lon * Bc;
    double yn = 0.5 * Cc * log((1 + f) / (1 - f));

    _setScaleXY(new_scale, xn, yn, duration);
}

void Transform::getLonLat(double &lon, double &lat) const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    final.getLonLat(lon, lat);
}

void Transform::getLonLatZoom(double &lon, double &lat, double &zoom) const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    getLonLat(lon, lat);
    zoom = getZoom();
}

void Transform::startPanning() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _clearPanning();

    // Add a 200ms timeout for resetting this to false
    current.panning = true;
    timestamp start = util::now();
    pan_timeout = std::make_shared<util::timeout<bool>>(false, current.panning, start, 200_milliseconds);
    transitions.emplace_front(pan_timeout);
}

void Transform::stopPanning() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _clearPanning();
}

void Transform::_clearPanning() {
    current.panning = false;
    if (pan_timeout) {
        transitions.remove(pan_timeout);
        pan_timeout.reset();
    }
}

#pragma mark - Zoom

void Transform::scaleBy(const double ds, const double cx, const double cy, const timestamp duration) {
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
                         const timestamp duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _setScale(scale, cx, cy, duration);
}

void Transform::setZoom(const double zoom, const timestamp duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _setScale(std::pow(2.0, zoom), -1, -1, duration);
}

double Transform::getZoom() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    return std::log(final.scale) / M_LN2;
}

double Transform::getScale() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    return final.scale;
}

void Transform::startScaling() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _clearScaling();

    // Add a 200ms timeout for resetting this to false
    current.scaling = true;
    timestamp start = util::now();
    scale_timeout = std::make_shared<util::timeout<bool>>(false, current.scaling, start, 200_milliseconds);
    transitions.emplace_front(scale_timeout);
}

void Transform::stopScaling() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _clearScaling();
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

void Transform::_clearScaling() {
    // This is only called internally, so we don't need a lock here.

    current.scaling = false;
    if (scale_timeout) {
        transitions.remove(scale_timeout);
        scale_timeout.reset();
    }
}

void Transform::_setScale(double new_scale, double cx, double cy, const timestamp duration) {
    // This is only called internally, so we don't need a lock here.

    // Ensure that we don't zoom in further than the maximum allowed.
    if (new_scale < min_scale) {
        new_scale = min_scale;
    } else if (new_scale > max_scale) {
        new_scale = max_scale;
    }

    // Zoom in on the center if we don't have click or gesture anchor coordinates.
    if (cx < 0 || cy < 0) {
        cx = current.width / 2;
        cy = current.height / 2;
    }

    // Account for the x/y offset from the center (= where the user clicked or pinched)
    const double factor = new_scale / current.scale;
    const double dx = (cx - current.width / 2) * (1.0 - factor);
    const double dy = (cy - current.height / 2) * (1.0 - factor);

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
                            const timestamp duration) {
    // This is only called internally, so we don't need a lock here.

    view.notify_map_change(duration ?
                           MapChangeRegionWillChangeAnimated :
                           MapChangeRegionWillChange);

    final.scale = new_scale;
    final.x = xn;
    final.y = yn;

    constrain(final.scale, final.y);

    if (duration == 0) {
        current.scale = final.scale;
        current.x = final.x;
        current.y = final.y;
    } else {
        // Use a common start time for all of the transitions to avoid divergent transitions.
        timestamp start = util::now();
        transitions.emplace_front(std::make_shared<util::ease_transition<double>>(
            current.scale, final.scale, current.scale, start, duration));
        transitions.emplace_front(
            std::make_shared<util::ease_transition<double>>(current.x, final.x, current.x, start, duration));
        transitions.emplace_front(
            std::make_shared<util::ease_transition<double>>(current.y, final.y, current.y, start, duration));
    }

    const double s = final.scale * util::tileSize;
    Bc = s / 360;
    Cc = s / (2 * M_PI);

    view.notify_map_change(duration ?
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
                         const double end_y, const timestamp duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    double center_x = current.width / 2, center_y = current.height / 2;

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

void Transform::setAngle(const double new_angle, const timestamp duration) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _setAngle(new_angle, duration);
}

void Transform::setAngle(const double new_angle, const double cx, const double cy) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    double dx = 0, dy = 0;

    if (cx >= 0 && cy >= 0) {
        dx = (final.width / 2) - cx;
        dy = (final.height / 2) - cy;
        _moveBy(dx, dy, 0);
    }

    _setAngle(new_angle, 0);

    if (cx >= 0 && cy >= 0) {
        _moveBy(-dx, -dy, 0);
    }
}

void Transform::_setAngle(double new_angle, const timestamp duration) {
    // This is only called internally, so we don't need a lock here.

    view.notify_map_change(duration ?
                           MapChangeRegionWillChangeAnimated :
                           MapChangeRegionWillChange);

    while (new_angle > M_PI)
        new_angle -= M2PI;
    while (new_angle <= -M_PI)
        new_angle += M2PI;

    final.angle = new_angle;

    if (duration == 0) {
        current.angle = final.angle;
    } else {
        timestamp start = util::now();
        transitions.emplace_front(std::make_shared<util::ease_transition<double>>(
            current.angle, final.angle, current.angle, start, duration));
    }

    view.notify_map_change(duration ?
                           MapChangeRegionDidChangeAnimated :
                           MapChangeRegionDidChange,
                           duration);
}

double Transform::getAngle() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    return final.angle;
}

void Transform::startRotating() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _clearRotating();

    // Add a 200ms timeout for resetting this to false
    current.rotating = true;
    timestamp start = util::now();
    rotate_timeout = std::make_shared<util::timeout<bool>>(false, current.rotating, start, 200_milliseconds);
    transitions.emplace_front(rotate_timeout);
}

void Transform::stopRotating() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    _clearRotating();
}

void Transform::_clearRotating() {
    // This is only called internally, so we don't need a lock here.

    current.rotating = false;
    if (rotate_timeout) {
        transitions.remove(rotate_timeout);
        rotate_timeout.reset();
    }
}

#pragma mark - Transition

bool Transform::needsTransition() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    return !transitions.empty();
}

void Transform::updateTransitions(const timestamp now) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    transitions.remove_if([now](const util::ptr<util::transition> &transition) {
        return transition->update(now) == util::transition::complete;
    });
}

void Transform::cancelTransitions() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    transitions.clear();
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
