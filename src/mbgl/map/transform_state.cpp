#include <mbgl/map/transform_state.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {
TransformState::TransformState(ConstrainMode constrainMode_, ViewportMode viewportMode_)
    : bounds(LatLngBounds()), constrainMode(constrainMode_), viewportMode(viewportMode_) {}

void TransformState::setProperties(const TransformStateProperties& properties) {
    if (properties.x) {
        setX(*properties.x);
    }
    if (properties.y) {
        setY(*properties.y);
    }
    if (properties.scale) {
        setScale(*properties.scale);
    }
    if (properties.bearing) {
        setBearing(*properties.bearing);
    }
    if (properties.pitch) {
        setPitch(*properties.pitch);
    }
    if (properties.xSkew) {
        setXSkew(*properties.xSkew);
    }
    if (properties.ySkew) {
        setYSkew(*properties.ySkew);
    }
    if (properties.axonometric) {
        setAxonometric(*properties.axonometric);
    }
    if (properties.panning) {
        setPanningInProgress(*properties.panning);
    }
    if (properties.scaling) {
        setScalingInProgress(*properties.scaling);
    }
    if (properties.rotating) {
        setRotatingInProgress(*properties.rotating);
    }
    if (properties.edgeInsets) {
        setEdgeInsets(*properties.edgeInsets);
    }
    if (properties.size) {
        setSize(*properties.size);
    }
    if (properties.constrain) {
        setConstrainMode(*properties.constrain);
    }
    if (properties.northOrientation) {
        setNorthOrientation(*properties.northOrientation);
    }
    if (properties.viewPortMode) {
        setViewportMode(*properties.viewPortMode);
    }
}

#pragma mark - Matrix

void TransformState::matrixFor(mat4& matrix, const UnwrappedTileID& tileID) const {
    const uint64_t tileScale = 1ull << tileID.canonical.z;
    const double s = Projection::worldSize(scale) / tileScale;

    matrix::identity(matrix);
    matrix::translate(
        matrix, matrix, int64_t(tileID.canonical.x + tileID.wrap * tileScale) * s, int64_t(tileID.canonical.y) * s, 0);
    matrix::scale(matrix, matrix, s / util::EXTENT, s / util::EXTENT, 1);
}

