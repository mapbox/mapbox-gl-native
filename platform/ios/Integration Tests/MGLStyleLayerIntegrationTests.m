#import "MGLMapViewIntegrationTest.h"

@interface MGLStyleLayerIntegrationTests : MGLMapViewIntegrationTest
@end

@implementation MGLStyleLayerIntegrationTests

- (MGLCircleStyleLayer*)setupCircleStyleLayer {
    // Adapted from https://docs.mapbox.com/ios/examples/dds-circle-layer/

    // "mapbox://examples.2uf7qges" is a tileset ID. For more
    // more information, see docs.mapbox.com/help/glossary/tileset-id/
    MGLSource *source = [[MGLVectorTileSource alloc] initWithIdentifier:@"trees" configurationURL:[NSURL URLWithString:@"mapbox://examples.2uf7qges"]];
    [self.mapView.style addSource:source];

    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithIdentifier: @"tree-style" source:source];

    // The source name from the source's TileJSON metadata: mapbox.com/api-documentation/maps/#retrieve-tilejson-metadata
    layer.sourceLayerIdentifier = @"yoshino-trees-a0puw5";

    return layer;
}

- (void)testForInterpolatingExpressionRenderCrashWithEmptyStops {
    // Tests: https://github.com/mapbox/mapbox-gl-native/issues/9539
    // Adapted from https://docs.mapbox.com/ios/examples/dds-circle-layer/
    self.mapView.centerCoordinate = CLLocationCoordinate2DMake(38.897,-77.039);
    self.mapView.zoomLevel = 10.5;

    MGLCircleStyleLayer *layer = [self setupCircleStyleLayer];

    NSExpression *interpExpression = [NSExpression mgl_expressionForInterpolatingExpression:NSExpression.zoomLevelVariableExpression
                                                                              withCurveType:MGLExpressionInterpolationModeLinear
                                                                                 parameters:nil
                                                                                      stops:[NSExpression expressionForConstantValue:@{}]];

    XCTAssertThrowsSpecificNamed((layer.circleColor = interpExpression), NSException, NSInvalidArgumentException);

    [self.mapView.style addLayer:layer];
    [self waitForMapViewToBeRenderedWithTimeout:10];
}

- (void)testForSteppingExpressionRenderCrashWithEmptyStops {
    // Tests: https://github.com/mapbox/mapbox-gl-native/issues/9539
    // Adapted from https://docs.mapbox.com/ios/examples/dds-circle-layer/
    self.mapView.centerCoordinate = CLLocationCoordinate2DMake(38.897,-77.039);
    self.mapView.zoomLevel = 10.5;

    MGLCircleStyleLayer *layer = [self setupCircleStyleLayer];

    NSExpression *steppingExpression = [NSExpression mgl_expressionForSteppingExpression:NSExpression.zoomLevelVariableExpression
                                                                          fromExpression:[NSExpression expressionForConstantValue:[UIColor greenColor]]
                                                                                   stops:[NSExpression expressionForConstantValue:@{}]];

    XCTAssertThrowsSpecificNamed((layer.circleColor = steppingExpression), NSException, NSInvalidArgumentException);

    [self.mapView.style addLayer:layer];
    [self waitForMapViewToBeRenderedWithTimeout:10];
}

@end
