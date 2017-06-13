#import "SourceCustomVectorExample.h"
@import Mapbox;

NSString *const MBXExampleSourceCustomVector = @"SourceCustomVectorExample";

@implementation SourceCustomVectorExample

- (void)viewDidLoad {
    [super viewDidLoad];

    // Third party vector tile sources can be added.
    
    // In this case we're using custom style JSON (https://www.mapbox.com/mapbox-gl-style-spec/) to add a third party tile source: <https://vector.mapzen.com/osm/all/{z}/{x}/{y}.mvt>
    NSURL *customStyleURL = [[NSBundle mainBundle] URLForResource:@"third_party_vector_style" withExtension:@"json"];

    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:customStyleURL];

    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    mapView.tintColor = [UIColor whiteColor];

    [self.view addSubview:mapView];
}

@end
