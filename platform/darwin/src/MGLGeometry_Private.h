#import "MGLGeometry.h"

#import <TargetConditionals.h>
#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#endif

#import <mbgl/util/geo.hpp>
#import <mbgl/util/geometry.hpp>

/// Returns the smallest rectangle that contains both the given rectangle and
/// the given point.
CGRect MGLExtendRect(CGRect rect, CGPoint point);

NS_INLINE mbgl::LatLng MGLLatLngFromLocationCoordinate2D(CLLocationCoordinate2D coordinate) {
    return mbgl::LatLng(coordinate.latitude, coordinate.longitude);
}

NS_INLINE mbgl::Point<double> MGLPointFromLocationCoordinate2D(CLLocationCoordinate2D coordinate) {
    return mbgl::Point<double>(coordinate.longitude, coordinate.latitude);
}

NS_INLINE CLLocationCoordinate2D MGLLocationCoordinate2DFromLatLng(mbgl::LatLng latLng) {
    return CLLocationCoordinate2DMake(latLng.latitude, latLng.longitude);
}

NS_INLINE MGLCoordinateBounds MGLCoordinateBoundsFromLatLngBounds(mbgl::LatLngBounds latLngBounds) {
    return MGLCoordinateBoundsMake(MGLLocationCoordinate2DFromLatLng(latLngBounds.southwest()),
                                   MGLLocationCoordinate2DFromLatLng(latLngBounds.northeast()));
}

NS_INLINE mbgl::LatLngBounds MGLLatLngBoundsFromCoordinateBounds(MGLCoordinateBounds coordinateBounds) {
    return mbgl::LatLngBounds::hull(MGLLatLngFromLocationCoordinate2D(coordinateBounds.sw),
                                    MGLLatLngFromLocationCoordinate2D(coordinateBounds.ne));
}

#if TARGET_OS_IPHONE
NS_INLINE mbgl::EdgeInsets MGLEdgeInsetsFromNSEdgeInsets(UIEdgeInsets insets) {
    return { insets.top, insets.left, insets.bottom, insets.right };
}
#else
NS_INLINE mbgl::EdgeInsets MGLEdgeInsetsFromNSEdgeInsets(NSEdgeInsets insets) {
    return { insets.top, insets.left, insets.bottom, insets.right };
}
#endif

/** Converts a map zoom level to a camera altitude.
    
    @param zoomLevel The zoom level to convert.
    @param pitch The camera pitch, measured in degrees.
    @param latitude The latitude of the point at the center of the viewport.
    @param size The size of the viewport.
    @return An altitude measured in meters. */
CLLocationDistance MGLAltitudeForZoomLevel(double zoomLevel, CGFloat pitch, CLLocationDegrees latitude, CGSize size);

/** Converts a camera altitude to a map zoom level.
    
    @param altitude The altitude to convert, measured in meters.
    @param pitch The camera pitch, measured in degrees.
    @param latitude The latitude of the point at the center of the viewport.
    @param size The size of the viewport.
    @return A zero-based zoom level. */
double MGLZoomLevelForAltitude(CLLocationDistance altitude, CGFloat pitch, CLLocationDegrees latitude, CGSize size);
