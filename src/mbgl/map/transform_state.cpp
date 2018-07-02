#include <mbgl/map/transform_state.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {

TransformState::TransformState(ConstrainMode constrainMode_, ViewportMode viewportMode_)
    : constrainMode(constrainMode_)
    , viewportMode(viewportMode_)
{
}

#pragma mark - Matrix

void TransformState::matrixFor(mat4& matrix, const UnwrappedTileID& tileID) const {
    const uint64_t tileScale = 1ull << tileID.canonical.z;
    const double s = Projection::worldSize(scale) / tileScale;

    matrix::identity(matrix);
    matrix::translate(matrix, matrix,
                      int64_t(tileID.canonical.x + tileID.wrap * tileScale) * s,
                      int64_t(tileID.canonical.y) * s, 0);
    matrix::scale(matrix, matrix, s / util::EXTENT, s / util::EXTENT, 1);
}

void TransformState::getProjMatrix(mat4& projMatrix, uint16_t nearZ, bool aligned) const {
    if (size.isEmpty()) {
        return;
    }

    const double cameraToCenterDistance = getCameraToCenterDistance();

     // Find the distance from the center point [width/2, height/2] to the
    // center top point [width/2, 0] in Z units, using the law of sines.
    // 1 Z unit is equivalent to 1 horizontal px at the center of the map
    // (the distance between[width/2, height/2] and [width/2 + 1, height/2])
    const double halfFov = getFieldOfView() / 2.0;
    const double groundAngle = M_PI / 2.0 + getPitch();
    const double topHalfSurfaceDistance = std::sin(halfFov) * cameraToCenterDistance / std::sin(M_PI - groundAngle - halfFov);

    // Calculate z distance of the farthest fragment that should be rendered.
    const double furthestDistance = std::cos(M_PI / 2 - getPitch()) * topHalfSurfaceDistance + cameraToCenterDistance;
    // Add a bit extra to avoid precision problems when a fragment's distance is exactly `furthestDistance`
    const double farZ = furthestDistance * 1.01;

    const double aspect = double(size.width) / size.height;
    matrix::perspective(projMatrix, getFieldOfView(), aspect, nearZ, farZ);

    const bool flippedY = viewportMode == ViewportMode::FlippedY;
    matrix::scale(projMatrix, projMatrix, 1, flippedY ? 1 : -1, 1);

    matrix::translate(projMatrix, projMatrix, 0, 0, -cameraToCenterDistance);

    using NO = NorthOrientation;
    switch (getNorthOrientation()) {
        case NO::Rightwards: matrix::rotate_y(projMatrix, projMatrix, getPitch()); break;
        case NO::Downwards: matrix::rotate_x(projMatrix, projMatrix, -getPitch()); break;
        case NO::Leftwards: matrix::rotate_y(projMatrix, projMatrix, -getPitch()); break;
        default: matrix::rotate_x(projMatrix, projMatrix, getPitch()); break;
    }

    matrix::rotate_z(projMatrix, projMatrix, getBearing() + getNorthOrientationAngle());

    const double worldSize = Projection::worldSize(scale);
    Point<double> pixel = {
        position.x - ((size.width / 2.0) / size.width * worldSize),
        position.y - ((size.height / 2.0) / size.height * worldSize)
    };
    matrix::translate(projMatrix, projMatrix, pixel.x, pixel.y, 0);

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
        const float dxa = -std::modf(pixel.x, &devNull) + bearingCos * xShift + bearingSin * yShift;
        const float dya = -std::modf(pixel.y, &devNull) + bearingCos * yShift + bearingSin * xShift;
        matrix::translate(projMatrix, projMatrix, dxa > 0.5 ? dxa - 1 : dxa, dya > 0.5 ? dya - 1 : dya, 0);
    }
}

#pragma mark - Dimensions

Size TransformState::getSize() const {
    return size;
}

void TransformState::setSize(const Size& size_) {
    size = size_;
    constrainToViewport();
}

#pragma mark - North Orientation

NorthOrientation TransformState::getNorthOrientation() const {
    return orientation;
}

void TransformState::setNorthOrientation(NorthOrientation orientation_) {
    orientation = orientation_;
    constrainToViewport();
}

double TransformState::getNorthOrientationAngle() const {
    double angleOrientation = 0;
    if (orientation == NorthOrientation::Rightwards) {
        angleOrientation += M_PI / 2.0;
    } else if (orientation == NorthOrientation::Downwards) {
        angleOrientation += M_PI;
    } else if (orientation == NorthOrientation::Leftwards) {
        angleOrientation -= M_PI / 2.0;
    }
    return angleOrientation;
}

#pragma mark - Constrain mode

ConstrainMode TransformState::getConstrainMode() const {
    return constrainMode;
}

void TransformState::setConstrainMode(ConstrainMode constrainMode_) {
    constrainMode = constrainMode_;
    constrainToViewport();
}

#pragma mark - ViewportMode

ViewportMode TransformState::getViewportMode() const {
    return viewportMode;
}

