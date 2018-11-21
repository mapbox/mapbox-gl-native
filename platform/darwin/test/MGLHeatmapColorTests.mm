#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/heatmap_layer.hpp>

@interface MGLHeatmapColorTests : XCTestCase <MGLMapViewDelegate>
@end

@implementation MGLHeatmapColorTests

- (void)testProperties {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    MGLHeatmapStyleLayer *layer = [[MGLHeatmapStyleLayer alloc] initWithIdentifier:@"layerID" source:source];

    auto rawLayer = static_cast<mbgl::style::HeatmapLayer*>(layer.rawLayer);
    
    XCTAssertTrue(rawLayer->getHeatmapColor().isUndefined(),
                  @"heatmap-color should be unset initially.");
    NSExpression *defaultExpression = layer.heatmapColor;

    NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
    layer.heatmapColor = constantExpression;

    
    mbgl::style::PropertyValue<float> propertyValue = { 0xff };
    XCTAssertEqual(rawLayer->getHeatmapColor().evaluate(0.0), mbgl::Color::red(),
                   @"Setting heatmapColor to a constant value expression should update heatmap-color.");
    XCTAssertEqualObjects(layer.heatmapColor, constantExpression,
                          @"heatmapColor should round-trip constant value expressions.");

    constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
    NSExpression *constantExpression2 = [NSExpression expressionWithFormat:@"%@", [MGLColor blueColor]];
    NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($heatmapDensity, %@, %@)", constantExpression, @{@12: constantExpression2}];
    layer.heatmapColor = functionExpression;
    
    XCTAssertEqual(rawLayer->getHeatmapColor().evaluate(11.0), mbgl::Color::red(),
                   @"Setting heatmapColor to an expression depending on $heatmapDensity should update heatmap-color.");
    XCTAssertEqual(rawLayer->getHeatmapColor().evaluate(12.0), mbgl::Color::blue(),
                   @"Setting heatmapColor to an expression depending on $heatmapDensity should update heatmap-color.");
    XCTAssertEqualObjects(layer.heatmapColor, functionExpression,
                          @"heatmapColor should round-trip expressions depending on $heatmapDensity.");

    layer.heatmapColor = nil;
    XCTAssertTrue(rawLayer->getHeatmapColor().isUndefined(),
                  @"Unsetting heatmapColor should return heatmap-color to the default value.");
    // The contained colors aren’t object equal, even though their descriptions are.
    XCTAssertEqualObjects(layer.heatmapColor.description, defaultExpression.description,
                          @"heatmapColor should return the default value after being unset.");

    functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
    XCTAssertThrowsSpecificNamed(layer.heatmapColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHeatmapLayer should raise an exception if a camera expression is applied to heatmapColor.");
    functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
    XCTAssertThrowsSpecificNamed(layer.heatmapColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHeatmapLayer should raise an exception if a data expression is applied to heatmapColor.");
    functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
    functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
    XCTAssertThrowsSpecificNamed(layer.heatmapColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHeatmapLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
}

@end
