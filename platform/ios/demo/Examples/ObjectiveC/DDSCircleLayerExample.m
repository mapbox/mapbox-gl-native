#import "DDSCircleLayerExample.h"
@import Mapbox;

NSString *const MBXExampleDDSCircleLayer = @"DDSCircleLayerExample";

@interface DDSCircleLayerExample () <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;

@end

@implementation DDSCircleLayerExample

- (void)viewDidLoad {
    [super viewDidLoad];

    // Create a new map view using the Mapbox Light style.
    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds
        styleURL:[MGLStyle lightStyleURLWithVersion:9]];
    
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.tintColor = [UIColor darkGrayColor];
    
    // Set the map’s center coordinate and zoom level.
    self.mapView.centerCoordinate = CLLocationCoordinate2DMake(38.897,-77.039);
    self.mapView.zoomLevel = 10.5;
    
    self.mapView.delegate = self;
    [self.view addSubview: self.mapView];
}

// Wait until the style is loaded before modifying the map style.
- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    
    // "mapbox://examples.2uf7qges" is a map ID referencing a tileset. For more
    // more information, see mapbox.com/help/define-map-id/
    MGLSource *source = [[MGLVectorSource alloc] initWithIdentifier:@"trees" configurationURL:[NSURL URLWithString:@"mapbox://examples.2uf7qges"]];
    
    [self.mapView.style addSource:source];
    
    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithIdentifier: @"tree-style" source:source];
    
    // The source name from the source's TileJSON metadata: mapbox.com/api-documentation/#retrieve-tilejson-metadata
    layer.sourceLayerIdentifier = @"yoshino-trees-a0puw5";
    
    // Stops based on age of tree in years.
    NSDictionary *stops = @{
        @0: [MGLStyleValue valueWithRawValue:[UIColor colorWithRed:1.00 green:0.72 blue:0.85 alpha:1.0]],
        @2: [MGLStyleValue valueWithRawValue:[UIColor colorWithRed:0.69 green:0.48 blue:0.73 alpha:1.0]],
        @4: [MGLStyleValue valueWithRawValue:[UIColor colorWithRed:0.61 green:0.31 blue:0.47 alpha:1.0]],
        @7: [MGLStyleValue valueWithRawValue:[UIColor colorWithRed:0.43 green:0.20 blue:0.38 alpha:1.0]],
        @16: [MGLStyleValue valueWithRawValue:[UIColor colorWithRed:0.33 green:0.17 blue:0.25 alpha:1.0]]
    };
    
    // Style the circle layer color based on the above categorical stops.
    layer.circleColor = [MGLStyleValue valueWithInterpolationMode: MGLInterpolationModeInterval
        sourceStops: stops
        attributeName: @"AGE"
        options: nil];
    
    layer.circleRadius = [MGLStyleValue valueWithRawValue:@3];
    
    [self.mapView.style addLayer:layer];
}

@end