void TransformState::setViewportMode(ViewportMode viewportMode_) {
    viewportMode = viewportMode_;
}

#pragma mark - Projection

void TransformState::setAxonometric(bool axonometric_) {
    axonometric = axonometric_;
}

bool TransformState::getAxonometric() const {
    return axonometric;
}

void TransformState::setXSkew(double xSkew_) {
    xSkew = xSkew_;
}

double TransformState::getXSkew() const {
    return xSkew;
}

void TransformState::setYSkew(double ySkew_) {
    ySkew = ySkew_;
}

double TransformState::getYSkew() const {
    return ySkew;
}

#pragma mark - Camera options

CameraOptions TransformState::getCameraOptions(const EdgeInsets& padding) const {
    LatLng center;
    if (padding.isFlush()) {
        center = getLatLng();
    } else {
        ScreenCoordinate point = padding.getCenter(size.width, size.height);
        point.y = size.height - point.y;
        center = screenCoordinateToLatLng(point).wrapped();
    }
    return CameraOptions()
        .withCenter(center)
        .withPadding(padding)
        .withZoom(getZoom())
        .withBearing(-bearing * util::RAD2DEG)
        .withPitch(pitch * util::RAD2DEG);
}

#pragma mark - Position

LatLng TransformState::getLatLng(LatLng::WrapMode wrapMode) const {
    return {
        util::RAD2DEG * (2 * std::atan(std::exp(position.y / Cc)) - 0.5 * M_PI),
        -position.x / Bc,
        wrapMode
    };
}

#pragma mark - Zoom

double TransformState::getZoom() const {
    return scaleZoom(scale);
}

double TransformState::getScale() const {
    return scale;
}

uint8_t TransformState::getIntegerZoom() const {
    return getZoom();
}

#pragma mark - Bounds

void TransformState::setLatLngBounds(optional<LatLngBounds> bounds_) {
    if (bounds_ != bounds) {
        bounds = bounds_;
        setLatLngZoom(getLatLng(LatLng::Unwrapped), getZoom());
    }
}

optional<LatLngBounds> TransformState::getLatLngBounds() const {
    return bounds;
}

void TransformState::setMinZoom(const double minZoom) {
    if (minZoom <= getMaxZoom()) {
        min_scale = zoomScale(util::clamp(minZoom, util::MIN_ZOOM, util::MAX_ZOOM));
    }
}

double TransformState::getMinZoom() const {
    return scaleZoom(minimumScaleAtCurrentSize());
}

void TransformState::setMaxZoom(const double maxZoom) {
    if (maxZoom >= getMinZoom()) {
        max_scale = zoomScale(util::clamp(maxZoom, util::MIN_ZOOM, util::MAX_ZOOM));
    }
}

double TransformState::getMaxZoom() const {
    return scaleZoom(max_scale);
}

void TransformState::setMinPitch(double minPitch) {
    if (minPitch <= getMaxPitch()) {
        min_pitch = minPitch;
    }
}

double TransformState::getMinPitch() const {
    return min_pitch;
}

void TransformState::setMaxPitch(double maxPitch) {
    if (maxPitch >= getMinPitch()) {
        max_pitch = maxPitch;
    }
}

double TransformState::getMaxPitch() const {
    return max_pitch;
}

#pragma mark - Rotation

double TransformState::getBearing() const {
    return bearing;
}

void TransformState::setBearing(double bearing_) {
    bearing = bearing_;
}

double TransformState::getFieldOfView() const {
    return fov;
}

double TransformState::getCameraToCenterDistance() const {
    return 0.5 * size.height / std::tan(fov / 2.0);
}

double TransformState::getPitch() const {
    return pitch;
}

void TransformState::setPitch(double pitch_) {
    pitch = pitch_;
}

#pragma mark - State

bool TransformState::isChanging() const {
    return gestureInProgress || transitionInProgress;
}

bool TransformState::isGestureInProgress() const {
    return gestureInProgress;
}

void TransformState::setGestureInProgress(bool gestureInProgress_) {
    gestureInProgress = gestureInProgress_;
}

bool TransformState::isTransitionInProgress() const {
    return transitionInProgress;
}

void TransformState::setTransitionInProgress(bool transitionInProgress_) {
    transitionInProgress = transitionInProgress_;
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

    mat4 mat = coordinatePointMatrix(getZoom());
    vec4 p;
    Point<double> pt = Projection::project(latLng, scale) / util::tileSize;
    vec4 c = {{ pt.x, pt.y, 0, 1 }};
    matrix::transformMat4(p, c, mat);
    return { p[0] / p[3], size.height - p[1] / p[3] };
}