void TransformState::getProjMatrix(mat4& projMatrix, uint16_t nearZ, bool aligned) const {
    if (size.isEmpty()) {
        return;
    }

    const double cameraToCenterDistance = getCameraToCenterDistance();
    auto offset = getCenterOffset();

    // Find the Z distance from the viewport center point
    // [width/2 + offset.x, height/2 + offset.y] to the top edge; to point
    // [width/2 + offset.x, 0] in Z units.
    // 1 Z unit is equivalent to 1 horizontal px at the center of the map
    // (the distance between[width/2, height/2] and [width/2 + 1, height/2])
    // See https://github.com/mapbox/mapbox-gl-native/pull/15195 for details.
    // See TransformState::fov description: fov = 2 * arctan((height / 2) / (height * 1.5)).
    const double tanFovAboveCenter = (size.height * 0.5 + offset.y) / (size.height * 1.5);
    const double tanMultiple = tanFovAboveCenter * std::tan(getPitch());
    assert(tanMultiple < 1);
    // Calculate z distance of the farthest fragment that should be rendered.
    const double furthestDistance = cameraToCenterDistance / (1 - tanMultiple);
    // Add a bit extra to avoid precision problems when a fragment's distance is exactly `furthestDistance`
    const double farZ = furthestDistance * 1.01;

    matrix::perspective(projMatrix, getFieldOfView(), double(size.width) / size.height, nearZ, farZ);

    // Move the center of perspective to center of specified edgeInsets.
    // Values are in range [-1, 1] where the upper and lower range values
    // position viewport center to the screen edges. This is overriden
    // if using axonometric perspective (not in public API yet, Issue #11882).
    // TODO(astojilj): Issue #11882 should take edge insets into account, too.
    projMatrix[8] = -offset.x * 2.0 / size.width;
    projMatrix[9] = offset.y * 2.0 / size.height;

    const bool flippedY = viewportMode == ViewportMode::FlippedY;
    matrix::scale(projMatrix, projMatrix, 1.0, flippedY ? 1 : -1, 1);

    matrix::translate(projMatrix, projMatrix, 0, 0, -cameraToCenterDistance);

    using NO = NorthOrientation;
    switch (getNorthOrientation()) {
        case NO::Rightwards:
            matrix::rotate_y(projMatrix, projMatrix, getPitch());
            break;
        case NO::Downwards:
            matrix::rotate_x(projMatrix, projMatrix, -getPitch());
            break;
        case NO::Leftwards:
            matrix::rotate_y(projMatrix, projMatrix, -getPitch());
            break;
        default:
            matrix::rotate_x(projMatrix, projMatrix, getPitch());
            break;
    }

    matrix::rotate_z(projMatrix, projMatrix, getBearing() + getNorthOrientationAngle());

    const double dx = pixel_x() - size.width / 2.0f, dy = pixel_y() - size.height / 2.0f;
    matrix::translate(projMatrix, projMatrix, dx, dy, 0);

    if (axonometric) {
        // mat[11] controls perspective
        projMatrix[11] = 0;

        // mat[8], mat[9] control x-skew, y-skew
        projMatrix[8] = xSkew;
        projMatrix[9] = ySkew;
    }

    matrix::scale(projMatrix, projMatrix, 1, 1,
                  1.0 / Projection::getMetersPerPixelAtLatitude(getLatLng(LatLng::Unwrapped).latitude(), getZoom()));

    // Make a second projection matrix that is aligned to a pixel grid for rendering raster tiles.
    // We're rounding the (floating point) x/y values to achieve to avoid rendering raster images to fractional
    // coordinates. Additionally, we adjust by half a pixel in either direction in case that viewport dimension
    // is an odd integer to preserve rendering to the pixel grid. We're rotating this shift based on the angle
    // of the transformation so that 0°, 90°, 180°, and 270° rasters are crisp, and adjust the shift so that
    // it is always <= 0.5 pixels.
    if (aligned) {
        const float xShift = float(size.width % 2) / 2, yShift = float(size.height % 2) / 2;
        const double bearingCos = std::cos(bearing), bearingSin = std::sin(bearing);
        double devNull;
        const float dxa = -std::modf(dx, &devNull) + bearingCos * xShift + bearingSin * yShift;
        const float dya = -std::modf(dy, &devNull) + bearingCos * yShift + bearingSin * xShift;
        matrix::translate(projMatrix, projMatrix, dxa > 0.5 ? dxa - 1 : dxa, dya > 0.5 ? dya - 1 : dya, 0);
    }
}

void TransformState::updateMatricesIfNeeded() const {
    if (!needsMatricesUpdate() || size.isEmpty()) return;

    getProjMatrix(projectionMatrix);
    coordMatrix = coordinatePointMatrix(projectionMatrix);

    bool err = matrix::invert(invertedMatrix, coordMatrix);

    if (err) throw std::runtime_error("failed to invert coordinatePointMatrix");
    requestMatricesUpdate = false;
}

const mat4& TransformState::getProjectionMatrix() const {
    updateMatricesIfNeeded();
    return projectionMatrix;
}

const mat4& TransformState::getCoordMatrix() const {
    updateMatricesIfNeeded();
    return coordMatrix;
}

const mat4& TransformState::getInvertedMatrix() const {
    updateMatricesIfNeeded();
    return invertedMatrix;
}

#pragma mark - Dimensions

Size TransformState::getSize() const {
    return size;
}

void TransformState::setSize(const Size& size_) {
    if (size != size_) {
        size = size_;
        requestMatricesUpdate = true;
    }
}

#pragma mark - North Orientation

NorthOrientation TransformState::getNorthOrientation() const {
    return orientation;
}

void TransformState::setNorthOrientation(const NorthOrientation val) {
    if (orientation != val) {
        orientation = val;
        requestMatricesUpdate = true;
    }
}

