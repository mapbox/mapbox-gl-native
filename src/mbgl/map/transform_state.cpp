#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/box.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/util/interpolate.hpp>

using namespace mbgl;

#pragma mark - Matrix

void TransformState::matrixFor(mat4& matrix, const TileID& id, const int8_t z) const {
    const double tile_scale = std::pow(2, z);
    double s = util::tileSize * scale / tile_scale;

    matrix::identity(matrix);
    matrix::translate(matrix, matrix, id.x * s, id.y * s, 0);
    matrix::scale(matrix, matrix, s / 4096.0f, s / 4096.0f, 1);
}

void TransformState::getProjMatrix(mat4& projMatrix) const {
    double halfFov = std::atan(0.5 / getAltitude());
    double topHalfSurfaceDistance = std::sin(halfFov) * getAltitude() /
        std::sin(M_PI / 2.0f - getPitch() - halfFov);
    // Calculate z value of the farthest fragment that should be rendered.
    double farZ = std::cos(M_PI / 2.0f - getPitch()) * topHalfSurfaceDistance + getAltitude();

    matrix::perspective(projMatrix, 2.0f * std::atan((getHeight() / 2.0f) / getAltitude()),
            double(getWidth()) / getHeight(), 0.1, farZ);

    matrix::translate(projMatrix, projMatrix, 0, 0, -getAltitude());

    // After the rotateX, z values are in pixel units. Convert them to
    // altitude unites. 1 altitude unit = the screen height.
    matrix::scale(projMatrix, projMatrix, 1, -1, 1.0f / getHeight());

    matrix::rotate_x(projMatrix, projMatrix, getPitch());
    matrix::rotate_z(projMatrix, projMatrix, getAngle());

    matrix::translate(projMatrix, projMatrix, pixel_x() - getWidth() / 2.0f,
            pixel_y() - getHeight() / 2.0f, 0);
}

box TransformState::cornersToBox(uint32_t z) const {
    double w = width;
    double h = height;
    box b(
    pointToCoordinate({ 0, 0 }).zoomTo(z),
    pointToCoordinate({ w, 0 }).zoomTo(z),
    pointToCoordinate({ w, h }).zoomTo(z),
    pointToCoordinate({ 0, h }).zoomTo(z));
    return b;
}


#pragma mark - Dimensions

bool TransformState::hasSize() const {
    return width && height;
}

uint16_t TransformState::getWidth() const {
    return width;
}

uint16_t TransformState::getHeight() const {
    return height;
}

#pragma mark - Position

const LatLng TransformState::getLatLng() const {
    LatLng ll;

    ll.longitude = -x / Bc;
    ll.latitude  = util::RAD2DEG * (2 * std::atan(std::exp(y / Cc)) - 0.5 * M_PI);

    // adjust for world wrap
    while (ll.longitude >  180) ll.longitude -= 180;
    while (ll.longitude < -180) ll.longitude += 180;

    // adjust for date line
    double w = util::tileSize * scale / 2;
    double x_ = x;
    if (x_ > w) {
        while (x_ > w) {
            x_ -= w;
            if (ll.longitude < 0) {
                ll.longitude += 180;
            } else if (ll.longitude > 0) {
                ll.longitude -= 180;
            }
        }
    } else if (x_ < -w) {
        while (x_ < -w) {
            x_ += w;
            if (ll.longitude < 0) {
                ll.longitude -= 180;
            } else if (ll.longitude > 0) {
                ll.longitude -= 180;
            }
        }
    }

    return ll;
}

double TransformState::pixel_x() const {
    const double center = (width - scale * util::tileSize) / 2;
    return center + x;
}

double TransformState::pixel_y() const {
    const double center = (height - scale * util::tileSize) / 2;
    return center + y;
}

#pragma mark - Zoom

float TransformState::getNormalizedZoom() const {
    return std::log(scale * util::tileSize / 512.0f) / M_LN2;
}

double TransformState::getZoom() const {
    return std::log(scale) / M_LN2;
}

int32_t TransformState::getIntegerZoom() const {
    return std::floor(getZoom());
}

double TransformState::getZoomFraction() const {
    return getZoom() - getIntegerZoom();
}

double TransformState::getScale() const {
    return scale;
}

double TransformState::getMinZoom() const {
    double test_scale = scale;
    double test_y = y;
    constrain(test_scale, test_y);

    return ::log2(::fmin(min_scale, test_scale));
}

double TransformState::getMaxZoom() const {
    return ::log2(max_scale);
}


#pragma mark - Rotation

float TransformState::getAngle() const {
    return angle;
}

float TransformState::getAltitude() const {
    return altitude;
}

float TransformState::getPitch() const {
    return pitch;
}


#pragma mark - State

bool TransformState::isChanging() const {
    return rotating || scaling || panning || gestureInProgress;
}

bool TransformState::isRotating() const {
    return rotating;
}

bool TransformState::isScaling() const {
    return scaling;
}

bool TransformState::isPanning() const {
    return panning;
}

