#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/projection.hpp>
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
    pointCoordinate({ 0, 0 }).zoomTo(z),
    pointCoordinate({ w, 0 }).zoomTo(z),
    pointCoordinate({ w, h }).zoomTo(z),
    pointCoordinate({ 0, h }).zoomTo(z));
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

#pragma mark - Projection

const vec2<double> TransformState::pixelForLatLng(const LatLng latLng) const {
    LatLng ll = getLatLng();
    double zoom = getZoom();

    const double centerX = static_cast<double>(width) / 2.0;
    const double centerY = static_cast<double>(height) / 2.0;

    const double m = Projection::getMetersPerPixelAtLatitude(0, zoom);

    const double angle_sin = std::sin(-angle);
    const double angle_cos = std::cos(-angle);

    const ProjectedMeters givenMeters = Projection::projectedMetersForLatLng(latLng);

    const double givenAbsoluteX = givenMeters.easting  / m;
    const double givenAbsoluteY = givenMeters.northing / m;

    const ProjectedMeters centerMeters = Projection::projectedMetersForLatLng(ll);

    const double centerAbsoluteX = centerMeters.easting  / m;
    const double centerAbsoluteY = centerMeters.northing / m;

    const double deltaX = givenAbsoluteX - centerAbsoluteX;
    const double deltaY = givenAbsoluteY - centerAbsoluteY;

    const double translatedX = deltaX + centerX;
    const double translatedY = deltaY + centerY;

    const double rotatedX = translatedX * angle_cos - translatedY * angle_sin;
    const double rotatedY = translatedX * angle_sin + translatedY * angle_cos;

    const double rotatedCenterX = centerX * angle_cos - centerY * angle_sin;
    const double rotatedCenterY = centerX * angle_sin + centerY * angle_cos;

    double x_ = rotatedX + (centerX - rotatedCenterX);
    double y_ = rotatedY + (centerY - rotatedCenterY);

    return vec2<double>(x_, y_);
}

const LatLng TransformState::latLngForPixel(const vec2<double> pixel) const {
    LatLng ll = getLatLng();
    double zoom = getZoom();

    const double centerX = static_cast<double>(width) / 2.0;
    const double centerY = static_cast<double>(height) / 2.0;

    const double m = Projection::getMetersPerPixelAtLatitude(0, zoom);

    const double angle_sin = std::sin(angle);
    const double angle_cos = std::cos(angle);

    const double unrotatedCenterX = centerX * angle_cos - centerY * angle_sin;
    const double unrotatedCenterY = centerX * angle_sin + centerY * angle_cos;

    const double unrotatedX = pixel.x * angle_cos - pixel.y * angle_sin;
    const double unrotatedY = pixel.x * angle_sin + pixel.y * angle_cos;

    const double givenX = unrotatedX + (centerX - unrotatedCenterX);
    const double givenY = unrotatedY + (centerY - unrotatedCenterY);

    const ProjectedMeters centerMeters = Projection::projectedMetersForLatLng(ll);

    const double centerAbsoluteX = centerMeters.easting  / m;
    const double centerAbsoluteY = centerMeters.northing / m;

    const double givenAbsoluteX = givenX + centerAbsoluteX - centerX;
    const double givenAbsoluteY = givenY + centerAbsoluteY - centerY;

    ProjectedMeters givenMeters = ProjectedMeters(givenAbsoluteY * m, givenAbsoluteX * m);

    // adjust for date line
    ProjectedMeters sw, ne;
    Projection::getWorldBoundsMeters(sw, ne);
    double d = ne.easting - sw.easting;
    if (ll.longitude > 0 && givenMeters.easting > centerMeters.easting) givenMeters.easting -= d;

    // adjust for world wrap
    while (givenMeters.easting < sw.easting) givenMeters.easting += d;
    while (givenMeters.easting > ne.easting) givenMeters.easting -= d;

    return Projection::latLngForProjectedMeters(givenMeters);
}

TileCoordinate TransformState::pointCoordinate(const vec2<double> point) const {

    float targetZ = 0;
    float tileZoom = std::floor(getZoom());

    mat4 mat = coordinatePointMatrix(tileZoom);

    mat4 inverted;
    bool err = matrix::invert(inverted, mat);

    if (err) throw std::runtime_error("failed to invert coordinatePointMatrix");

    // since we don't know the correct projected z value for the point,
    // unproject two points to get a line and then find the point on that
    // line with z=0

    matrix::vec4 coord0;
    matrix::vec4 coord1;
    matrix::vec4 point0 = {{ point.x, point.y, 0, 1 }};
    matrix::vec4 point1 = {{ point.x, point.y, 1, 1 }};
    matrix::transformMat4(coord0, point0, inverted);
    matrix::transformMat4(coord1, point1, inverted);

    float w0 = coord0[3];
    float w1 = coord1[3];
    float x0 = coord0[0] / w0;
    float x1 = coord1[0] / w1;
    float y0 = coord0[1] / w0;
    float y1 = coord1[1] / w1;
    float z0 = coord0[2] / w0;
    float z1 = coord1[2] / w1;


    float t = z0 == z1 ? 0 : (targetZ - z0) / (z1 - z0);
  
    return { util::interpolate(x0, x1, t), util::interpolate(y0, y1, t), tileZoom };
}

mat4 TransformState::coordinatePointMatrix(float z) const {
    mat4 proj;
    getProjMatrix(proj);
    float s = util::tileSize * scale / std::pow(2, z);
    matrix::scale(proj, proj, s , s, 1);
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


#pragma mark - Changing

bool TransformState::isChanging() const {
    return rotating || scaling || panning || gestureInProgress;
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

double TransformState::pixel_x() const {
    const double center = (width - scale * util::tileSize) / 2;
    return center + x;
}

double TransformState::pixel_y() const {
    const double center = (height - scale * util::tileSize) / 2;
    return center + y;
}
