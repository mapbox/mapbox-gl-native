#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/box.hpp>

using namespace mbgl;

#pragma mark - Matrix

void TransformState::matrixFor(mat4& matrix, const TileID& id, const int8_t z) const {
    const double tile_scale = std::pow(2, z);
    const double tile_size = scale * util::tileSize / tile_scale;

    matrix::identity(matrix);

    matrix::translate(matrix, matrix, 0.5f * (float)width, 0.5f * (float)height, 0);
    matrix::rotate_z(matrix, matrix, angle);
    matrix::translate(matrix, matrix, -0.5f * (float)width, -0.5f * (float)height, 0);

    matrix::translate(matrix, matrix, pixel_x() + id.x * tile_size, pixel_y() + id.y * tile_size, 0);

    // TODO: Get rid of the 8 (scaling from 4096 to tile size);
    float factor = scale / tile_scale / (4096.0f / util::tileSize);
    matrix::scale(matrix, matrix, factor, factor, 1);
}

box TransformState::cornersToBox(uint32_t z) const {
    const double ref_scale = std::pow(2, z);

    const double angle_sin = std::sin(-angle);
    const double angle_cos = std::cos(-angle);

    const double w_2 = static_cast<double>(width) / 2.0;
    const double h_2 = static_cast<double>(height) / 2.0;
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

    return std::log2(std::fmin(min_scale, test_scale));
}

double TransformState::getMaxZoom() const {
    return std::log2(max_scale);
}


#pragma mark - Rotation

float TransformState::getAngle() const {
    return angle;
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

    const double centerX = width  / 2;
    const double centerY = height / 2;

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
