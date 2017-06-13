#import "RuntimeAddLineExample.h"
@import Mapbox;

NSString *const MBXExampleRuntimeAddLine = @"RuntimeAddLineExample";

@interface RuntimeAddLineExample () <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@end

@implementation RuntimeAddLineExample

- (void)viewDidLoad {
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(45.5076, -122.6736)
			    zoomLevel:11
                 animated:NO];

    [self.view addSubview:self.mapView];

    self.mapView.delegate = self;
}

// Wait until the map is loaded before adding to the map.
- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    [self loadGeoJSON];
}

- (void)loadGeoJSON {
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        NSString *path = [[NSBundle mainBundle] pathForResource:@"example" ofType:@"geojson"];
        NSData *jsonData = [NSData dataWithContentsOfFile:path];
        dispatch_async(dispatch_get_main_queue(), ^{
            [self drawPolyline:jsonData];
        });
    });
}

- (void)drawPolyline:(NSData *)geoJson {
    // Add our GeoJSON data to the map as an MGLShapeSource.
    // We can then reference this data from an MGLStyleLayer.
    MGLShape *shape = [MGLShape shapeWithData:geoJson encoding:NSUTF8StringEncoding error:nil];
    MGLSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"polyline" shape:shape options:nil];
    [self.mapView.style addSource:source];

    // Create new layer for the line
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"polyline" source:source];
    layer.lineJoin = [MGLStyleValue valueWithRawValue:[NSValue valueWithMGLLineJoin:MGLLineJoinRound]];
    layer.lineCap = [MGLStyleValue valueWithRawValue:[NSValue valueWithMGLLineCap:MGLLineCapRound]];
    layer.lineColor = [MGLStyleValue valueWithRawValue:[UIColor colorWithRed:59/255.0 green:178/255.0 blue:208/255.0 alpha:1]];
    // Use a style function to smoothly adjust the line width from 2pt to 20pt between zoom levels 14 and 18. The `interpolationBase` parameter allows the values to interpolate along an exponential curve.
    layer.lineWidth = [MGLStyleValue valueWithInterpolationMode:MGLInterpolationModeExponential
        cameraStops:@{
            @14: [MGLStyleValue valueWithRawValue:@2],
            @18: [MGLStyleValue valueWithRawValue:@20]
        }
        options:@{MGLStyleFunctionOptionDefaultValue:@1.5}];

    // We can also add a second layer that will draw a stroke around the original line.
    MGLLineStyleLayer *casingLayer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"polyline-case" source:source];
    // Copy these attributes from the main line layer.
    casingLayer.lineJoin = layer.lineJoin;
    casingLayer.lineCap = layer.lineCap;
    // Line gap width represents the space before the outline begins, so should match the main line’s line width exactly.
    casingLayer.lineGapWidth = layer.lineWidth;
    // Stroke color slightly darker than the line color.
    casingLayer.lineColor = [MGLStyleValue valueWithRawValue:[UIColor colorWithRed:41/255.0 green:145/255.0 blue:171/255.0 alpha:1]];
    // Use a style function to gradually increase the stroke width between zoom levels 14 and 18.
    casingLayer.lineWidth = [MGLStyleValue valueWithInterpolationMode:MGLInterpolationModeExponential
        cameraStops:@{
            @14: [MGLStyleValue valueWithRawValue:@1],
            @18: [MGLStyleValue valueWithRawValue:@4]
        }
        options:@{MGLStyleFunctionOptionDefaultValue:@1.5}];
    
    // Just for fun, let’s add another copy of the line with a dash pattern.
    MGLLineStyleLayer *dashedLayer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"polyline-dash" source:source];
    dashedLayer.lineJoin = layer.lineJoin;
    dashedLayer.lineCap = layer.lineCap;
    dashedLayer.lineWidth = layer.lineWidth;
    dashedLayer.lineColor = [MGLStyleValue valueWithRawValue:[UIColor whiteColor]];
    dashedLayer.lineOpacity = [MGLStyleValue valueWithRawValue:@0.5];
    // Dash pattern in the format [dash, gap, dash, gap, ...]. You’ll want to adjust these values based on the line cap style.
    dashedLayer.lineDashPattern = [MGLStyleValue valueWithRawValue:@[@0, @1.5]];

    [self.mapView.style addLayer:layer];
    [self.mapView.style addLayer:dashedLayer];
    [self.mapView.style insertLayer:casingLayer belowLayer:layer];
}

@end