Point<double> TransformState::screenCoordinateToMapPosition(const ScreenCoordinate& point) const {
    if (size.isEmpty()) {
        return {};
    }

    mat4 mat = coordinatePointMatrix(getZoom());

    mat4 inverted;
    bool err = matrix::invert(inverted, mat);

    if (err) throw std::runtime_error("failed to invert coordinatePointMatrix");

    double flippedY = size.height - point.y;

    // since we don't know the correct projected z value for the point,
    // unproject two points to get a line and then find the point on that
    // line with z=0

    vec4 coord0;
    vec4 coord1;
    vec4 point0 = {{ point.x, flippedY, 0, 1 }};
    vec4 point1 = {{ point.x, flippedY, 1, 1 }};
    matrix::transformMat4(coord0, point0, inverted);
    matrix::transformMat4(coord1, point1, inverted);

    double w0 = coord0[3];
    double w1 = coord1[3];

    Point<double> p0 = Point<double>(coord0[0], coord0[1]) / w0;
    Point<double> p1 = Point<double>(coord1[0], coord1[1]) / w1;

    double z0 = coord0[2] / w0;
    double z1 = coord1[2] / w1;
    double t = z0 == z1 ? 0 : -z0 / (z1 - z0);

    return util::interpolate(p0, p1, t);
}

LatLng TransformState::screenCoordinateToLatLng(const ScreenCoordinate& point, LatLng::WrapMode wrapMode) const {
    return Projection::unproject(screenCoordinateToMapPosition(point), scale / util::tileSize, wrapMode);
}

mat4 TransformState::coordinatePointMatrix(double z) const {
    mat4 proj;
    getProjMatrix(proj);
    float s = Projection::worldSize(scale) / std::pow(2, z);
    matrix::scale(proj, proj, s, s, 1);
    matrix::multiply(proj, getPixelMatrix(), proj);
    return proj;
}

mat4 TransformState::getPixelMatrix() const {
    mat4 m;
    matrix::identity(m);
    matrix::scale(m, m,
                  static_cast<double>(size.width) / 2, -static_cast<double>(size.height) / 2, 1);
    matrix::translate(m, m, 1, -1, 0);
    return m;
}


#pragma mark - (private helper functions)

bool TransformState::rotatedNorth() const {
    using NO = NorthOrientation;
    return (orientation == NO::Leftwards || orientation == NO::Rightwards);
}

double TransformState::minimumScaleAtCurrentSize() const {
    return util::max(min_scale, static_cast<double>(rotatedNorth() ? size.width : size.height) / util::tileSize);
}

void TransformState::constrainToViewport() {
    if (constrainMode == ConstrainMode::None) {
        return;
    }

    // Constrain scale to avoid zooming out far enough to show off-world areas on the Y axis.
    scale = util::max(scale, minimumScaleAtCurrentSize());

    const bool rotated = rotatedNorth();
    const double worldSize = Projection::worldSize(scale);

    // Constrain min/max pan to avoid showing off-world areas on the Y axis.
    double max_y = (worldSize - (rotated ? size.width : size.height)) / 2.0;
    position.y = std::max(-max_y, std::min(position.y, max_y));

    Bc = worldSize / util::DEGREES_MAX;
    Cc = worldSize / util::M2PI;

    if (constrainMode == ConstrainMode::WidthAndHeight) {
        // Constrain min/max pan to avoid showing off-world areas on the X axis.
        double max_x = (worldSize - (rotated ? size.height : size.width)) / 2.0;
        position.x = std::max(-max_x, std::min(position.x, max_x));
    }
}

void TransformState::moveLatLng(const LatLng& latLng, const ScreenCoordinate& anchor) {
    auto centerCoord = Projection::project(getLatLng(LatLng::Unwrapped), scale);
    auto latLngCoord = Projection::project(latLng, scale);
    auto anchorCoord = Projection::project(screenCoordinateToLatLng(anchor), scale);
    setLatLngZoom(Projection::unproject(centerCoord + latLngCoord - anchorCoord, scale), getZoom());
}

void TransformState::setLatLngZoom(const LatLng& latLng, double zoom) {
    const LatLng constrained = bounds ? bounds->constrain(latLng) : latLng;

    constexpr double m = 1 - 1e-15;
    const double f = util::clamp(std::sin(util::DEG2RAD * constrained.latitude()), -m, m);

    scale = util::clamp(zoomScale(zoom), min_scale, max_scale);
    Bc = Projection::worldSize(scale) / util::DEGREES_MAX;
    Cc = Projection::worldSize(scale) / util::M2PI;

    position.x = -constrained.longitude() * Bc;
    position.y = 0.5 * Cc * std::log((1 + f) / (1 - f));

    constrainToViewport();
}

float TransformState::getCameraToTileDistance(const UnwrappedTileID& tileID) const {
    mat4 projectionMatrix;
    getProjMatrix(projectionMatrix);
    mat4 tileProjectionMatrix;
    matrixFor(tileProjectionMatrix, tileID);
    matrix::multiply(tileProjectionMatrix, projectionMatrix, tileProjectionMatrix);
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
    mat4 mat = coordinatePointMatrix(getZoom());
    Point<double> pt = Projection::project(latLng, scale) / util::tileSize;
    vec4 p = {{ pt.x, pt.y, 0, 1 }};
    vec4 topPoint;
    matrix::transformMat4(topPoint, p, mat);
    return topPoint[3] / getCameraToCenterDistance();
}

} // namespace mbgl
