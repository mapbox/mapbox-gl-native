#import "CustomStyleExample.h"
@import Mapbox;

NSString *const MBXExampleCustomStyle = @"CustomStyleExample";

@implementation CustomStyleExample

- (void)viewDidLoad {
    [super viewDidLoad];

    // Fill in the next line with your style URL from Mapbox Studio.
    // <#mapbox://styles/userName/styleHash#>
    NSURL *styleURL = [NSURL URLWithString:@"mapbox://styles/mapbox/outdoors-v9"];
    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds
                                                   styleURL:styleURL];

    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    // Set the map’s center coordinate and zoom level.
    [mapView setCenterCoordinate:CLLocationCoordinate2DMake(45.52954, -122.72317)
                       zoomLevel:14
                        animated:NO];

    [self.view addSubview:mapView];
}

@end
