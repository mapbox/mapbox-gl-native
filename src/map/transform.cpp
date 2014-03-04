#include <llmr/map/transform.hpp>
#include <llmr/util/constants.hpp>
#include <llmr/util/mat4.hpp>
#include <llmr/util/std.hpp>
#include <llmr/util/math.hpp>
#include <cstdio>

using namespace llmr;


const double MAXEXTENT = 20037508.34;
const double D2R = M_PI / 180.0;
const double R2D = 180.0 / M_PI;
const double M2PI = 2 * M_PI;
const double A = 6378137;


Transform::Transform() {
    setScale(scale);
    setAngle(angle);
}

bool Transform::needsAnimation() const {
    return !animations.empty();
}

void Transform::updateAnimations() {
    animations.remove_if([](const std::shared_ptr<util::animation>& animation) {
        return animation->update() == util::animation::complete;
    });
}

void Transform::cancelAnimations() {
    animations.clear();
}

void Transform::moveBy(double dx, double dy, double duration) {
    double xn = x + cos(angle) * dx + sin(angle) * dy;
    double yn = y + cos(angle) * dy + sin(-angle) * dx;
    if (duration == 0) {
        x = xn;
        y = yn;
    } else {
        animations.emplace_front(std::make_shared<util::ease_animation>(x, xn, x, duration));
        animations.emplace_front(std::make_shared<util::ease_animation>(y, yn, y, duration));
    }
}

void Transform::startPanning() {
    stopPanning();

    // Add a 200ms timeout for resetting this to false
    panning = true;
    pan_timeout = std::make_shared<util::timeout<bool>>(false, panning, 0.2);
    animations.emplace_front(pan_timeout);
}

void Transform::stopPanning() {
    panning = false;
    if (pan_timeout) {
        animations.remove(pan_timeout);
        pan_timeout.reset();
    }
}

void Transform::scaleBy(double ds, double cx, double cy, double duration) {
    // clamp scale to min/max values
    double new_scale = scale * ds;
    if (new_scale < min_scale) {
        ds = min_scale / scale;
        new_scale = min_scale;
    } else if (new_scale > max_scale) {
        ds = max_scale / scale;
        new_scale = max_scale;
    }

    setScale(new_scale, cx, cy, duration);
}

void Transform::startScaling() {
    stopScaling();

    // Add a 200ms timeout for resetting this to false
    scaling = true;
    scale_timeout = std::make_shared<util::timeout<bool>>(false, scaling, 0.2);
    animations.emplace_front(scale_timeout);
}

void Transform::stopScaling() {
    scaling = false;
    if (scale_timeout) {
        animations.remove(scale_timeout);
        scale_timeout.reset();
    }
}

void Transform::rotateBy(double anchor_x, double anchor_y, double start_x, double start_y, double end_x, double end_y, double duration) {
    double center_x = width / 2, center_y = height / 2;

    const double begin_center_x = start_x - center_x;
    const double begin_center_y = start_y - center_y;

    const double beginning_center_dist = sqrt(begin_center_x * begin_center_x + begin_center_y * begin_center_y);

    // If the first click was too close to the center, move the center of rotation by 200 pixels
    // in the direction of the click.
    if (beginning_center_dist < 200) {
        const double offset_x = -200, offset_y = 0;
        const double rotate_angle = atan2(begin_center_y, begin_center_x);
        const double rotate_angle_sin = sin(rotate_angle);
        const double rotate_angle_cos = cos(rotate_angle);
        center_x = start_x + rotate_angle_cos * offset_x - rotate_angle_sin * offset_y;
        center_y = start_y + rotate_angle_sin * offset_x + rotate_angle_cos * offset_y;
    }

    const double first_x = start_x - center_x, first_y = start_y - center_y;
    const double second_x = end_x - center_x, second_y = end_y - center_y;

    const double ang = angle + util::angle_between(first_x, first_y, second_x, second_y);

    setAngle(ang, duration);
}

void Transform::setAngle(double new_angle, double duration) {
    while (new_angle > M_PI) new_angle -= M2PI;
    while (new_angle <= -M_PI) new_angle += M2PI;

    if (duration == 0) {
        angle = new_angle;
    } else {
        animations.emplace_front(std::make_shared<util::ease_animation>(angle, new_angle, angle, duration));
    }
}

void Transform::startRotating() {
    stopRotating();

    // Add a 200ms timeout for resetting this to false
    rotating = true;
    rotate_timeout = std::make_shared<util::timeout<bool>>(false, rotating, 0.2);
    animations.emplace_front(rotate_timeout);
}

void Transform::stopRotating() {
    rotating = false;
    if (rotate_timeout) {
        animations.remove(rotate_timeout);
        rotate_timeout.reset();
    }
}

void Transform::setScaleXY(double new_scale, double xn, double yn, double duration) {
    if (duration == 0) {
        scale = new_scale;
        x = xn;
        y = yn;
    } else {
        animations.emplace_front(std::make_shared<util::ease_animation>(scale, new_scale, scale, duration));
        animations.emplace_front(std::make_shared<util::ease_animation>(x, xn, x, duration));
        animations.emplace_front(std::make_shared<util::ease_animation>(y, yn, y, duration));
    }

    const double s = scale * util::tileSize;
    zc = s / 2;
    Bc = s / 360;
    Cc = s / (2 * M_PI);
}

