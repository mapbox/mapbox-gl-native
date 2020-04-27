#include "camera.hpp"
#include <cassert>
#include <cmath>
#include <mbgl/map/camera.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/projection.hpp>

namespace mbgl {

namespace {
double vec2Len(const vec2& v) {
    return std::sqrt(v[0] * v[0] + v[1] * v[1]);
};

double vec2Dot(const vec2& a, const vec2& b) {
    return a[0] * b[0] + a[1] * b[1];
};

vec2 vec2Scale(const vec2& v, double s) {
    return vec2{{v[0] * s, v[1] * s}};
};
} // namespace

namespace util {

static double mercatorXfromLng(double lng) {
    return (180.0 + lng) / 360.0;
}

static double mercatorYfromLat(double lat) {
    return (180.0 - (180.0 / M_PI * std::log(std::tan(M_PI_4 + lat * M_PI / 360.0)))) / 360.0;
}

static double latFromMercatorY(double y) {
    return util::RAD2DEG * (2.0 * std::atan(std::exp(M_PI - y * util::M2PI)) - M_PI_2);
}

static double lngFromMercatorX(double x) {
    return x * 360.0 - 180.0;
}

static double* getColumn(mat4& matrix, int col) {
    assert(col >= 0 && col < 4);
    return &matrix[col * 4];
}

static const double* getColumn(const mat4& matrix, int col) {
    assert(col >= 0 && col < 4);
    return &matrix[col * 4];
}

static vec3 toMercator(const LatLng& location, double altitudeMeters) {
    const double pixelsPerMeter = 1.0 / Projection::getMetersPerPixelAtLatitude(location.latitude(), 0.0);
    const double worldSize = Projection::worldSize(std::pow(2.0, 0.0));

    return {{mercatorXfromLng(location.longitude()),
             mercatorYfromLat(location.latitude()),
             altitudeMeters * pixelsPerMeter / worldSize}};
}

static Quaternion orientationFromPitchBearing(double pitch, double bearing) {
    // Both angles have to be negated to achieve CW rotation around the axis of rotation
    Quaternion rotBearing = Quaternion::fromAxisAngle({{0.0, 0.0, 1.0}}, -bearing);
    Quaternion rotPitch = Quaternion::fromAxisAngle({{1.0, 0.0, 0.0}}, -pitch);

    return rotBearing.multiply(rotPitch);
}

static void updateTransform(mat4& transform, const Quaternion& orientation) {
    // Construct rotation matrix from orientation
    mat4 m = orientation.toRotationMatrix();

    // Apply translation to the matrix
    double* col = getColumn(m, 3);

    col[0] = getColumn(transform, 3)[0];
    col[1] = getColumn(transform, 3)[1];
    col[2] = getColumn(transform, 3)[2];

    transform = m;
}

static void updateTransform(mat4& transform, const vec3& position) {
    getColumn(transform, 3)[0] = position[0];
    getColumn(transform, 3)[1] = position[1];
    getColumn(transform, 3)[2] = position[2];
}

Camera::Camera() : orientation(Quaternion::identity) {
    matrix::identity(transform);
}

vec3 Camera::getPosition() const {
    const double* p = getColumn(transform, 3);
    return {{p[0], p[1], p[2]}};
}

mat4 Camera::getCameraToWorld(double scale, bool flippedY) const {
    mat4 cameraToWorld;
    matrix::invert(cameraToWorld, getWorldToCamera(scale, flippedY));
    return cameraToWorld;
}

mat4 Camera::getWorldToCamera(double scale, bool flippedY) const {
    // transformation chain from world space to camera space:
    // 1. Height value (z) of renderables is in meters. Scale z coordinate by pixelsPerMeter
    // 2. Transform from pixel coordinates to camera space with cameraMatrix^-1
    // 3. flip Y if required

    // worldToCamera: flip * cam^-1 * zScale
    // cameraToWorld: (flip * cam^-1 * zScale)^-1 => (zScale^-1 * cam * flip^-1)
    const double worldSize = Projection::worldSize(scale);
    const double latitude = latFromMercatorY(getColumn(transform, 3)[1]);
    const double pixelsPerMeter = worldSize / (std::cos(latitude * util::DEG2RAD) * util::M2PI * util::EARTH_RADIUS_M);

    // Compute inverse of the camera matrix
    mat4 result = orientation.conjugate().toRotationMatrix();

    matrix::translate(
        result, result, -transform[12] * worldSize, -transform[13] * worldSize, -transform[14] * worldSize);

    if (!flippedY) {
        // Pre-multiply y
        result[1] *= -1.0;
        result[5] *= -1.0;
        result[9] *= -1.0;
        result[13] *= -1.0;
    }

    // Post-multiply z
    result[8] *= pixelsPerMeter;
    result[9] *= pixelsPerMeter;
    result[10] *= pixelsPerMeter;
    result[11] *= pixelsPerMeter;

    return result;
}

mat4 Camera::getCameraToClipPerspective(double fovy, double aspectRatio, double nearZ, double farZ) const {
    mat4 projection;
    matrix::perspective(projection, fovy, aspectRatio, nearZ, farZ);
    return projection;
}

vec3 Camera::forward() const {
    const double* column = getColumn(transform, 2);
    // The forward direction is towards the map, [0, 0, -1]
    return {{-column[0], -column[1], -column[2]}};
}

vec3 Camera::right() const {
    const double* column = getColumn(transform, 0);
    return {{column[0], column[1], column[2]}};
}

vec3 Camera::up() const {
    const double* column = getColumn(transform, 1);
    // Up direction has to be flipped due to y-axis pointing towards south
    return {{-column[0], -column[1], -column[2]}};
}

void Camera::getOrientation(double& pitch, double& bearing) const {
    const vec3 f = forward();
    const vec3 r = right();

    bearing = std::atan2(-r[1], r[0]);
    pitch = std::atan2(std::sqrt(f[0] * f[0] + f[1] * f[1]), -f[2]);
}

void Camera::setOrientation(double pitch, double bearing) {
    orientation = orientationFromPitchBearing(pitch, bearing);
    updateTransform(transform, orientation);
}

void Camera::setOrientation(const Quaternion& orientation_) {
    orientation = orientation_;
    updateTransform(transform, orientation);
}

void Camera::setPosition(const vec3& position) {
    updateTransform(transform, position);
}

optional<Quaternion> Camera::orientationFromFrame(const vec3& forward, const vec3& up) {
    vec3 upVector = up;

    vec2 xyForward = {{forward[0], forward[1]}};
    vec2 xyUp = {{up[0], up[1]}};
    const double epsilon = 1e-15;

    // Remove roll-component of the resulting orientation by projecting
    // the up-vector to the forward vector on xy-plane
    if (vec2Len(xyForward) >= epsilon) {
        const vec2 xyDir = vec2Scale(xyForward, 1.0 / vec2Len(xyForward));

        xyUp = vec2Scale(xyDir, vec2Dot(xyUp, xyDir));
        upVector[0] = xyUp[0];
        upVector[1] = xyUp[1];
    }

    const vec3 right = vec3Cross(upVector, forward);

    if (vec3Length(right) < epsilon) {
        return nullopt;
    }

    const double bearing = std::atan2(-right[1], right[0]);
    const double pitch = std::atan2(std::sqrt(forward[0] * forward[0] + forward[1] * forward[1]), -forward[2]);

    return util::orientationFromPitchBearing(pitch, bearing);
}
} // namespace util
} // namespace mbgl