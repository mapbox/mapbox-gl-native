#import "KIFTestActor+MapboxGL.h"

#import "MapboxGL.h"

#import <KIF/UIApplication-KIFAdditions.h>

@implementation KIFTestActor (MapboxGL)

- (UIWindow *)window {
    return [[UIApplication sharedApplication] statusBarWindow];
}

- (UIViewController *)viewController {
    return (UIViewController *)[[tester.mapView nextResponder] nextResponder];
}

- (MGLMapView *)mapView {
    return (MGLMapView *)[tester waitForViewWithAccessibilityLabel:@"Map"];
}

- (UIView *)compass {
    return [tester waitForViewWithAccessibilityLabel:@"Compass"];
}

@end
