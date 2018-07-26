#import "MGLTestingSupport.h"

NSString * const MGLTestingSupportMapViewID = @"MGLTestingMapViewID";

const MGLTestingSupportNotification MGLTestingSupportNotificationMapViewStyleLoaded = @"com.mapbox.examples.mapview-style-loaded";
const MGLTestingSupportNotification MGLTestingSupportNotificationMapViewRendered = @"com.mapbox.examples.mapview-rendered";
const MGLTestingSupportNotification MGLTestingSupportNotificationMapViewRegionWillChange = @"com.mapbox.examples.mapview-region-will-change";
const MGLTestingSupportNotification MGLTestingSupportNotificationMapViewRegionIsChanging = @"com.mapbox.examples.mapview-region-is-changing";
const MGLTestingSupportNotification MGLTestingSupportNotificationMapViewRegionDidChanged = @"com.mapbox.examples.mapview-region-did-changed";

void testingSupportPostNotification(MGLTestingSupportNotification name) {
    CFNotificationCenterRef center = CFNotificationCenterGetDarwinNotifyCenter();
    CFNotificationCenterPostNotification(center, (CFNotificationName)name, NULL, NULL, true);
}
