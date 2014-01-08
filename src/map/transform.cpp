#include <llmr/map/transform.hpp>

#include <llmr/util/mat4.h>
#include <cmath>
#include <cstdio>

using namespace llmr;


const double MAXEXTENT = 20037508.34;
const double D2R = M_PI / 180.0;
const double R2D = 180.0 / M_PI;
const double M2PI = 2 * M_PI;
const double A = 6378137;


transform::transform()
    :
    width(0),
    height(0),
    x(0),
    y(0),
    angle(0.0),
    scale(1.0) {
    setScale(scale);
    setAngle(angle);
}

void transform::moveBy(double dx, double dy) {
    x += cos(angle) * dx + sin(angle) * dy;
    y += cos(angle) * dy + sin(-angle) * dx;
}

void transform::scaleBy(double ds, double cx, double cy) {
    const double dx = (cx - width / 2) * (1.0 - ds);
    const double dy = (cy - height / 2) * (1.0 - ds);
    const double fx = cos(angle) * dx + sin(angle) * dy;
    const double fy = cos(angle) * dy + sin(-angle) * dx;

    scale *= ds;
    x = (x * ds) + fx;
    y = (y * ds) + fy;
}

void transform::rotateBy(double cx, double cy, double dx, double dy) {
    const double distance = copysign(sqrt(dx * dx + dy * dy), dx);
    setAngle(angle + D2R * distance);
}

void transform::setAngle(double new_angle) {
    angle = new_angle;
    while (angle > M_PI) angle -= M2PI;
    while (angle <= -M_PI) angle += M2PI;
}

void transform::setScale(double new_scale) {
    const double factor = new_scale / scale;
    x *= factor;
    y *= factor;
    scale = new_scale;

    const double s = scale * size;
    zc = s / 2;
    Bc = s / 360;
    Cc = s / (2 * M_PI);
}

void transform::setZoom(double zoom) {
    setScale(pow(2.0, zoom));
}

void transform::setLonLat(double lon, double lat) {
    const double f = fmin(fmax(sin(D2R * lat), -0.9999), 0.9999);
    x = -round(lon * Bc);
    y = round(0.5 * Cc * log((1 + f) / (1 - f)));
}

void transform::getLonLat(double& lon, double& lat) const {
    lon = -x / Bc;
    lat = R2D * (2 * atan(exp(y / Cc)) - 0.5 * M_PI);
}

double transform::pixel_x() const {
    const double center = (width - scale * size) / 2;
    return center + x;
}

double transform::pixel_y() const {
    const double center = (height - scale * size) / 2;
    return center + y;
}

void transform::matrixFor(float matrix[16], uint32_t tile_z, uint32_t tile_x, uint32_t tile_y) const {
    const double tile_scale = pow(2, tile_z);
    const double tile_size = scale * size / tile_scale;

    mat4_identity(matrix);

    mat4_translate(matrix, matrix, 0.5f * (float)width, 0.5f * (float)height, 0);
    mat4_rotate_z(matrix, matrix, angle);
    mat4_translate(matrix, matrix, -0.5f * (float)width, -0.5f * (float)height, 0);

    mat4_translate(matrix, matrix, pixel_x(), pixel_y(), 0);
    mat4_translate(matrix, matrix, tile_x * tile_size, tile_y * tile_size, 0);

    // TODO: Get rid of the 8 (scaling from 4096 to 512 px tile size);
    mat4_scale(matrix, matrix, scale / tile_scale / 8, scale / tile_scale / 8, 1);

    // Clipping plane
    mat4_translate(matrix, matrix, 0, 0, -1);



}
