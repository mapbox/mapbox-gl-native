#import "UserTrackingModesExample.h"
@import Mapbox;

NSString *const MBXExampleUserTrackingModes = @"UserTrackingModesExample";

@implementation UserTrackingModesExample

- (void)viewDidLoad {
    [super viewDidLoad];

    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:[MGLStyle darkStyleURLWithVersion:9]];
    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    mapView.userTrackingMode = MGLUserTrackingModeFollowWithHeading;

    // The user location annotation takes its color from the map view tint.
    mapView.tintColor = [UIColor redColor];

    // You can set the attribution button tint separately.
    mapView.attributionButton.tintColor = [UIColor lightGrayColor];

    [self.view addSubview:mapView];
}

@end