bool TransformState::isGestureInProgress() const {
    return gestureInProgress;
}


#pragma mark - Projection

double TransformState::lngX(double lng) const {
    return (180.0f + lng) * worldSize() / 360.0f;
}

double TransformState::latY(double lat) const {
    double y_ = 180.0f / M_PI * std::log(std::tan(M_PI / 4 + lat * M_PI / 360.0f));
    return (180.0f - y_) * worldSize() / 360.0f;
}

double TransformState::xLng(double x_, double worldSize_) const {
    return x_ * 360.0f / worldSize_ - 180.0f;
}

double TransformState::yLat(double y_, double worldSize_) const {
    double y2 = 180.0f - y_ * 360.0f / worldSize_;
    return 360.0f / M_PI * std::atan(std::exp(y2 * M_PI / 180.0f)) - 90.0f;
}

double TransformState::zoomScale(double zoom) const {
    return std::pow(2.0f, zoom);
}

float TransformState::worldSize() const {
    return util::tileSize * scale;
}

vec2<double> TransformState::latLngToPoint(const LatLng& latLng) const {
    return coordinateToPoint(latLngToCoordinate(latLng));
}

LatLng TransformState::pointToLatLng(const vec2<double> point) const {
    return coordinateToLatLng(pointToCoordinate(point));
}

TileCoordinate TransformState::latLngToCoordinate(const LatLng& latLng) const {
    const double tileZoom = getZoom();
    const double k = zoomScale(tileZoom) / worldSize();
    return {
        lngX(latLng.longitude) * k,
        latY(latLng.latitude) * k,
        tileZoom
    };
}

LatLng TransformState::coordinateToLatLng(const TileCoordinate& coord) const {
    const double worldSize_ = zoomScale(coord.zoom);
    LatLng latLng = {
        yLat(coord.row, worldSize_),
        xLng(coord.column, worldSize_)
    };
    while (latLng.longitude < -180.0f) latLng.longitude += 360.0f;
    while (latLng.longitude > 180.0f) latLng.longitude -= 360.0f;
    return latLng;
}

vec2<double> TransformState::coordinateToPoint(const TileCoordinate& coord) const {
    mat4 mat = coordinatePointMatrix(coord.zoom);
    matrix::vec4 p;
    matrix::vec4 c = {{ coord.column, coord.row, 0, 1 }};
    matrix::transformMat4(p, c, mat);
    return { p[0] / p[3], height - p[1] / p[3] };
}

TileCoordinate TransformState::pointToCoordinate(const vec2<double> point) const {

    float targetZ = 0;
    const double tileZoom = getZoom();

    mat4 mat = coordinatePointMatrix(tileZoom);

    mat4 inverted;
    bool err = matrix::invert(inverted, mat);

    if (err) throw std::runtime_error("failed to invert coordinatePointMatrix");

    double flippedY = height - point.y;

    // since we don't know the correct projected z value for the point,
    // unproject two points to get a line and then find the point on that
    // line with z=0

    matrix::vec4 coord0;
    matrix::vec4 coord1;
    matrix::vec4 point0 = {{ point.x, flippedY, 0, 1 }};
    matrix::vec4 point1 = {{ point.x, flippedY, 1, 1 }};
    matrix::transformMat4(coord0, point0, inverted);
    matrix::transformMat4(coord1, point1, inverted);

    double w0 = coord0[3];
    double w1 = coord1[3];
    double x0 = coord0[0] / w0;
    double x1 = coord1[0] / w1;
    double y0 = coord0[1] / w0;
    double y1 = coord1[1] / w1;
    double z0 = coord0[2] / w0;
    double z1 = coord1[2] / w1;

    double t = z0 == z1 ? 0 : (targetZ - z0) / (z1 - z0);
  
    return { util::interpolate(x0, x1, t), util::interpolate(y0, y1, t), tileZoom };
}

mat4 TransformState::coordinatePointMatrix(double z) const {
    mat4 proj;
    getProjMatrix(proj);
    float s = util::tileSize * scale / std::pow(2, z);
    matrix::scale(proj, proj, s, s, 1);
    matrix::multiply(proj, getPixelMatrix(), proj);
    return proj;
}

mat4 TransformState::getPixelMatrix() const {
    mat4 m;
    matrix::identity(m);
    matrix::scale(m, m, width / 2.0f, -height / 2.0f, 1);
    matrix::translate(m, m, 1, -1, 0);
    return m;
}


#pragma mark - (private helper functions)

void TransformState::constrain(double& scale_, double& y_) const {
    // Constrain minimum zoom to avoid zooming out far enough to show off-world areas.
    if (scale_ < height / util::tileSize) {
        scale_ = height / util::tileSize;
    }

    // Constrain min/max vertical pan to avoid showing off-world areas.
    double max_y = ((scale_ * util::tileSize) - height) / 2;

    if (y_ > max_y) y_ = max_y;
    if (y_ < -max_y) y_ = -max_y;
}


