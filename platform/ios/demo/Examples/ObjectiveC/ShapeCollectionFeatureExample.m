#import "ShapeCollectionFeatureExample.h"
@import Mapbox;

NSString *const MBXExampleShapeCollectionFeature = @"ShapeCollectionFeatureExample";

@interface ShapeCollectionFeatureExample () <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@end

@implementation ShapeCollectionFeatureExample

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:[MGLStyle lightStyleURLWithVersion:9]];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.tintColor = [UIColor darkGrayColor];
    
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(38.897435, -77.039679) zoomLevel:12 animated:NO];
    self.mapView.delegate = self;
    
    [self.view addSubview:self.mapView];
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    
    // Parse the GeoJSON data.
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        NSString *path = [[NSBundle mainBundle] pathForResource:@"metro-line" ofType:@"geojson"];
        
        NSData *data = [NSData dataWithContentsOfFile:path];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [self drawShapeCollection:data];
        });
    });
}

- (void)drawShapeCollection:(NSData *)data {
    
    // Use [MGLShape shapeWithData:encoding:error:] to create a MGLShapeCollectionFeature from GeoJSON data.
    MGLShape *feature = [MGLShape shapeWithData:data encoding:NSUTF8StringEncoding error:NULL];
    
    // Create source and add it to the map style.
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"transit" shape:feature options:nil];
    [self.mapView.style addSource:source];
    
    // Create station style layer.
    MGLCircleStyleLayer *circleLayer = [[MGLCircleStyleLayer alloc] initWithIdentifier:@"stations" source:source];
    circleLayer.predicate = [NSPredicate predicateWithFormat:@"TYPE = 'Station'"];
    circleLayer.circleColor = [MGLStyleValue valueWithRawValue:[UIColor redColor]];
    circleLayer.circleRadius = [MGLStyleValue valueWithRawValue:@6];
    circleLayer.circleStrokeWidth = [MGLStyleValue valueWithRawValue:@2];
    circleLayer.circleStrokeColor = [MGLStyleValue valueWithRawValue:[UIColor blackColor]];
    
    // Create line style layer.
    MGLLineStyleLayer *lineLayer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"rail-line" source: source];
    lineLayer.predicate = [NSPredicate predicateWithFormat:@"TYPE = 'Rail line'"];
    lineLayer.lineColor = [MGLStyleValue valueWithRawValue:[UIColor redColor]];
    lineLayer.lineWidth = [MGLStyleValue valueWithRawValue:@2];
    
    // Add style layers to the map view's style.
    [self.mapView.style addLayer:circleLayer];
    [self.mapView.style insertLayer:lineLayer belowLayer:circleLayer];
}

@end

