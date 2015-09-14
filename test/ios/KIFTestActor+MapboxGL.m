#import "KIFTestActor+MapboxGL.h"

#import "Mapbox.h"

#import <KIF/UIApplication-KIFAdditions.h>

@implementation KIFTestActor (MapboxGL)

- (UIWindow *)window {
    return [[UIApplication sharedApplication] statusBarWindow];
}

- (MGLTViewController *)viewController {
    return (MGLTViewController *)[[tester.mapView nextResponder] nextResponder];
}

- (MGLMapView *)mapView {
    return (MGLMapView *)[tester waitForViewWithAccessibilityLabel:@"Map"];
}

- (UIView *)compass {
    return [tester waitForViewWithAccessibilityLabel:@"Compass"];
}

@end
