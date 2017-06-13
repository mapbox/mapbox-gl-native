#import "RuntimeCircleStylesExample.h"
@import Mapbox;

NSString *const MBXExampleRuntimeCircleStyles = @"RuntimeCircleStylesExample";

@interface RuntimeCircleStylesExample () <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;

@end

@implementation RuntimeCircleStylesExample

- (void)viewDidLoad {
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    [self.mapView setStyleURL:[MGLStyle lightStyleURLWithVersion:9]];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.tintColor = [UIColor darkGrayColor];

    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(37.753574, -122.447303)
			    zoomLevel:10
			     animated:NO];

    [self.view addSubview:self.mapView];

    self.mapView.delegate = self;
}

// Wait until the style is loaded before modifying the map style.
- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    [self addLayer];
}

- (void)addLayer {
    MGLSource *source = [[MGLVectorSource alloc] initWithIdentifier:@"population" configurationURL:[NSURL URLWithString:@"mapbox://examples.8fgz4egr"]];

    NSDictionary *ethnicities = @{
        @"White": [UIColor colorWithRed: 251/255.0 green: 176/255.0 blue: 59/255.0 alpha: 1.0],
        @"Black": [UIColor colorWithRed: 34/255.0 green: 59/255.0 blue: 83/255.0 alpha: 1.0],
        @"Hispanic": [UIColor colorWithRed: 229/255.0 green: 94/255.0 blue: 94/255.0 alpha: 1.0],
        @"Asian": [UIColor colorWithRed: 59/255.0 green: 178/255.0 blue: 208/255.0 alpha: 1.0],
        @"Other": [UIColor colorWithRed: 204/255.0 green: 204/255.0 blue: 204/255.0 alpha: 1.0],
    };


    [self.mapView.style addSource:source];

    // Create a new layer for each ethnicity/circle color.
    for (NSString *key in [ethnicities allKeys]) {
        // Each layer should have a unique identifier.
        MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithIdentifier:[NSString stringWithFormat:@"population-%@", key] source:source];

        // Specifying the `sourceLayerIdentifier` is required for a vector tile source. This is the json attribute that wraps the data in the source.
        layer.sourceLayerIdentifier = @"sf2010";

        // Use a style function to smoothly adjust the circle radius from 2pt to 180pt between zoom levels 12 and 22. The `interpolationBase` parameter allows the values to interpolate along an exponential curve.
        layer.circleRadius = [MGLStyleValue valueWithInterpolationMode:MGLInterpolationModeExponential
            cameraStops:@{
                @12: [MGLStyleValue valueWithRawValue:@2],
                @22: [MGLStyleValue valueWithRawValue:@180]
            }
            options: @{MGLStyleFunctionOptionDefaultValue:@1.75}];
        layer.circleOpacity = [MGLStyleValue valueWithRawValue:@0.7];

        // Set the circle color to match the ethnicity.
        layer.circleColor = [MGLStyleValue valueWithRawValue:ethnicities[key]];
        
        // Use an NSPredicate to filter to just one ethnicity for this layer.
        layer.predicate = [NSPredicate predicateWithFormat:@"ethnicity == %@", key];
        
        [self.mapView.style addLayer:layer];
    }
}

@end
