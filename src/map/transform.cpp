#include <mbgl/map/transform.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/transition.hpp>
#include <mbgl/platform/platform.hpp>
#include <cstdio>

using namespace mbgl;

const double MIN_ROTATE_ZOOM = 3;

Transform::Transform(View &view) : view(view), mtx(std::make_unique<uv::rwlock>()) {
}

#pragma mark - Map View

bool Transform::resize(const uint16_t w, const uint16_t h, const float ratio,
                       const uint16_t fb_w, const uint16_t fb_h) {
    uv::writelock lock(mtx);

    if (final.width != w || final.height != h || final.pixelRatio != ratio ||
        final.framebuffer[0] != fb_w || final.framebuffer[1] != fb_h) {

        view.notify_map_change(MapChangeRegionWillChange);

        current.width = final.width = w;
        current.height = final.height = h;
        current.pixelRatio = final.pixelRatio = ratio;
        current.framebuffer[0] = final.framebuffer[0] = fb_w;
        current.framebuffer[1] = final.framebuffer[1] = fb_h;
//        if (!canRotate() && current.angle) setBearing(0);
//        constrain(current.scale, current.getPoint().y);

        view.notify_map_change(MapChangeRegionDidChange);

        return true;
    } else {
        return false;
    }
}

#pragma mark - Position

void Transform::setView(const LatLng& center, double zoom, double bearing) {
    uv::writelock lock(mtx);
    cancelTransitions();
    current.center = center;
    current.zoom = zoom;
    current.bearing = bearing;
    final = current;
}

void Transform::setCenter(const LatLng& center) {
    uv::writelock lock(mtx);
    cancelTransitions();
    current.center = center;
    final = current;
}

void Transform::setZoom(double zoom) {
    uv::writelock lock(mtx);
    cancelTransitions();
    current.zoom = zoom;
    final = current;
}

void Transform::setBearing(double bearing) {
    uv::writelock lock(mtx);
    cancelTransitions();
    current.bearing = bearing;
    final = current;
}

LatLng Transform::getCenter() const {
    uv::readlock lock(mtx);
    return current.center;
}

double Transform::getZoom() const {
    uv::readlock lock(mtx);
    return current.zoom;
}

double Transform::getBearing() const {
    uv::readlock lock(mtx);
    return current.bearing;
}

# pragma mark - Transitions

void Transform::panBy(const Point& delta, const timestamp duration) {
    uv::writelock lock(mtx);

    panTo(current.unproject(current.getPoint().add(delta)), duration);
}

void Transform::panTo(const LatLng& center, const timestamp duration) {
    uv::writelock lock(mtx);

    cancelTransitions();

    view.notify_map_change(duration ?
                           MapChangeRegionWillChangeAnimated :
                           MapChangeRegionWillChange);

    final.center = center;

    if (duration == 0) {
        current = final;
    } else {
        timestamp start = util::now();
        transitions.emplace_front(std::make_shared<util::ease_transition<LatLng>>(
            current.center, final.center, current.center, start, duration));
    }

    view.notify_map_change(duration ?
                           MapChangeRegionDidChangeAnimated :
                           MapChangeRegionDidChange,
                           duration);
}

void Transform::zoomTo(double zoom, const timestamp duration) {
    uv::writelock lock(mtx);

    cancelTransitions();

    zoom = std::min(std::max(zoom, min_zoom), max_zoom);

    view.notify_map_change(duration ?
                           MapChangeRegionWillChangeAnimated :
                           MapChangeRegionWillChange);

    final.zoom = zoom;

    if (duration == 0) {
        current = final;
    } else {
        timestamp start = util::now();
        transitions.emplace_front(std::make_shared<util::ease_transition<double>>(
            current.zoom, final.zoom, current.zoom, start, duration));
    }

    view.notify_map_change(duration ?
                           MapChangeRegionDidChangeAnimated :
                           MapChangeRegionDidChange,
                           duration);
}

void Transform::rotateTo(double bearing, const timestamp duration) {
    uv::writelock lock(mtx);

    cancelTransitions();

    view.notify_map_change(duration ?
                           MapChangeRegionWillChangeAnimated :
                           MapChangeRegionWillChange);

    final.bearing = bearing;

    if (duration == 0) {
        current = final;
    } else {
        timestamp start = util::now();
        transitions.emplace_front(std::make_shared<util::ease_transition<double>>(
            current.bearing, final.bearing, current.bearing, start, duration));
    }

    view.notify_map_change(duration ?
                           MapChangeRegionDidChangeAnimated :
                           MapChangeRegionDidChange,
                           duration);
}