double TransformState::getNorthOrientationAngle() const {
    double angleOrientation = 0;
    if (orientation == NorthOrientation::Rightwards) {
        angleOrientation += M_PI / 2.0f;
    } else if (orientation == NorthOrientation::Downwards) {
        angleOrientation += M_PI;
    } else if (orientation == NorthOrientation::Leftwards) {
        angleOrientation -= M_PI / 2.0f;
    }
    return angleOrientation;
}

#pragma mark - Constrain mode

ConstrainMode TransformState::getConstrainMode() const {
    return constrainMode;
}

void TransformState::setConstrainMode(const ConstrainMode val) {
    if (constrainMode != val) {
        constrainMode = val;
        requestMatricesUpdate = true;
    }
}

#pragma mark - ViewportMode

ViewportMode TransformState::getViewportMode() const {
    return viewportMode;
}

void TransformState::setViewportMode(ViewportMode val) {
    if (viewportMode != val) {
        viewportMode = val;
        requestMatricesUpdate = true;
    }
}

#pragma mark - Camera options

CameraOptions TransformState::getCameraOptions(optional<EdgeInsets> padding) const {
    return CameraOptions()
        .withCenter(getLatLng())
        .withPadding(padding ? padding : edgeInsets)
        .withZoom(getZoom())
        .withBearing(-bearing * util::RAD2DEG)
        .withPitch(pitch * util::RAD2DEG);
}

#pragma mark - EdgeInsets

void TransformState::setEdgeInsets(const EdgeInsets& val) {
    if (edgeInsets != val) {
        edgeInsets = val;
        requestMatricesUpdate = true;
    }
}

#pragma mark - Position

LatLng TransformState::getLatLng(LatLng::WrapMode wrapMode) const {
    return {util::RAD2DEG * (2 * std::atan(std::exp(y / Cc)) - 0.5 * M_PI), -x / Bc, wrapMode};
}

double TransformState::pixel_x() const {
    const double center = (size.width - Projection::worldSize(scale)) / 2;
    return center + x;
}

double TransformState::pixel_y() const {
    const double center = (size.height - Projection::worldSize(scale)) / 2;
    return center + y;
}

#pragma mark - Zoom

double TransformState::getZoom() const {
    return scaleZoom(scale);
}

uint8_t TransformState::getIntegerZoom() const {
    return getZoom();
}

double TransformState::getZoomFraction() const {
    return getZoom() - getIntegerZoom();
}

#pragma mark - Bounds

void TransformState::setLatLngBounds(LatLngBounds bounds_) {
    if (bounds_ != bounds) {
        bounds = bounds_;
        setLatLngZoom(getLatLng(LatLng::Unwrapped), getZoom());
    }
}

LatLngBounds TransformState::getLatLngBounds() const {
    return bounds;
}

void TransformState::setMinZoom(const double minZoom) {
    if (minZoom <= getMaxZoom()) {
        min_scale = zoomScale(util::clamp(minZoom, util::MIN_ZOOM, util::MAX_ZOOM));
    }
}

double TransformState::getMinZoom() const {
    double test_scale = min_scale;
    double unused_x = x;
    double unused_y = y;
    constrain(test_scale, unused_x, unused_y);

    return scaleZoom(test_scale);
}

void TransformState::setMaxZoom(const double maxZoom) {
    if (maxZoom >= getMinZoom()) {
        max_scale = zoomScale(util::clamp(maxZoom, util::MIN_ZOOM, util::MAX_ZOOM));
    }
}

double TransformState::getMaxZoom() const {
    return scaleZoom(max_scale);
}

#pragma mark - Scale
double TransformState::getScale() const {
    return scale;
}

void TransformState::setScale(double val) {
    if (scale != val) {
        scale = val;
        requestMatricesUpdate = true;
    }
}

#pragma mark - Positions

double TransformState::getX() const {
    return x;
}

void TransformState::setX(double val) {
    if (x != val) {
        x = val;
        requestMatricesUpdate = true;
    }
}

double TransformState::getY() const {
    return y;
}

void TransformState::setY(double val) {
    if (y != val) {
        y = val;
        requestMatricesUpdate = true;
    }
}

#pragma mark - Rotation

double TransformState::getBearing() const {
    return bearing;
}

