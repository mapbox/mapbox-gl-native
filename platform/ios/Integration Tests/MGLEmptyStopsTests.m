#import "MGLMapViewIntegrationTest.h"

@interface MGLEmptyStopsTests : MGLMapViewIntegrationTest

@end

@implementation MGLEmptyStopsTests


- (void)testEmptyStops {
    // From https://www.mapbox.com/ios-sdk/examples/dds-circle-layer/
    self.mapView.centerCoordinate = CLLocationCoordinate2DMake(38.897,-77.039);
    self.mapView.zoomLevel = 10.5;

    // "mapbox://examples.2uf7qges" is a map ID referencing a tileset. For more
    // more information, see mapbox.com/help/define-map-id/
    MGLSource *source = [[MGLVectorTileSource alloc] initWithIdentifier:@"trees" configurationURL:[NSURL URLWithString:@"mapbox://examples.2uf7qges"]];

    [self.mapView.style addSource:source];

    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithIdentifier: @"tree-style" source:source];

    // The source name from the source's TileJSON metadata: mapbox.com/api-documentation/#retrieve-tilejson-metadata
    layer.sourceLayerIdentifier = @"yoshino-trees-a0puw5";

    {
        NSExpression *interpExpression = [NSExpression mgl_expressionForInterpolatingExpression:NSExpression.zoomLevelVariableExpression
                                                                                  withCurveType:MGLExpressionInterpolationModeLinear
                                                                                     parameters:nil
                                                                                          stops:[NSExpression expressionForConstantValue:@{}]];

        @try {
            layer.circleColor = interpExpression;
            XCTFail();
        }
        @catch (NSException *exception) {
            XCTAssertNotNil(exception);
            XCTAssert(exception.name == NSInvalidArgumentException);
        }
    }

    {
        NSExpression *steppingExpression = [NSExpression mgl_expressionForSteppingExpression:NSExpression.zoomLevelVariableExpression
                                                                              fromExpression:[NSExpression expressionForConstantValue:[UIColor greenColor]]
                                                                                       stops:[NSExpression expressionForConstantValue:@{}]];

        @try {
            layer.circleColor = steppingExpression;
            XCTFail();
        }
        @catch (NSException *exception) {
            XCTAssertNotNil(exception);
            XCTAssert(exception.name == NSInvalidArgumentException);
        }
    }

    [self.mapView.style addLayer:layer];
    [self waitForMapViewToBeRenderedWithTimeout:5.0];
}

@end
