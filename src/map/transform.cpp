#include <mbgl/map/transform.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/unitbezier.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/platform/platform.hpp>

using namespace mbgl;

const double MIN_ROTATE_ZOOM = 3;

Transform::Transform(View &view) : view(view), mtx(std::make_unique<uv::rwlock>()) {
}

#pragma mark - Map View

bool Transform::resize(const uint16_t w, const uint16_t h, const float ratio,
                       const uint16_t fb_w, const uint16_t fb_h) {
    uv::writelock lock(mtx);

    if (state.width != w || state.height != h || state.pixelRatio != ratio ||
        state.framebuffer[0] != fb_w || state.framebuffer[1] != fb_h) {

        view.notify_map_change(MapChangeRegionWillChange);

        state.width = w;
        state.height = h;
        state.pixelRatio = ratio;
        state.framebuffer[0] = fb_w;
        state.framebuffer[1] = fb_h;
//        if (!canRotate() && state.angle) setBearing(0);
//        constrain(state.scale, state.getPoint().y);

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
    state.center = center;
    state.zoom = zoom;
    state.bearing = bearing;
}

void Transform::setCenter(const LatLng& center) {
    uv::writelock lock(mtx);
    cancelTransitions();
    state.center = center;
}

void Transform::setZoom(double zoom) {
    uv::writelock lock(mtx);
    cancelTransitions();
    state.zoom = zoom;
}

void Transform::setBearing(double bearing) {
    uv::writelock lock(mtx);
    cancelTransitions();
    state.bearing = bearing;
}

LatLng Transform::getCenter() const {
    uv::readlock lock(mtx);
    return state.center;
}

double Transform::getZoom() const {
    uv::readlock lock(mtx);
    return state.zoom;
}

double Transform::getBearing() const {
    uv::readlock lock(mtx);
    return state.bearing;
}

# pragma mark - Transitions

void Transform::panBy(const Point& delta, const timestamp duration) {
    uv::writelock lock(mtx);

    panTo(state.unproject(state.getPoint().add(delta)), duration);
}

void Transform::panTo(const LatLng& center, const timestamp duration) {
    uv::writelock lock(mtx);

    Point from = state.getPoint();
    Point to = state.project(center);

    timed([this, from, to] (double t) {
        state.center = state.unproject(util::interpolate(from, to, t));
    }, duration);
}

void Transform::zoomTo(double zoom, const LatLng& center, const timestamp duration) {
    uv::writelock lock(mtx);

    double from = state.zoom;
    double to = zoom;

    timed([this, from, to, center] (double t) {
        state.setZoomAround(util::interpolate(from, to, t), center);
    }, duration);
}

void Transform::rotateTo(double bearing, const LatLng& center, const timestamp duration) {
    uv::writelock lock(mtx);

    double from = state.bearing;
    double to = bearing;

    timed([this, from, to, center] (double t) {
        state.setBearingAround(util::interpolate(from, to, t), center);
    }, duration);
}

void Transform::easeTo(const LatLng& center, double zoom, double bearing, timestamp duration) {
    Point offset;

    double startZoom = state.zoom;
    double startBearing = state.bearing;

    double scale = state.zoomScale(zoom - startZoom);
    Point from = state.getPoint();
    Point to = state.project(center);
    LatLng around = state.pointLocation(state.centerPoint().add(to.sub(from).div(1 - 1 / scale)));

    timed([this, startZoom, zoom, startBearing, bearing, around] (double t) {
        if (zoom != startZoom) {
            state.setZoomAround(util::interpolate(startZoom, zoom, t), around);
        }

        if (bearing != startBearing) {
            state.bearing = util::interpolate(startBearing, bearing, t);
        }
    }, duration);
}

void Transform::flyTo(const LatLng& center, double zoom, double bearing, timestamp duration) {
}

#pragma mark - Constraints

void Transform::constrain(double& zoom, double& y) const {
    double scale = std::pow(2, zoom);

    // Constrain minimum zoom to avoid zooming out far enough to show off-world areas.
    if (scale < (state.height / util::tileSize)) {
        scale = (state.height / util::tileSize);
        zoom = std::log2(scale);
    }

    // Constrain min/max vertical pan to avoid showing off-world areas.
    double max_y = ((scale * util::tileSize) - state.height) / 2;

    if (y > max_y) y = max_y;
    if (y < -max_y) y = -max_y;
}

bool Transform::canRotate() {
    return state.zoom > MIN_ROTATE_ZOOM;
}

double Transform::getMinZoom() const {
    double test_zoom = state.zoom;
    double test_y = state.getPoint().y;
    constrain(test_zoom, test_y);
    
    return std::fmin(min_zoom, test_zoom);
}

double Transform::getMaxZoom() const {
    return max_zoom;
}

# pragma mark - Projection

Point Transform::locationPoint(const LatLng& latlng) const {
    uv::readlock lock(mtx);
    const Point p = state.project(latlng);
    return state.centerPoint()._sub(state.getPoint()._sub(p)._rotate(state.getAngle()));
}

LatLng Transform::pointLocation(const Point& p) const {
    uv::readlock lock(mtx);
    const Point p2 = state.centerPoint()._sub(p)._rotate(-state.getAngle());
    return state.unproject(state.getPoint().sub(p2));
}

#pragma mark - Transition

util::UnitBezier ease(0, 0, 0.25, 1);

void Transform::timed(const Transition& t, const timestamp d) {
    cancelTransitions();

    view.notify_map_change(duration ?
                           MapChangeRegionWillChangeAnimated :
                           MapChangeRegionWillChange);

    if (d == 0) {
        t(1);
    } else {
        start = util::now();
        duration = d;
        transition = t;
    }

    view.notify_map_change(duration ?
                           MapChangeRegionDidChangeAnimated :
                           MapChangeRegionDidChange,
                           duration);
}

bool Transform::needsTransition() const {
    uv::readlock lock(mtx);

    return transition.operator bool();
}

void Transform::updateTransitions(const timestamp now) {
    uv::writelock lock(mtx);

    double t = (now - start) / duration;
    if (t >= 1) {
        transition(1);
        transition = nullptr;
    } else {
        transition(ease.solve(t, 0.001));
    }
}

void Transform::cancelTransitions() {
    uv::writelock lock(mtx);

    transition = nullptr;
}

#pragma mark - Transform state

const TransformState Transform::getState() const {
    uv::readlock lock(mtx);

    return state;
}
