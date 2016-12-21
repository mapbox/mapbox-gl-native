#import "MGLGeometry_Private.h"

#import <mbgl/util/projection.hpp>

/** Vertical field of view, measured in degrees, for determining the altitude
    of the viewpoint.
    
    TransformState::getProjMatrix() has a variable vertical field of view that
    defaults to 2 arctan ⅓ rad ≈ 36.9° but MapKit uses a vertical field of view of 30°.
    flyTo() assumes a field of view of 2 arctan ½ rad. */
const CLLocationDegrees MGLAngularFieldOfView = 30;

const MGLCoordinateSpan MGLCoordinateSpanZero = {0, 0};

CGRect MGLExtendRect(CGRect rect, CGPoint point) {
    if (point.x < rect.origin.x) {
        rect.size.width += rect.origin.x - point.x;
        rect.origin.x = point.x;
    }
    if (point.x > rect.origin.x + rect.size.width) {
        rect.size.width += point.x - (rect.origin.x + rect.size.width);
    }
    if (point.y < rect.origin.y) {
        rect.size.height += rect.origin.y - point.y;
        rect.origin.y = point.y;
    }
    if (point.y > rect.origin.y + rect.size.height) {
        rect.size.height += point.y - (rect.origin.y + rect.size.height);
    }
    return rect;
}

CLLocationDistance MGLAltitudeForZoomLevel(double zoomLevel, CGFloat pitch, CLLocationDegrees latitude, CGSize size) {
    CLLocationDistance metersPerPixel = mbgl::Projection::getMetersPerPixelAtLatitude(latitude, zoomLevel);
    CLLocationDistance metersTall = metersPerPixel * size.height;
    CLLocationDistance altitude = metersTall / 2 / std::tan(MGLRadiansFromDegrees(MGLAngularFieldOfView) / 2.);
    return altitude * std::sin(M_PI_2 - MGLRadiansFromDegrees(pitch)) / std::sin(M_PI_2);
}

double MGLZoomLevelForAltitude(CLLocationDistance altitude, CGFloat pitch, CLLocationDegrees latitude, CGSize size) {
    CLLocationDistance eyeAltitude = altitude / std::sin(M_PI_2 - MGLRadiansFromDegrees(pitch)) * std::sin(M_PI_2);
    CLLocationDistance metersTall = eyeAltitude * 2 * std::tan(MGLRadiansFromDegrees(MGLAngularFieldOfView) / 2.);
    CLLocationDistance metersPerPixel = metersTall / size.height;
    CGFloat mapPixelWidthAtZoom = std::cos(MGLRadiansFromDegrees(latitude)) * mbgl::util::M2PI * mbgl::util::EARTH_RADIUS_M / metersPerPixel;
    return ::log2(mapPixelWidthAtZoom / mbgl::util::tileSize);
}