void Transform::setScale(double new_scale, double cx, double cy, double duration) {
    if (new_scale < min_scale) {
        new_scale = min_scale;
    } else if (new_scale > max_scale) {
        new_scale = max_scale;
    }

    if (cx < 0 || cy < 0) {
        cx = width / 2;
        cy = height / 2;
    }

    const double factor = new_scale / scale;
    const double dx = (cx - width / 2) * (1.0 - factor);
    const double dy = (cy - height / 2) * (1.0 - factor);

    double xn = x * factor + dx;
    double yn = y * factor + dy;

    setScaleXY(new_scale, xn, yn, duration);
}

void Transform::setZoom(double zoom, double duration) {
    setScale(pow(2.0, zoom), -1, -1, duration);
}

void Transform::setLonLat(double lon, double lat, double duration) {
    const double f = fmin(fmax(sin(D2R * lat), -0.9999), 0.9999);
    double xn = -round(lon * Bc);
    double yn = round(0.5 * Cc * log((1 + f) / (1 - f)));

    setScaleXY(scale, xn, yn, duration);
}

void Transform::setLonLatZoom(double lon, double lat, double zoom, double duration) {
    double new_scale = pow(2.0, zoom);

    const double s = new_scale * util::tileSize;
    zc = s / 2;
    Bc = s / 360;
    Cc = s / (2 * M_PI);

    const double f = fmin(fmax(sin(D2R * lat), -0.9999), 0.9999);
    double xn = -round(lon * Bc);
    double yn = round(0.5 * Cc * log((1 + f) / (1 - f)));

    setScaleXY(new_scale, xn, yn, duration);
}

void Transform::getLonLat(double &lon, double &lat) const {
    lon = -x / Bc;
    lat = R2D * (2 * atan(exp(y / Cc)) - 0.5 * M_PI);
}

void Transform::getLonLatZoom(double& lon, double& lat, double& zoom) const {
    getLonLat(lon, lat);
    zoom = getZoom();
}

double Transform::pixel_x() const {
    const double center = (width - scale * util::tileSize) / 2;
    return center + x;
}

double Transform::pixel_y() const {
    const double center = (height - scale * util::tileSize) / 2;
    return center + y;
}

void Transform::matrixFor(mat4& matrix, const vec3<int32_t>& id) const {
    const double tile_scale = pow(2, id.z);
    const double tile_size = scale * util::tileSize / tile_scale;

    matrix::identity(matrix);

    matrix::translate(matrix, matrix, 0.5f * (float)width, 0.5f * (float)height, 0);
    matrix::rotate_z(matrix, matrix, angle);
    matrix::translate(matrix, matrix, -0.5f * (float)width, -0.5f * (float)height, 0);

    matrix::translate(matrix, matrix, pixel_x() + id.x * tile_size, pixel_y() + id.y * tile_size, 0);

    // TODO: Get rid of the 8 (scaling from 4096 to tile size);
    float factor = scale / tile_scale / (4096.0f / util::tileSize);
    matrix::scale(matrix, matrix, factor, factor, 1);

    // Clipping plane
    matrix::translate(matrix, matrix, 0, 0, -1);
}

float Transform::getZoom() const {
    return log(scale) / M_LN2;
}

int32_t Transform::getIntegerZoom() const {
    return floor(log(scale) / M_LN2);
}

double Transform::getScale() const {
    return scale;
}

double Transform::getAngle() const {
    return angle;
}

box Transform::mapCornersToBox(uint32_t z) const {
    const double ref_scale = pow(2, z);

    const double angle_sin = sin(-angle);
    const double angle_cos = cos(-angle);

    const double w_2 = width / 2;
    const double h_2 = height / 2;
    const double ss_0 = scale * util::tileSize;
    const double ss_1 = ref_scale / ss_0;
    const double ss_2 = ss_0 / 2.0;

    // Calculate the corners of the map view. The resulting coordinates will be
    // in fractional tile coordinates.
    box b;

    b.tl.x = ((-w_2) * angle_cos - (-h_2) * angle_sin + ss_2 - x) * ss_1;
    b.tl.y = ((-w_2) * angle_sin + (-h_2) * angle_cos + ss_2 - y) * ss_1;

    b.tr.x = ((+w_2) * angle_cos - (-h_2) * angle_sin + ss_2 - x) * ss_1;
    b.tr.y = ((+w_2) * angle_sin + (-h_2) * angle_cos + ss_2 - y) * ss_1;

    b.bl.x = ((-w_2) * angle_cos - (+h_2) * angle_sin + ss_2 - x) * ss_1;
    b.bl.y = ((-w_2) * angle_sin + (+h_2) * angle_cos + ss_2 - y) * ss_1;

    b.br.x = ((+w_2) * angle_cos - (+h_2) * angle_sin + ss_2 - x) * ss_1;
    b.br.y = ((+w_2) * angle_sin + (+h_2) * angle_cos + ss_2 - y) * ss_1;

    b.center.x = (ss_2 - x) * ss_1;
    b.center.y = (ss_2 - y) * ss_1;

    return b;
}

