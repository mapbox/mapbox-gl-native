#import "MGLGeometry.h"

#import <TargetConditionals.h>

#import <mbgl/map/map.hpp>
#import <mbgl/util/geo.hpp>

NS_INLINE mbgl::LatLng MGLLatLngFromLocationCoordinate2D(CLLocationCoordinate2D coordinate) {
    return mbgl::LatLng(coordinate.latitude, coordinate.longitude);
}

NS_INLINE CLLocationCoordinate2D MGLLocationCoordinate2DFromLatLng(mbgl::LatLng latLng) {
    return CLLocationCoordinate2DMake(latLng.latitude, latLng.longitude);
}

NS_INLINE MGLCoordinateBounds MGLCoordinateBoundsFromLatLngBounds(mbgl::LatLngBounds latLngBounds) {
    return MGLCoordinateBoundsMake(MGLLocationCoordinate2DFromLatLng(latLngBounds.sw),
                                   MGLLocationCoordinate2DFromLatLng(latLngBounds.ne));
}

NS_INLINE mbgl::LatLngBounds MGLLatLngBoundsFromCoordinateBounds(MGLCoordinateBounds coordinateBounds) {
    return mbgl::LatLngBounds(MGLLatLngFromLocationCoordinate2D(coordinateBounds.sw),
                              MGLLatLngFromLocationCoordinate2D(coordinateBounds.ne));
}

NS_INLINE BOOL MGLCoordinateInCoordinateBounds(CLLocationCoordinate2D coordinate, MGLCoordinateBounds coordinateBounds) {
    mbgl::LatLngBounds bounds = MGLLatLngBoundsFromCoordinateBounds(coordinateBounds);
    return bounds.contains(MGLLatLngFromLocationCoordinate2D(coordinate));
}

#if TARGET_OS_MAC
NS_INLINE mbgl::EdgeInsets MGLEdgeInsetsFromNSEdgeInsets(NSEdgeInsets insets) {
    return { insets.top, insets.left, insets.bottom, insets.right };
}
#elif TARGET_OS_IOS
NS_INLINE mbgl::EdgeInsets MGLEdgeInsetsFromNSEdgeInsets(UIEdgeInsets insets) {
    return { insets.top, insets.left, insets.bottom, insets.right };
}
#endif