void TransformState::setBearing(double val) {
    if (bearing != val) {
        bearing = val;
        requestMatricesUpdate = true;
    }
}

float TransformState::getFieldOfView() const {
    return fov;
}

float TransformState::getCameraToCenterDistance() const {
    return 0.5 * size.height / std::tan(fov / 2.0);
}

double TransformState::getPitch() const {
    return pitch;
}

void TransformState::setPitch(double val) {
    if (pitch != val) {
        pitch = val;
        requestMatricesUpdate = true;
    }
}

double TransformState::getXSkew() const {
    return xSkew;
}

void TransformState::setXSkew(double val) {
    if (xSkew != val) {
        xSkew = val;
        requestMatricesUpdate = true;
    }
}
double TransformState::getYSkew() const {
    return ySkew;
}

void TransformState::setYSkew(double val) {
    if (ySkew != val) {
        ySkew = val;
        requestMatricesUpdate = true;
    }
}

bool TransformState::getAxonometric() const {
    return axonometric;
}

void TransformState::setAxonometric(bool val) {
    if (axonometric != val) {
        axonometric = val;
        requestMatricesUpdate = true;
    }
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

double TransformState::zoomScale(double zoom) const {
    return std::pow(2.0, zoom);
}

double TransformState::scaleZoom(double s) const {
    return util::log2(s);
}

ScreenCoordinate TransformState::latLngToScreenCoordinate(const LatLng& latLng) const {
    if (size.isEmpty()) {
        return {};
    }

    vec4 p;
    Point<double> pt = Projection::project(latLng, scale) / util::tileSize;
    vec4 c = {{pt.x, pt.y, 0, 1}};
    matrix::transformMat4(p, c, getCoordMatrix());
    return {p[0] / p[3], size.height - p[1] / p[3]};
}

TileCoordinate TransformState::screenCoordinateToTileCoordinate(const ScreenCoordinate& point, uint8_t atZoom) const {
    if (size.isEmpty()) {
        return {{}, 0};
    }

    float targetZ = 0;

    double flippedY = size.height - point.y;

    // since we don't know the correct projected z value for the point,
    // unproject two points to get a line and then find the point on that
    // line with z=0

    vec4 coord0;
    vec4 coord1;
    vec4 point0 = {{point.x, flippedY, 0, 1}};
    vec4 point1 = {{point.x, flippedY, 1, 1}};
    matrix::transformMat4(coord0, point0, getInvertedMatrix());
    matrix::transformMat4(coord1, point1, getInvertedMatrix());

    double w0 = coord0[3];
    double w1 = coord1[3];

    Point<double> p0 = Point<double>(coord0[0], coord0[1]) / w0;
    Point<double> p1 = Point<double>(coord1[0], coord1[1]) / w1;

    double z0 = coord0[2] / w0;
    double z1 = coord1[2] / w1;
    double t = z0 == z1 ? 0 : (targetZ - z0) / (z1 - z0);

    Point<double> p = util::interpolate(p0, p1, t) / scale * static_cast<double>(1 << atZoom);
    return {{p.x, p.y}, static_cast<double>(atZoom)};
}

LatLng TransformState::screenCoordinateToLatLng(const ScreenCoordinate& point, LatLng::WrapMode wrapMode) const {
    auto coord = screenCoordinateToTileCoordinate(point, 0);
    return Projection::unproject(coord.p, 1 / util::tileSize, wrapMode);
}

mat4 TransformState::coordinatePointMatrix(const mat4& projMatrix) const {
    mat4 proj = projMatrix;
    matrix::scale(proj, proj, util::tileSize, util::tileSize, 1);
    matrix::multiply(proj, getPixelMatrix(), proj);
    return proj;
}

mat4 TransformState::getPixelMatrix() const {
    mat4 m;
    matrix::identity(m);
    matrix::scale(m, m, static_cast<double>(size.width) / 2, -static_cast<double>(size.height) / 2, 1);
    matrix::translate(m, m, 1, -1, 0);
    return m;
}

#pragma mark - (private helper functions)

bool TransformState::rotatedNorth() const {
    using NO = NorthOrientation;
    return (orientation == NO::Leftwards || orientation == NO::Rightwards);
}

void TransformState::constrain(double& scale_, double& x_, double& y_) const {
    if (constrainMode == ConstrainMode::None) {
        return;
    }

    // Constrain scale to avoid zooming out far enough to show off-world areas on the Y axis.
    const double ratioY = (rotatedNorth() ? size.width : size.height) / util::tileSize;
    scale_ = util::max(scale_, ratioY);

    // Constrain min/max pan to avoid showing off-world areas on the Y axis.
    double max_y = (scale_ * util::tileSize - (rotatedNorth() ? size.width : size.height)) / 2;
    y_ = std::max(-max_y, std::min(y_, max_y));

    if (constrainMode == ConstrainMode::WidthAndHeight) {
        // Constrain min/max pan to avoid showing off-world areas on the X axis.
        double max_x = (scale_ * util::tileSize - (rotatedNorth() ? size.height : size.width)) / 2;
        x_ = std::max(-max_x, std::min(x_, max_x));
    }
}

ScreenCoordinate TransformState::getCenterOffset() const {
    return {0.5 * (edgeInsets.left() - edgeInsets.right()), 0.5 * (edgeInsets.top() - edgeInsets.bottom())};
}

void TransformState::moveLatLng(const LatLng& latLng, const ScreenCoordinate& anchor) {
    auto centerCoord = Projection::project(getLatLng(LatLng::Unwrapped), scale);
    auto latLngCoord = Projection::project(latLng, scale);
    auto anchorCoord = Projection::project(screenCoordinateToLatLng(anchor), scale);
    setLatLngZoom(Projection::unproject(centerCoord + latLngCoord - anchorCoord, scale), getZoom());
}

void TransformState::setLatLngZoom(const LatLng& latLng, double zoom) {
    LatLng constrained = latLng;
    constrained = bounds.constrain(latLng);

    double newScale = util::clamp(zoomScale(zoom), min_scale, max_scale);
    const double newWorldSize = newScale * util::tileSize;
    Bc = newWorldSize / util::DEGREES_MAX;
    Cc = newWorldSize / util::M2PI;

    const double m = 1 - 1e-15;
    const double f = util::clamp(std::sin(util::DEG2RAD * constrained.latitude()), -m, m);

    ScreenCoordinate point = {
        -constrained.longitude() * Bc,
        0.5 * Cc * std::log((1 + f) / (1 - f)),
    };
    setScalePoint(newScale, point);
}

void TransformState::setScalePoint(const double newScale, const ScreenCoordinate& point) {
    double constrainedScale = newScale;
    ScreenCoordinate constrainedPoint = point;
    constrain(constrainedScale, constrainedPoint.x, constrainedPoint.y);

    scale = constrainedScale;
    x = constrainedPoint.x;
    y = constrainedPoint.y;
    Bc = Projection::worldSize(scale) / util::DEGREES_MAX;
    Cc = Projection::worldSize(scale) / util::M2PI;
    requestMatricesUpdate = true;
}

float TransformState::getCameraToTileDistance(const UnwrappedTileID& tileID) const {
    mat4 tileProjectionMatrix;
    matrixFor(tileProjectionMatrix, tileID);
    matrix::multiply(tileProjectionMatrix, getProjectionMatrix(), tileProjectionMatrix);
    vec4 tileCenter = {{util::tileSize / 2, util::tileSize / 2, 0, 1}};
    vec4 projectedCenter;
    matrix::transformMat4(projectedCenter, tileCenter, tileProjectionMatrix);
    return projectedCenter[3];
}

float TransformState::maxPitchScaleFactor() const {
    if (size.isEmpty()) {
        return {};
    }
    auto latLng = screenCoordinateToLatLng({ 0, static_cast<float>(getSize().height) });

    Point<double> pt = Projection::project(latLng, scale) / util::tileSize;
    vec4 p = {{ pt.x, pt.y, 0, 1 }};
    vec4 topPoint;
    matrix::transformMat4(topPoint, p, getCoordMatrix());
    return topPoint[3] / getCameraToCenterDistance();
}

} // namespace mbgl
