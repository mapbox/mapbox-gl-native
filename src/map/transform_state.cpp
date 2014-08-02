#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/constants.hpp>

using namespace mbgl;

const double D2R = M_PI / 180.0;
const double R2D = 180.0 / M_PI;
const double M2PI = 2 * M_PI;

#pragma mark - Matrix

void TransformState::matrixFor(mat4& matrix, const Tile::ID& id) const {
    const double angle = getAngle();
    const double scale = getScale();
    const double tile_scale = std::pow(2, id.z);
    const double tile_size = scale * util::tileSize / tile_scale;
    const Point p = centerPoint();
    const Point c = getPoint();
    const double pixel_x = width / 2 - c.x;
    const double pixel_y = height / 2 - c.y;

    matrix::identity(matrix);

    matrix::translate(matrix, matrix, p.x, p.y, 0);
    matrix::rotate_z(matrix, matrix, angle);
    matrix::translate(matrix, matrix, -p.x, p.y, 0);
    matrix::translate(matrix, matrix, pixel_x + id.x * tile_size, pixel_y + id.y * tile_size, 0);

    // TODO: Get rid of the 8 (scaling from 4096 to tile size);
    float factor = scale / tile_scale / (4096.0f / util::tileSize);
    matrix::scale(matrix, matrix, factor, factor, 1);
}

box TransformState::cornersToBox(uint32_t z) const {
    const double ref_scale = std::pow(2, z);
    const double angle = getAngle();
    const double scale = getScale();
    const Point p = getPoint();
    const double x = p.x;
    const double y = p.y;

    const double angle_sin = std::sin(-angle);
    const double angle_cos = std::cos(-angle);

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

uint16_t TransformState::getFramebufferWidth() const {
    return framebuffer[0];
}

uint16_t TransformState::getFramebufferHeight() const {
    return framebuffer[1];
}

const std::array<uint16_t, 2> TransformState::getFramebufferDimensions() const {
    return framebuffer;
}

float TransformState::getPixelRatio() const {
    return pixelRatio;
}

#pragma mark - Zoom

float TransformState::getNormalizedZoom() const {
    return std::log(getScale() * util::tileSize / 512.0f) / M_LN2;
}

int32_t TransformState::getIntegerZoom() const {
    return std::floor(zoom);
}

double TransformState::getZoom() const {
    return zoom;
}

double TransformState::getScale() const {
    return std::pow(2, zoom);
}

#pragma mark - Rotation

double TransformState::getAngle() const {
    return -bearing * D2R;
}

#pragma mark - Changing

bool TransformState::isChanging() const {
    return rotating || scaling || panning;
}

#pragma mark - Projection

Point TransformState::project(const LatLng& latLng) const {
    const double lat = latLng.lat * D2R;
    const double lng = latLng.lng * D2R;
    const double W = getScale() * util::tileSize;
    const double R = W / M2PI;
    const double x = R * lng;
    const double y = R * std::log(std::tan(M_PI / 4 + lat / 2));
    return Point(x, y);
}

LatLng TransformState::unproject(const Point& p) const {
    const double W = getScale() * util::tileSize;
    const double R = W / M2PI;
    const double lng = p.x / R;
    const double lat = 2 * std::atan(std::exp(p.y / R)) - M_PI / 2;
    return LatLng(lat * R2D, lng * R2D);
}