void Transform::rotateBy(const Point& start, const Point& end, const timestamp duration) {
}

void Transform::easeTo(const LatLng& center, double zoom, double bearing, timestamp duration) {
}

void Transform::flyTo(const LatLng& center, double zoom, double bearing, timestamp duration) {
}

void Transform::startPanning() {
    uv::writelock lock(mtx);

    _clearPanning();

    // Add a 200ms timeout for resetting this to false
    current.panning = true;
    timestamp start = util::now();
    pan_timeout = std::make_shared<util::timeout<bool>>(false, current.panning, start, 200_milliseconds);
    transitions.emplace_front(pan_timeout);
}

void Transform::stopPanning() {
    uv::writelock lock(mtx);

    _clearPanning();
}

void Transform::_clearPanning() {
    current.panning = false;
    if (pan_timeout) {
        transitions.remove(pan_timeout);
        pan_timeout.reset();
    }
}

void Transform::startScaling() {
    uv::writelock lock(mtx);

    _clearScaling();

    // Add a 200ms timeout for resetting this to false
    current.scaling = true;
    timestamp start = util::now();
    scale_timeout = std::make_shared<util::timeout<bool>>(false, current.scaling, start, 200_milliseconds);
    transitions.emplace_front(scale_timeout);
}

void Transform::stopScaling() {
    uv::writelock lock(mtx);

    _clearScaling();
}

void Transform::_clearScaling() {
    // This is only called internally, so we don't need a lock here.

    current.scaling = false;
    if (scale_timeout) {
        transitions.remove(scale_timeout);
        scale_timeout.reset();
    }
}

void Transform::startRotating() {
    uv::writelock lock(mtx);
    
    _clearRotating();
    
    // Add a 200ms timeout for resetting this to false
    current.rotating = true;
    timestamp start = util::now();
    rotate_timeout = std::make_shared<util::timeout<bool>>(false, current.rotating, start, 200_milliseconds);
    transitions.emplace_front(rotate_timeout);
}

void Transform::stopRotating() {
    uv::writelock lock(mtx);
    
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

#pragma mark - Constraints

void Transform::constrain(double& zoom, double& y) const {
    double scale = std::pow(2, zoom);

    // Constrain minimum zoom to avoid zooming out far enough to show off-world areas.
    if (scale < (current.height / util::tileSize)) {
        scale = (current.height / util::tileSize);
        zoom = std::log2(scale);
    }

    // Constrain min/max vertical pan to avoid showing off-world areas.
    double max_y = ((scale * util::tileSize) - current.height) / 2;

    if (y > max_y) y = max_y;
    if (y < -max_y) y = -max_y;
}

bool Transform::canRotate() {
    return current.zoom > MIN_ROTATE_ZOOM;
}

double Transform::getMinZoom() const {
    double test_zoom = current.zoom;
    double test_y = current.getPoint().y;
    constrain(test_zoom, test_y);
    
    return std::fmin(min_zoom, test_zoom);
}

double Transform::getMaxZoom() const {
    return max_zoom;
}

# pragma mark - Projection

Point Transform::locationPoint(const LatLng& latlng) const {
    uv::readlock lock(mtx);
    const Point p = current.project(latlng);
    return current.centerPoint()._sub(current.getPoint()._sub(p)._rotate(current.getAngle()));
}

LatLng Transform::pointLocation(const Point& p) const {
    uv::readlock lock(mtx);
    const Point p2 = current.centerPoint()._sub(p)._rotate(-current.getAngle());
    return current.unproject(current.getPoint().sub(p2));
}

#pragma mark - Transition

bool Transform::needsTransition() const {
    uv::readlock lock(mtx);

    return !transitions.empty();
}

void Transform::updateTransitions(const timestamp now) {
    uv::writelock lock(mtx);

    transitions.remove_if([now](const std::shared_ptr<util::transition> &transition) {
        return transition->update(now) == util::transition::complete;
    });
}

void Transform::cancelTransitions() {
    uv::writelock lock(mtx);

    transitions.clear();
}

#pragma mark - Transform state

const TransformState Transform::currentState() const {
    uv::readlock lock(mtx);

    return current;
}

const TransformState Transform::finalState() const {
    uv::readlock lock(mtx);

    return final;
}
