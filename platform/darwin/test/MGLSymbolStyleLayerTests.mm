// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"
#include "../../darwin/src/MGLAttributedExpression.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/expression/dsl.hpp>

@interface MGLSymbolLayerTests : MGLStyleLayerTests
@end

@implementation MGLSymbolLayerTests

+ (NSString *)layerType {
    return @"symbol";
}

- (void)testPredicates {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    MGLSymbolStyleLayer *layer = [[MGLSymbolStyleLayer alloc] initWithIdentifier:@"layerID" source:source];

    XCTAssertNil(layer.sourceLayerIdentifier);
    layer.sourceLayerIdentifier = @"layerID";
    XCTAssertEqualObjects(layer.sourceLayerIdentifier, @"layerID");
    layer.sourceLayerIdentifier = nil;
    XCTAssertNil(layer.sourceLayerIdentifier);

    XCTAssertNil(layer.predicate);
    layer.predicate = [NSPredicate predicateWithFormat:@"$featureIdentifier = 1"];
    XCTAssertEqualObjects(layer.predicate,  [NSPredicate predicateWithFormat:@"$featureIdentifier = 1"]);
    layer.predicate = nil;
    XCTAssertNil(layer.predicate);
}

- (void)testProperties {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];

    MGLSymbolStyleLayer *layer = [[MGLSymbolStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertEqualObjects(@(layer.rawLayer->getTypeInfo()->type), @"symbol");
    auto rawLayer = static_cast<mbgl::style::SymbolLayer*>(layer.rawLayer);

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // icon-allow-overlap
    {
        XCTAssertTrue(rawLayer->getIconAllowOverlap().isUndefined(),
                      @"icon-allow-overlap should be unset initially.");
        NSExpression *defaultExpression = layer.iconAllowsOverlap;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"true"];
        layer.iconAllowsOverlap = constantExpression;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconAllowOverlap(), propertyValue,
                       @"Setting iconAllowsOverlap to a constant value expression should update icon-allow-overlap.");
        XCTAssertEqualObjects(layer.iconAllowsOverlap, constantExpression,
                              @"iconAllowsOverlap should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"true"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconAllowsOverlap = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<bool>(
                step(zoom(), literal(true), 18.0, literal(true))
            );
        }

        XCTAssertEqual(rawLayer->getIconAllowOverlap(), propertyValue,
                       @"Setting iconAllowsOverlap to a camera expression should update icon-allow-overlap.");
        XCTAssertEqualObjects(layer.iconAllowsOverlap, functionExpression,
                              @"iconAllowsOverlap should round-trip camera expressions.");


        layer.iconAllowsOverlap = nil;
        XCTAssertTrue(rawLayer->getIconAllowOverlap().isUndefined(),
                      @"Unsetting iconAllowsOverlap should return icon-allow-overlap to the default value.");
        XCTAssertEqualObjects(layer.iconAllowsOverlap, defaultExpression,
                              @"iconAllowsOverlap should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.iconAllowsOverlap = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.iconAllowsOverlap = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // icon-anchor
    {
        XCTAssertTrue(rawLayer->getIconAnchor().isUndefined(),
                      @"icon-anchor should be unset initially.");
        NSExpression *defaultExpression = layer.iconAnchor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'bottom-right'"];
        layer.iconAnchor = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::SymbolAnchorType> propertyValue = { mbgl::style::SymbolAnchorType::BottomRight };
        XCTAssertEqual(rawLayer->getIconAnchor(), propertyValue,
                       @"Setting iconAnchor to a constant value expression should update icon-anchor.");
        XCTAssertEqualObjects(layer.iconAnchor, constantExpression,
                              @"iconAnchor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'bottom-right'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconAnchor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::SymbolAnchorType>(
                step(zoom(), literal("bottom-right"), 18.0, literal("bottom-right"))
            );
        }

        XCTAssertEqual(rawLayer->getIconAnchor(), propertyValue,
                       @"Setting iconAnchor to a camera expression should update icon-anchor.");
        XCTAssertEqualObjects(layer.iconAnchor, functionExpression,
                              @"iconAnchor should round-trip camera expressions.");


        layer.iconAnchor = nil;
        XCTAssertTrue(rawLayer->getIconAnchor().isUndefined(),
                      @"Unsetting iconAnchor should return icon-anchor to the default value.");
        XCTAssertEqualObjects(layer.iconAnchor, defaultExpression,
                              @"iconAnchor should return the default value after being unset.");
    }

    // icon-ignore-placement
    {
        XCTAssertTrue(rawLayer->getIconIgnorePlacement().isUndefined(),
                      @"icon-ignore-placement should be unset initially.");
        NSExpression *defaultExpression = layer.iconIgnoresPlacement;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"true"];
        layer.iconIgnoresPlacement = constantExpression;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconIgnorePlacement(), propertyValue,
                       @"Setting iconIgnoresPlacement to a constant value expression should update icon-ignore-placement.");
        XCTAssertEqualObjects(layer.iconIgnoresPlacement, constantExpression,
                              @"iconIgnoresPlacement should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"true"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconIgnoresPlacement = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<bool>(
                step(zoom(), literal(true), 18.0, literal(true))
            );
        }

        XCTAssertEqual(rawLayer->getIconIgnorePlacement(), propertyValue,
                       @"Setting iconIgnoresPlacement to a camera expression should update icon-ignore-placement.");
        XCTAssertEqualObjects(layer.iconIgnoresPlacement, functionExpression,
                              @"iconIgnoresPlacement should round-trip camera expressions.");


        layer.iconIgnoresPlacement = nil;
        XCTAssertTrue(rawLayer->getIconIgnorePlacement().isUndefined(),
                      @"Unsetting iconIgnoresPlacement should return icon-ignore-placement to the default value.");
        XCTAssertEqualObjects(layer.iconIgnoresPlacement, defaultExpression,
                              @"iconIgnoresPlacement should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.iconIgnoresPlacement = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.iconIgnoresPlacement = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // icon-image
    {
        XCTAssertTrue(rawLayer->getIconImage().isUndefined(),
                      @"icon-image should be unset initially.");
        NSExpression *defaultExpression = layer.iconImageName;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"Icon Image"];
        layer.iconImageName = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::expression::Image> propertyValue = { "Icon Image" };
        XCTAssertEqual(rawLayer->getIconImage(), propertyValue,
                       @"Setting iconImageName to a constant value expression should update icon-image.");
        XCTAssertEqualObjects(layer.iconImageName, constantExpression,
                              @"iconImageName should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"Icon Image"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconImageName = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::expression::Image>(
                step(zoom(), literal("Icon Image"), 18.0, literal("Icon Image"))
            );
        }

        XCTAssertEqual(rawLayer->getIconImage(), propertyValue,
                       @"Setting iconImageName to a camera expression should update icon-image.");
        XCTAssertEqualObjects(layer.iconImageName, functionExpression,
                              @"iconImageName should round-trip camera expressions.");


        layer.iconImageName = nil;
        XCTAssertTrue(rawLayer->getIconImage().isUndefined(),
                      @"Unsetting iconImageName should return icon-image to the default value.");
        XCTAssertEqualObjects(layer.iconImageName, defaultExpression,
                              @"iconImageName should return the default value after being unset.");

        // Tokens test
        layer.iconImageName = [NSExpression expressionForConstantValue:@"{token}"];

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::expression::Image>(
                toString(get(literal("token")))
            );
        }

        XCTAssertEqual(rawLayer->getIconImage(), propertyValue,
                       @"Setting iconImageName to a constant string with tokens should convert to an expression.");

        NSExpression* tokenExpression = [NSExpression expressionWithFormat:@"CAST(token, \"NSString\")"];
        XCTAssertEqualObjects(layer.iconImageName, tokenExpression,
                              @"Setting iconImageName to a constant string with tokens should convert to an expression.");
    }

    // icon-offset
    {
        XCTAssertTrue(rawLayer->getIconOffset().isUndefined(),
                      @"icon-offset should be unset initially.");
        NSExpression *defaultExpression = layer.iconOffset;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@",
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.iconOffset = constantExpression;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getIconOffset(), propertyValue,
                       @"Setting iconOffset to a constant value expression should update icon-offset.");
        XCTAssertEqualObjects(layer.iconOffset, constantExpression,
                              @"iconOffset should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{1, 1}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconOffset = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::array<float, 2>>(
                step(zoom(), literal({ 1, 1 }), 18.0, literal({ 1, 1 }))
            );
        }

        XCTAssertEqual(rawLayer->getIconOffset(), propertyValue,
                       @"Setting iconOffset to a camera expression should update icon-offset.");
        XCTAssertEqualObjects(layer.iconOffset, functionExpression,
                              @"iconOffset should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.iconOffset = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::array<float, 2>>(
                interpolate(linear(), number(get("keyName")), 18.0, literal({ 1, 1 }))
            );
        }

        XCTAssertEqual(rawLayer->getIconOffset(), propertyValue,
                       @"Setting iconOffset to a data expression should update icon-offset.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.iconOffset, pedanticFunctionExpression,
                              @"iconOffset should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.iconOffset = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::array<float, 2>>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal({ 1, 1 })))
            );
        }

        XCTAssertEqual(rawLayer->getIconOffset(), propertyValue,
                       @"Setting iconOffset to a camera-data expression should update icon-offset.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.iconOffset, pedanticFunctionExpression,
                              @"iconOffset should round-trip camera-data expressions.");

        layer.iconOffset = nil;
        XCTAssertTrue(rawLayer->getIconOffset().isUndefined(),
                      @"Unsetting iconOffset should return icon-offset to the default value.");
        XCTAssertEqualObjects(layer.iconOffset, defaultExpression,
                              @"iconOffset should return the default value after being unset.");
    }

    // icon-optional
    {
        XCTAssertTrue(rawLayer->getIconOptional().isUndefined(),
                      @"icon-optional should be unset initially.");
        NSExpression *defaultExpression = layer.iconOptional;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"true"];
        layer.iconOptional = constantExpression;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconOptional(), propertyValue,
                       @"Setting iconOptional to a constant value expression should update icon-optional.");
        XCTAssertEqualObjects(layer.iconOptional, constantExpression,
                              @"iconOptional should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"true"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconOptional = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<bool>(
                step(zoom(), literal(true), 18.0, literal(true))
            );
        }

        XCTAssertEqual(rawLayer->getIconOptional(), propertyValue,
                       @"Setting iconOptional to a camera expression should update icon-optional.");
        XCTAssertEqualObjects(layer.iconOptional, functionExpression,
                              @"iconOptional should round-trip camera expressions.");


        layer.iconOptional = nil;
        XCTAssertTrue(rawLayer->getIconOptional().isUndefined(),
                      @"Unsetting iconOptional should return icon-optional to the default value.");
        XCTAssertEqualObjects(layer.iconOptional, defaultExpression,
                              @"iconOptional should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.iconOptional = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.iconOptional = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // icon-padding
    {
        XCTAssertTrue(rawLayer->getIconPadding().isUndefined(),
                      @"icon-padding should be unset initially.");
        NSExpression *defaultExpression = layer.iconPadding;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.iconPadding = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getIconPadding(), propertyValue,
                       @"Setting iconPadding to a constant value expression should update icon-padding.");
        XCTAssertEqualObjects(layer.iconPadding, constantExpression,
                              @"iconPadding should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconPadding = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconPadding(), propertyValue,
                       @"Setting iconPadding to a camera expression should update icon-padding.");
        XCTAssertEqualObjects(layer.iconPadding, functionExpression,
                              @"iconPadding should round-trip camera expressions.");


        layer.iconPadding = nil;
        XCTAssertTrue(rawLayer->getIconPadding().isUndefined(),
                      @"Unsetting iconPadding should return icon-padding to the default value.");
        XCTAssertEqualObjects(layer.iconPadding, defaultExpression,
                              @"iconPadding should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.iconPadding = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.iconPadding = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // icon-pitch-alignment
    {
        XCTAssertTrue(rawLayer->getIconPitchAlignment().isUndefined(),
                      @"icon-pitch-alignment should be unset initially.");
        NSExpression *defaultExpression = layer.iconPitchAlignment;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'auto'"];
        layer.iconPitchAlignment = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Auto };
        XCTAssertEqual(rawLayer->getIconPitchAlignment(), propertyValue,
                       @"Setting iconPitchAlignment to a constant value expression should update icon-pitch-alignment.");
        XCTAssertEqualObjects(layer.iconPitchAlignment, constantExpression,
                              @"iconPitchAlignment should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'auto'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconPitchAlignment = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::AlignmentType>(
                step(zoom(), literal("auto"), 18.0, literal("auto"))
            );
        }

        XCTAssertEqual(rawLayer->getIconPitchAlignment(), propertyValue,
                       @"Setting iconPitchAlignment to a camera expression should update icon-pitch-alignment.");
        XCTAssertEqualObjects(layer.iconPitchAlignment, functionExpression,
                              @"iconPitchAlignment should round-trip camera expressions.");


        layer.iconPitchAlignment = nil;
        XCTAssertTrue(rawLayer->getIconPitchAlignment().isUndefined(),
                      @"Unsetting iconPitchAlignment should return icon-pitch-alignment to the default value.");
        XCTAssertEqualObjects(layer.iconPitchAlignment, defaultExpression,
                              @"iconPitchAlignment should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.iconPitchAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.iconPitchAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // icon-rotate
    {
        XCTAssertTrue(rawLayer->getIconRotate().isUndefined(),
                      @"icon-rotate should be unset initially.");
        NSExpression *defaultExpression = layer.iconRotation;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.iconRotation = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getIconRotate(), propertyValue,
                       @"Setting iconRotation to a constant value expression should update icon-rotate.");
        XCTAssertEqualObjects(layer.iconRotation, constantExpression,
                              @"iconRotation should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconRotation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconRotate(), propertyValue,
                       @"Setting iconRotation to a camera expression should update icon-rotate.");
        XCTAssertEqualObjects(layer.iconRotation, functionExpression,
                              @"iconRotation should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.iconRotation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconRotate(), propertyValue,
                       @"Setting iconRotation to a data expression should update icon-rotate.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.iconRotation, pedanticFunctionExpression,
                              @"iconRotation should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.iconRotation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getIconRotate(), propertyValue,
                       @"Setting iconRotation to a camera-data expression should update icon-rotate.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.iconRotation, pedanticFunctionExpression,
                              @"iconRotation should round-trip camera-data expressions.");

        layer.iconRotation = nil;
        XCTAssertTrue(rawLayer->getIconRotate().isUndefined(),
                      @"Unsetting iconRotation should return icon-rotate to the default value.");
        XCTAssertEqualObjects(layer.iconRotation, defaultExpression,
                              @"iconRotation should return the default value after being unset.");
    }

    // icon-rotation-alignment
    {
        XCTAssertTrue(rawLayer->getIconRotationAlignment().isUndefined(),
                      @"icon-rotation-alignment should be unset initially.");
        NSExpression *defaultExpression = layer.iconRotationAlignment;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'auto'"];
        layer.iconRotationAlignment = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Auto };
        XCTAssertEqual(rawLayer->getIconRotationAlignment(), propertyValue,
                       @"Setting iconRotationAlignment to a constant value expression should update icon-rotation-alignment.");
        XCTAssertEqualObjects(layer.iconRotationAlignment, constantExpression,
                              @"iconRotationAlignment should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'auto'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconRotationAlignment = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::AlignmentType>(
                step(zoom(), literal("auto"), 18.0, literal("auto"))
            );
        }

        XCTAssertEqual(rawLayer->getIconRotationAlignment(), propertyValue,
                       @"Setting iconRotationAlignment to a camera expression should update icon-rotation-alignment.");
        XCTAssertEqualObjects(layer.iconRotationAlignment, functionExpression,
                              @"iconRotationAlignment should round-trip camera expressions.");


        layer.iconRotationAlignment = nil;
        XCTAssertTrue(rawLayer->getIconRotationAlignment().isUndefined(),
                      @"Unsetting iconRotationAlignment should return icon-rotation-alignment to the default value.");
        XCTAssertEqualObjects(layer.iconRotationAlignment, defaultExpression,
                              @"iconRotationAlignment should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.iconRotationAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.iconRotationAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // icon-size
    {
        XCTAssertTrue(rawLayer->getIconSize().isUndefined(),
                      @"icon-size should be unset initially.");
        NSExpression *defaultExpression = layer.iconScale;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.iconScale = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getIconSize(), propertyValue,
                       @"Setting iconScale to a constant value expression should update icon-size.");
        XCTAssertEqualObjects(layer.iconScale, constantExpression,
                              @"iconScale should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconScale = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconSize(), propertyValue,
                       @"Setting iconScale to a camera expression should update icon-size.");
        XCTAssertEqualObjects(layer.iconScale, functionExpression,
                              @"iconScale should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.iconScale = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconSize(), propertyValue,
                       @"Setting iconScale to a data expression should update icon-size.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.iconScale, pedanticFunctionExpression,
                              @"iconScale should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.iconScale = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getIconSize(), propertyValue,
                       @"Setting iconScale to a camera-data expression should update icon-size.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.iconScale, pedanticFunctionExpression,
                              @"iconScale should round-trip camera-data expressions.");

        layer.iconScale = nil;
        XCTAssertTrue(rawLayer->getIconSize().isUndefined(),
                      @"Unsetting iconScale should return icon-size to the default value.");
        XCTAssertEqualObjects(layer.iconScale, defaultExpression,
                              @"iconScale should return the default value after being unset.");
    }

    // icon-text-fit
    {
        XCTAssertTrue(rawLayer->getIconTextFit().isUndefined(),
                      @"icon-text-fit should be unset initially.");
        NSExpression *defaultExpression = layer.iconTextFit;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'both'"];
        layer.iconTextFit = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::IconTextFitType> propertyValue = { mbgl::style::IconTextFitType::Both };
        XCTAssertEqual(rawLayer->getIconTextFit(), propertyValue,
                       @"Setting iconTextFit to a constant value expression should update icon-text-fit.");
        XCTAssertEqualObjects(layer.iconTextFit, constantExpression,
                              @"iconTextFit should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'both'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconTextFit = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::IconTextFitType>(
                step(zoom(), literal("both"), 18.0, literal("both"))
            );
        }

        XCTAssertEqual(rawLayer->getIconTextFit(), propertyValue,
                       @"Setting iconTextFit to a camera expression should update icon-text-fit.");
        XCTAssertEqualObjects(layer.iconTextFit, functionExpression,
                              @"iconTextFit should round-trip camera expressions.");


        layer.iconTextFit = nil;
        XCTAssertTrue(rawLayer->getIconTextFit().isUndefined(),
                      @"Unsetting iconTextFit should return icon-text-fit to the default value.");
        XCTAssertEqualObjects(layer.iconTextFit, defaultExpression,
                              @"iconTextFit should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.iconTextFit = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.iconTextFit = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // icon-text-fit-padding
    {
        XCTAssertTrue(rawLayer->getIconTextFitPadding().isUndefined(),
                      @"icon-text-fit-padding should be unset initially.");
        NSExpression *defaultExpression = layer.iconTextFitPadding;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@",
#if TARGET_OS_IPHONE
            [NSValue valueWithUIEdgeInsets:UIEdgeInsetsMake(1, 1, 1, 1)]
#else
            [NSValue valueWithEdgeInsets:NSEdgeInsetsMake(1, 1, 1, 1)]
#endif
        ];
        layer.iconTextFitPadding = constantExpression;
        mbgl::style::PropertyValue<std::array<float, 4>> propertyValue = { { 1, 1, 1, 1 } };
        XCTAssertEqual(rawLayer->getIconTextFitPadding(), propertyValue,
                       @"Setting iconTextFitPadding to a constant value expression should update icon-text-fit-padding.");
        XCTAssertEqualObjects(layer.iconTextFitPadding, constantExpression,
                              @"iconTextFitPadding should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{1, 1, 1, 1}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconTextFitPadding = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::array<float, 4>>(
                step(zoom(), literal({ 1, 1, 1, 1 }), 18.0, literal({ 1, 1, 1, 1 }))
            );
        }

        XCTAssertEqual(rawLayer->getIconTextFitPadding(), propertyValue,
                       @"Setting iconTextFitPadding to a camera expression should update icon-text-fit-padding.");
        XCTAssertEqualObjects(layer.iconTextFitPadding, functionExpression,
                              @"iconTextFitPadding should round-trip camera expressions.");


        layer.iconTextFitPadding = nil;
        XCTAssertTrue(rawLayer->getIconTextFitPadding().isUndefined(),
                      @"Unsetting iconTextFitPadding should return icon-text-fit-padding to the default value.");
        XCTAssertEqualObjects(layer.iconTextFitPadding, defaultExpression,
                              @"iconTextFitPadding should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.iconTextFitPadding = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.iconTextFitPadding = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // icon-keep-upright
    {
        XCTAssertTrue(rawLayer->getIconKeepUpright().isUndefined(),
                      @"icon-keep-upright should be unset initially.");
        NSExpression *defaultExpression = layer.keepsIconUpright;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"true"];
        layer.keepsIconUpright = constantExpression;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconKeepUpright(), propertyValue,
                       @"Setting keepsIconUpright to a constant value expression should update icon-keep-upright.");
        XCTAssertEqualObjects(layer.keepsIconUpright, constantExpression,
                              @"keepsIconUpright should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"true"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.keepsIconUpright = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<bool>(
                step(zoom(), literal(true), 18.0, literal(true))
            );
        }

        XCTAssertEqual(rawLayer->getIconKeepUpright(), propertyValue,
                       @"Setting keepsIconUpright to a camera expression should update icon-keep-upright.");
        XCTAssertEqualObjects(layer.keepsIconUpright, functionExpression,
                              @"keepsIconUpright should round-trip camera expressions.");


        layer.keepsIconUpright = nil;
        XCTAssertTrue(rawLayer->getIconKeepUpright().isUndefined(),
                      @"Unsetting keepsIconUpright should return icon-keep-upright to the default value.");
        XCTAssertEqualObjects(layer.keepsIconUpright, defaultExpression,
                              @"keepsIconUpright should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.keepsIconUpright = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.keepsIconUpright = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-keep-upright
    {
        XCTAssertTrue(rawLayer->getTextKeepUpright().isUndefined(),
                      @"text-keep-upright should be unset initially.");
        NSExpression *defaultExpression = layer.keepsTextUpright;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"false"];
        layer.keepsTextUpright = constantExpression;
        mbgl::style::PropertyValue<bool> propertyValue = { false };
        XCTAssertEqual(rawLayer->getTextKeepUpright(), propertyValue,
                       @"Setting keepsTextUpright to a constant value expression should update text-keep-upright.");
        XCTAssertEqualObjects(layer.keepsTextUpright, constantExpression,
                              @"keepsTextUpright should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"false"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.keepsTextUpright = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<bool>(
                step(zoom(), literal(false), 18.0, literal(false))
            );
        }

        XCTAssertEqual(rawLayer->getTextKeepUpright(), propertyValue,
                       @"Setting keepsTextUpright to a camera expression should update text-keep-upright.");
        XCTAssertEqualObjects(layer.keepsTextUpright, functionExpression,
                              @"keepsTextUpright should round-trip camera expressions.");


        layer.keepsTextUpright = nil;
        XCTAssertTrue(rawLayer->getTextKeepUpright().isUndefined(),
                      @"Unsetting keepsTextUpright should return text-keep-upright to the default value.");
        XCTAssertEqualObjects(layer.keepsTextUpright, defaultExpression,
                              @"keepsTextUpright should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.keepsTextUpright = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.keepsTextUpright = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-max-angle
    {
        XCTAssertTrue(rawLayer->getTextMaxAngle().isUndefined(),
                      @"text-max-angle should be unset initially.");
        NSExpression *defaultExpression = layer.maximumTextAngle;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.maximumTextAngle = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextMaxAngle(), propertyValue,
                       @"Setting maximumTextAngle to a constant value expression should update text-max-angle.");
        XCTAssertEqualObjects(layer.maximumTextAngle, constantExpression,
                              @"maximumTextAngle should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.maximumTextAngle = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextMaxAngle(), propertyValue,
                       @"Setting maximumTextAngle to a camera expression should update text-max-angle.");
        XCTAssertEqualObjects(layer.maximumTextAngle, functionExpression,
                              @"maximumTextAngle should round-trip camera expressions.");


        layer.maximumTextAngle = nil;
        XCTAssertTrue(rawLayer->getTextMaxAngle().isUndefined(),
                      @"Unsetting maximumTextAngle should return text-max-angle to the default value.");
        XCTAssertEqualObjects(layer.maximumTextAngle, defaultExpression,
                              @"maximumTextAngle should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.maximumTextAngle = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.maximumTextAngle = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-max-width
    {
        XCTAssertTrue(rawLayer->getTextMaxWidth().isUndefined(),
                      @"text-max-width should be unset initially.");
        NSExpression *defaultExpression = layer.maximumTextWidth;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.maximumTextWidth = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextMaxWidth(), propertyValue,
                       @"Setting maximumTextWidth to a constant value expression should update text-max-width.");
        XCTAssertEqualObjects(layer.maximumTextWidth, constantExpression,
                              @"maximumTextWidth should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.maximumTextWidth = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextMaxWidth(), propertyValue,
                       @"Setting maximumTextWidth to a camera expression should update text-max-width.");
        XCTAssertEqualObjects(layer.maximumTextWidth, functionExpression,
                              @"maximumTextWidth should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.maximumTextWidth = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextMaxWidth(), propertyValue,
                       @"Setting maximumTextWidth to a data expression should update text-max-width.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.maximumTextWidth, pedanticFunctionExpression,
                              @"maximumTextWidth should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.maximumTextWidth = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getTextMaxWidth(), propertyValue,
                       @"Setting maximumTextWidth to a camera-data expression should update text-max-width.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.maximumTextWidth, pedanticFunctionExpression,
                              @"maximumTextWidth should round-trip camera-data expressions.");

        layer.maximumTextWidth = nil;
        XCTAssertTrue(rawLayer->getTextMaxWidth().isUndefined(),
                      @"Unsetting maximumTextWidth should return text-max-width to the default value.");
        XCTAssertEqualObjects(layer.maximumTextWidth, defaultExpression,
                              @"maximumTextWidth should return the default value after being unset.");
    }

    // symbol-avoid-edges
    {
        XCTAssertTrue(rawLayer->getSymbolAvoidEdges().isUndefined(),
                      @"symbol-avoid-edges should be unset initially.");
        NSExpression *defaultExpression = layer.symbolAvoidsEdges;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"true"];
        layer.symbolAvoidsEdges = constantExpression;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getSymbolAvoidEdges(), propertyValue,
                       @"Setting symbolAvoidsEdges to a constant value expression should update symbol-avoid-edges.");
        XCTAssertEqualObjects(layer.symbolAvoidsEdges, constantExpression,
                              @"symbolAvoidsEdges should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"true"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.symbolAvoidsEdges = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<bool>(
                step(zoom(), literal(true), 18.0, literal(true))
            );
        }

        XCTAssertEqual(rawLayer->getSymbolAvoidEdges(), propertyValue,
                       @"Setting symbolAvoidsEdges to a camera expression should update symbol-avoid-edges.");
        XCTAssertEqualObjects(layer.symbolAvoidsEdges, functionExpression,
                              @"symbolAvoidsEdges should round-trip camera expressions.");


        layer.symbolAvoidsEdges = nil;
        XCTAssertTrue(rawLayer->getSymbolAvoidEdges().isUndefined(),
                      @"Unsetting symbolAvoidsEdges should return symbol-avoid-edges to the default value.");
        XCTAssertEqualObjects(layer.symbolAvoidsEdges, defaultExpression,
                              @"symbolAvoidsEdges should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.symbolAvoidsEdges = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.symbolAvoidsEdges = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // symbol-placement
    {
        XCTAssertTrue(rawLayer->getSymbolPlacement().isUndefined(),
                      @"symbol-placement should be unset initially.");
        NSExpression *defaultExpression = layer.symbolPlacement;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'line-center'"];
        layer.symbolPlacement = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::SymbolPlacementType> propertyValue = { mbgl::style::SymbolPlacementType::LineCenter };
        XCTAssertEqual(rawLayer->getSymbolPlacement(), propertyValue,
                       @"Setting symbolPlacement to a constant value expression should update symbol-placement.");
        XCTAssertEqualObjects(layer.symbolPlacement, constantExpression,
                              @"symbolPlacement should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'line-center'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.symbolPlacement = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::SymbolPlacementType>(
                step(zoom(), literal("line-center"), 18.0, literal("line-center"))
            );
        }

        XCTAssertEqual(rawLayer->getSymbolPlacement(), propertyValue,
                       @"Setting symbolPlacement to a camera expression should update symbol-placement.");
        XCTAssertEqualObjects(layer.symbolPlacement, functionExpression,
                              @"symbolPlacement should round-trip camera expressions.");


        layer.symbolPlacement = nil;
        XCTAssertTrue(rawLayer->getSymbolPlacement().isUndefined(),
                      @"Unsetting symbolPlacement should return symbol-placement to the default value.");
        XCTAssertEqualObjects(layer.symbolPlacement, defaultExpression,
                              @"symbolPlacement should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.symbolPlacement = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.symbolPlacement = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // symbol-sort-key
    {
        XCTAssertTrue(rawLayer->getSymbolSortKey().isUndefined(),
                      @"symbol-sort-key should be unset initially.");
        NSExpression *defaultExpression = layer.symbolSortKey;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.symbolSortKey = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getSymbolSortKey(), propertyValue,
                       @"Setting symbolSortKey to a constant value expression should update symbol-sort-key.");
        XCTAssertEqualObjects(layer.symbolSortKey, constantExpression,
                              @"symbolSortKey should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.symbolSortKey = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getSymbolSortKey(), propertyValue,
                       @"Setting symbolSortKey to a camera expression should update symbol-sort-key.");
        XCTAssertEqualObjects(layer.symbolSortKey, functionExpression,
                              @"symbolSortKey should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.symbolSortKey = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getSymbolSortKey(), propertyValue,
                       @"Setting symbolSortKey to a data expression should update symbol-sort-key.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.symbolSortKey, pedanticFunctionExpression,
                              @"symbolSortKey should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.symbolSortKey = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getSymbolSortKey(), propertyValue,
                       @"Setting symbolSortKey to a camera-data expression should update symbol-sort-key.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.symbolSortKey, pedanticFunctionExpression,
                              @"symbolSortKey should round-trip camera-data expressions.");

        layer.symbolSortKey = nil;
        XCTAssertTrue(rawLayer->getSymbolSortKey().isUndefined(),
                      @"Unsetting symbolSortKey should return symbol-sort-key to the default value.");
        XCTAssertEqualObjects(layer.symbolSortKey, defaultExpression,
                              @"symbolSortKey should return the default value after being unset.");
    }

    // symbol-spacing
    {
        XCTAssertTrue(rawLayer->getSymbolSpacing().isUndefined(),
                      @"symbol-spacing should be unset initially.");
        NSExpression *defaultExpression = layer.symbolSpacing;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.symbolSpacing = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getSymbolSpacing(), propertyValue,
                       @"Setting symbolSpacing to a constant value expression should update symbol-spacing.");
        XCTAssertEqualObjects(layer.symbolSpacing, constantExpression,
                              @"symbolSpacing should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.symbolSpacing = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getSymbolSpacing(), propertyValue,
                       @"Setting symbolSpacing to a camera expression should update symbol-spacing.");
        XCTAssertEqualObjects(layer.symbolSpacing, functionExpression,
                              @"symbolSpacing should round-trip camera expressions.");


        layer.symbolSpacing = nil;
        XCTAssertTrue(rawLayer->getSymbolSpacing().isUndefined(),
                      @"Unsetting symbolSpacing should return symbol-spacing to the default value.");
        XCTAssertEqualObjects(layer.symbolSpacing, defaultExpression,
                              @"symbolSpacing should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.symbolSpacing = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.symbolSpacing = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // symbol-z-order
    {
        XCTAssertTrue(rawLayer->getSymbolZOrder().isUndefined(),
                      @"symbol-z-order should be unset initially.");
        NSExpression *defaultExpression = layer.symbolZOrder;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'source'"];
        layer.symbolZOrder = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::SymbolZOrderType> propertyValue = { mbgl::style::SymbolZOrderType::Source };
        XCTAssertEqual(rawLayer->getSymbolZOrder(), propertyValue,
                       @"Setting symbolZOrder to a constant value expression should update symbol-z-order.");
        XCTAssertEqualObjects(layer.symbolZOrder, constantExpression,
                              @"symbolZOrder should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'source'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.symbolZOrder = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::SymbolZOrderType>(
                step(zoom(), literal("source"), 18.0, literal("source"))
            );
        }

        XCTAssertEqual(rawLayer->getSymbolZOrder(), propertyValue,
                       @"Setting symbolZOrder to a camera expression should update symbol-z-order.");
        XCTAssertEqualObjects(layer.symbolZOrder, functionExpression,
                              @"symbolZOrder should round-trip camera expressions.");


        layer.symbolZOrder = nil;
        XCTAssertTrue(rawLayer->getSymbolZOrder().isUndefined(),
                      @"Unsetting symbolZOrder should return symbol-z-order to the default value.");
        XCTAssertEqualObjects(layer.symbolZOrder, defaultExpression,
                              @"symbolZOrder should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.symbolZOrder = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.symbolZOrder = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-field
    {
        XCTAssertTrue(rawLayer->getTextField().isUndefined(),
                      @"text-field should be unset initially.");
        NSExpression *defaultExpression = layer.text;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'Text Field'"];
        layer.text = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::expression::Formatted> propertyValue = { "Text Field" };
        XCTAssertEqual(rawLayer->getTextField(), propertyValue,
                       @"Setting text to a constant value expression should update text-field.");
        XCTAssertEqualObjects(layer.text, constantExpression,
                              @"text should round-trip constant value expressions.");

        MGLAttributedExpression *attributedConstantExpression = [[MGLAttributedExpression alloc] initWithExpression:[NSExpression expressionWithFormat:@"'Text Field'"]
                                                                                                 attributes:@{}];
        constantExpression = [NSExpression mgl_expressionForAttributedExpressions:@[[NSExpression expressionForConstantValue:attributedConstantExpression]]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.text = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::expression::Formatted>(
                step(zoom(), format("Text Field"), 18.0, format("Text Field"))
            );
        }

        XCTAssertEqual(rawLayer->getTextField(), propertyValue,
                       @"Setting text to a camera expression should update text-field.");
        XCTAssertEqualObjects(layer.text, functionExpression,
                              @"text should round-trip camera expressions.");


        layer.text = nil;
        XCTAssertTrue(rawLayer->getTextField().isUndefined(),
                      @"Unsetting text should return text-field to the default value.");
        XCTAssertEqualObjects(layer.text, defaultExpression,
                              @"text should return the default value after being unset.");

        // Tokens test
        layer.text = [NSExpression expressionForConstantValue:@"{token}"];

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::expression::Formatted>(
                format(toString(get(literal("token"))))
            );
        }

        XCTAssertEqual(rawLayer->getTextField(), propertyValue,
                       @"Setting text to a constant string with tokens should convert to an expression.");

        MGLAttributedExpression *tokenAttibutedExpression = [[MGLAttributedExpression alloc] initWithExpression:[NSExpression expressionWithFormat:@"CAST(token, 'NSString')"]
                                                                                                     attributes:@{}];
        NSExpression* tokenExpression = [NSExpression mgl_expressionForAttributedExpressions:@[[NSExpression expressionForConstantValue:tokenAttibutedExpression]]];
        XCTAssertEqualObjects(layer.text, tokenExpression,
                              @"Setting text to a constant string with tokens should convert to an expression.");
    }

    // text-allow-overlap
    {
        XCTAssertTrue(rawLayer->getTextAllowOverlap().isUndefined(),
                      @"text-allow-overlap should be unset initially.");
        NSExpression *defaultExpression = layer.textAllowsOverlap;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"true"];
        layer.textAllowsOverlap = constantExpression;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getTextAllowOverlap(), propertyValue,
                       @"Setting textAllowsOverlap to a constant value expression should update text-allow-overlap.");
        XCTAssertEqualObjects(layer.textAllowsOverlap, constantExpression,
                              @"textAllowsOverlap should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"true"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textAllowsOverlap = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<bool>(
                step(zoom(), literal(true), 18.0, literal(true))
            );
        }

        XCTAssertEqual(rawLayer->getTextAllowOverlap(), propertyValue,
                       @"Setting textAllowsOverlap to a camera expression should update text-allow-overlap.");
        XCTAssertEqualObjects(layer.textAllowsOverlap, functionExpression,
                              @"textAllowsOverlap should round-trip camera expressions.");


        layer.textAllowsOverlap = nil;
        XCTAssertTrue(rawLayer->getTextAllowOverlap().isUndefined(),
                      @"Unsetting textAllowsOverlap should return text-allow-overlap to the default value.");
        XCTAssertEqualObjects(layer.textAllowsOverlap, defaultExpression,
                              @"textAllowsOverlap should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textAllowsOverlap = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textAllowsOverlap = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-anchor
    {
        XCTAssertTrue(rawLayer->getTextAnchor().isUndefined(),
                      @"text-anchor should be unset initially.");
        NSExpression *defaultExpression = layer.textAnchor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'bottom-right'"];
        layer.textAnchor = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::SymbolAnchorType> propertyValue = { mbgl::style::SymbolAnchorType::BottomRight };
        XCTAssertEqual(rawLayer->getTextAnchor(), propertyValue,
                       @"Setting textAnchor to a constant value expression should update text-anchor.");
        XCTAssertEqualObjects(layer.textAnchor, constantExpression,
                              @"textAnchor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'bottom-right'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textAnchor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::SymbolAnchorType>(
                step(zoom(), literal("bottom-right"), 18.0, literal("bottom-right"))
            );
        }

        XCTAssertEqual(rawLayer->getTextAnchor(), propertyValue,
                       @"Setting textAnchor to a camera expression should update text-anchor.");
        XCTAssertEqualObjects(layer.textAnchor, functionExpression,
                              @"textAnchor should round-trip camera expressions.");


        layer.textAnchor = nil;
        XCTAssertTrue(rawLayer->getTextAnchor().isUndefined(),
                      @"Unsetting textAnchor should return text-anchor to the default value.");
        XCTAssertEqualObjects(layer.textAnchor, defaultExpression,
                              @"textAnchor should return the default value after being unset.");
    }

    // text-font
    {
        XCTAssertTrue(rawLayer->getTextFont().isUndefined(),
                      @"text-font should be unset initially.");
        NSExpression *defaultExpression = layer.textFontNames;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"{'Text Font', 'Tnof Txet'}"];
        layer.textFontNames = constantExpression;
        mbgl::style::PropertyValue<std::vector<std::string>> propertyValue = { { "Text Font", "Tnof Txet" } };
        XCTAssertEqual(rawLayer->getTextFont(), propertyValue,
                       @"Setting textFontNames to a constant value expression should update text-font.");
        XCTAssertEqualObjects(layer.textFontNames, constantExpression,
                              @"textFontNames should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{'Text Font', 'Tnof Txet'}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textFontNames = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::vector<std::string>>(
                step(zoom(), literal({ "Text Font", "Tnof Txet" }), 18.0, literal({ "Text Font", "Tnof Txet" }))
            );
        }

        XCTAssertEqual(rawLayer->getTextFont(), propertyValue,
                       @"Setting textFontNames to a camera expression should update text-font.");
        XCTAssertEqualObjects(layer.textFontNames, functionExpression,
                              @"textFontNames should round-trip camera expressions.");


        layer.textFontNames = nil;
        XCTAssertTrue(rawLayer->getTextFont().isUndefined(),
                      @"Unsetting textFontNames should return text-font to the default value.");
        XCTAssertEqualObjects(layer.textFontNames, defaultExpression,
                              @"textFontNames should return the default value after being unset.");
    }

    // text-size
    {
        XCTAssertTrue(rawLayer->getTextSize().isUndefined(),
                      @"text-size should be unset initially.");
        NSExpression *defaultExpression = layer.textFontSize;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.textFontSize = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextSize(), propertyValue,
                       @"Setting textFontSize to a constant value expression should update text-size.");
        XCTAssertEqualObjects(layer.textFontSize, constantExpression,
                              @"textFontSize should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textFontSize = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextSize(), propertyValue,
                       @"Setting textFontSize to a camera expression should update text-size.");
        XCTAssertEqualObjects(layer.textFontSize, functionExpression,
                              @"textFontSize should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.textFontSize = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextSize(), propertyValue,
                       @"Setting textFontSize to a data expression should update text-size.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.textFontSize, pedanticFunctionExpression,
                              @"textFontSize should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.textFontSize = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getTextSize(), propertyValue,
                       @"Setting textFontSize to a camera-data expression should update text-size.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.textFontSize, pedanticFunctionExpression,
                              @"textFontSize should round-trip camera-data expressions.");

        layer.textFontSize = nil;
        XCTAssertTrue(rawLayer->getTextSize().isUndefined(),
                      @"Unsetting textFontSize should return text-size to the default value.");
        XCTAssertEqualObjects(layer.textFontSize, defaultExpression,
                              @"textFontSize should return the default value after being unset.");
    }

    // text-ignore-placement
    {
        XCTAssertTrue(rawLayer->getTextIgnorePlacement().isUndefined(),
                      @"text-ignore-placement should be unset initially.");
        NSExpression *defaultExpression = layer.textIgnoresPlacement;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"true"];
        layer.textIgnoresPlacement = constantExpression;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getTextIgnorePlacement(), propertyValue,
                       @"Setting textIgnoresPlacement to a constant value expression should update text-ignore-placement.");
        XCTAssertEqualObjects(layer.textIgnoresPlacement, constantExpression,
                              @"textIgnoresPlacement should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"true"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textIgnoresPlacement = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<bool>(
                step(zoom(), literal(true), 18.0, literal(true))
            );
        }

        XCTAssertEqual(rawLayer->getTextIgnorePlacement(), propertyValue,
                       @"Setting textIgnoresPlacement to a camera expression should update text-ignore-placement.");
        XCTAssertEqualObjects(layer.textIgnoresPlacement, functionExpression,
                              @"textIgnoresPlacement should round-trip camera expressions.");


        layer.textIgnoresPlacement = nil;
        XCTAssertTrue(rawLayer->getTextIgnorePlacement().isUndefined(),
                      @"Unsetting textIgnoresPlacement should return text-ignore-placement to the default value.");
        XCTAssertEqualObjects(layer.textIgnoresPlacement, defaultExpression,
                              @"textIgnoresPlacement should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textIgnoresPlacement = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textIgnoresPlacement = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-justify
    {
        XCTAssertTrue(rawLayer->getTextJustify().isUndefined(),
                      @"text-justify should be unset initially.");
        NSExpression *defaultExpression = layer.textJustification;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'right'"];
        layer.textJustification = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::TextJustifyType> propertyValue = { mbgl::style::TextJustifyType::Right };
        XCTAssertEqual(rawLayer->getTextJustify(), propertyValue,
                       @"Setting textJustification to a constant value expression should update text-justify.");
        XCTAssertEqualObjects(layer.textJustification, constantExpression,
                              @"textJustification should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'right'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textJustification = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::TextJustifyType>(
                step(zoom(), literal("right"), 18.0, literal("right"))
            );
        }

        XCTAssertEqual(rawLayer->getTextJustify(), propertyValue,
                       @"Setting textJustification to a camera expression should update text-justify.");
        XCTAssertEqualObjects(layer.textJustification, functionExpression,
                              @"textJustification should round-trip camera expressions.");


        layer.textJustification = nil;
        XCTAssertTrue(rawLayer->getTextJustify().isUndefined(),
                      @"Unsetting textJustification should return text-justify to the default value.");
        XCTAssertEqualObjects(layer.textJustification, defaultExpression,
                              @"textJustification should return the default value after being unset.");
    }

    // text-letter-spacing
    {
        XCTAssertTrue(rawLayer->getTextLetterSpacing().isUndefined(),
                      @"text-letter-spacing should be unset initially.");
        NSExpression *defaultExpression = layer.textLetterSpacing;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.textLetterSpacing = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextLetterSpacing(), propertyValue,
                       @"Setting textLetterSpacing to a constant value expression should update text-letter-spacing.");
        XCTAssertEqualObjects(layer.textLetterSpacing, constantExpression,
                              @"textLetterSpacing should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textLetterSpacing = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextLetterSpacing(), propertyValue,
                       @"Setting textLetterSpacing to a camera expression should update text-letter-spacing.");
        XCTAssertEqualObjects(layer.textLetterSpacing, functionExpression,
                              @"textLetterSpacing should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.textLetterSpacing = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextLetterSpacing(), propertyValue,
                       @"Setting textLetterSpacing to a data expression should update text-letter-spacing.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.textLetterSpacing, pedanticFunctionExpression,
                              @"textLetterSpacing should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.textLetterSpacing = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getTextLetterSpacing(), propertyValue,
                       @"Setting textLetterSpacing to a camera-data expression should update text-letter-spacing.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.textLetterSpacing, pedanticFunctionExpression,
                              @"textLetterSpacing should round-trip camera-data expressions.");

        layer.textLetterSpacing = nil;
        XCTAssertTrue(rawLayer->getTextLetterSpacing().isUndefined(),
                      @"Unsetting textLetterSpacing should return text-letter-spacing to the default value.");
        XCTAssertEqualObjects(layer.textLetterSpacing, defaultExpression,
                              @"textLetterSpacing should return the default value after being unset.");
    }

    // text-line-height
    {
        XCTAssertTrue(rawLayer->getTextLineHeight().isUndefined(),
                      @"text-line-height should be unset initially.");
        NSExpression *defaultExpression = layer.textLineHeight;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.textLineHeight = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextLineHeight(), propertyValue,
                       @"Setting textLineHeight to a constant value expression should update text-line-height.");
        XCTAssertEqualObjects(layer.textLineHeight, constantExpression,
                              @"textLineHeight should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textLineHeight = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextLineHeight(), propertyValue,
                       @"Setting textLineHeight to a camera expression should update text-line-height.");
        XCTAssertEqualObjects(layer.textLineHeight, functionExpression,
                              @"textLineHeight should round-trip camera expressions.");


        layer.textLineHeight = nil;
        XCTAssertTrue(rawLayer->getTextLineHeight().isUndefined(),
                      @"Unsetting textLineHeight should return text-line-height to the default value.");
        XCTAssertEqualObjects(layer.textLineHeight, defaultExpression,
                              @"textLineHeight should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textLineHeight = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textLineHeight = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-offset
    {
        XCTAssertTrue(rawLayer->getTextOffset().isUndefined(),
                      @"text-offset should be unset initially.");
        NSExpression *defaultExpression = layer.textOffset;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@",
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.textOffset = constantExpression;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getTextOffset(), propertyValue,
                       @"Setting textOffset to a constant value expression should update text-offset.");
        XCTAssertEqualObjects(layer.textOffset, constantExpression,
                              @"textOffset should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{1, 1}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textOffset = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::array<float, 2>>(
                step(zoom(), literal({ 1, 1 }), 18.0, literal({ 1, 1 }))
            );
        }

        XCTAssertEqual(rawLayer->getTextOffset(), propertyValue,
                       @"Setting textOffset to a camera expression should update text-offset.");
        XCTAssertEqualObjects(layer.textOffset, functionExpression,
                              @"textOffset should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.textOffset = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::array<float, 2>>(
                interpolate(linear(), number(get("keyName")), 18.0, literal({ 1, 1 }))
            );
        }

        XCTAssertEqual(rawLayer->getTextOffset(), propertyValue,
                       @"Setting textOffset to a data expression should update text-offset.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.textOffset, pedanticFunctionExpression,
                              @"textOffset should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.textOffset = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::array<float, 2>>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal({ 1, 1 })))
            );
        }

        XCTAssertEqual(rawLayer->getTextOffset(), propertyValue,
                       @"Setting textOffset to a camera-data expression should update text-offset.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.textOffset, pedanticFunctionExpression,
                              @"textOffset should round-trip camera-data expressions.");

        layer.textOffset = nil;
        XCTAssertTrue(rawLayer->getTextOffset().isUndefined(),
                      @"Unsetting textOffset should return text-offset to the default value.");
        XCTAssertEqualObjects(layer.textOffset, defaultExpression,
                              @"textOffset should return the default value after being unset.");
    }

    // text-optional
    {
        XCTAssertTrue(rawLayer->getTextOptional().isUndefined(),
                      @"text-optional should be unset initially.");
        NSExpression *defaultExpression = layer.textOptional;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"true"];
        layer.textOptional = constantExpression;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getTextOptional(), propertyValue,
                       @"Setting textOptional to a constant value expression should update text-optional.");
        XCTAssertEqualObjects(layer.textOptional, constantExpression,
                              @"textOptional should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"true"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textOptional = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<bool>(
                step(zoom(), literal(true), 18.0, literal(true))
            );
        }

        XCTAssertEqual(rawLayer->getTextOptional(), propertyValue,
                       @"Setting textOptional to a camera expression should update text-optional.");
        XCTAssertEqualObjects(layer.textOptional, functionExpression,
                              @"textOptional should round-trip camera expressions.");


        layer.textOptional = nil;
        XCTAssertTrue(rawLayer->getTextOptional().isUndefined(),
                      @"Unsetting textOptional should return text-optional to the default value.");
        XCTAssertEqualObjects(layer.textOptional, defaultExpression,
                              @"textOptional should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textOptional = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textOptional = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-padding
    {
        XCTAssertTrue(rawLayer->getTextPadding().isUndefined(),
                      @"text-padding should be unset initially.");
        NSExpression *defaultExpression = layer.textPadding;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.textPadding = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextPadding(), propertyValue,
                       @"Setting textPadding to a constant value expression should update text-padding.");
        XCTAssertEqualObjects(layer.textPadding, constantExpression,
                              @"textPadding should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textPadding = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextPadding(), propertyValue,
                       @"Setting textPadding to a camera expression should update text-padding.");
        XCTAssertEqualObjects(layer.textPadding, functionExpression,
                              @"textPadding should round-trip camera expressions.");


        layer.textPadding = nil;
        XCTAssertTrue(rawLayer->getTextPadding().isUndefined(),
                      @"Unsetting textPadding should return text-padding to the default value.");
        XCTAssertEqualObjects(layer.textPadding, defaultExpression,
                              @"textPadding should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textPadding = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textPadding = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-pitch-alignment
    {
        XCTAssertTrue(rawLayer->getTextPitchAlignment().isUndefined(),
                      @"text-pitch-alignment should be unset initially.");
        NSExpression *defaultExpression = layer.textPitchAlignment;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'auto'"];
        layer.textPitchAlignment = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Auto };
        XCTAssertEqual(rawLayer->getTextPitchAlignment(), propertyValue,
                       @"Setting textPitchAlignment to a constant value expression should update text-pitch-alignment.");
        XCTAssertEqualObjects(layer.textPitchAlignment, constantExpression,
                              @"textPitchAlignment should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'auto'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textPitchAlignment = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::AlignmentType>(
                step(zoom(), literal("auto"), 18.0, literal("auto"))
            );
        }

        XCTAssertEqual(rawLayer->getTextPitchAlignment(), propertyValue,
                       @"Setting textPitchAlignment to a camera expression should update text-pitch-alignment.");
        XCTAssertEqualObjects(layer.textPitchAlignment, functionExpression,
                              @"textPitchAlignment should round-trip camera expressions.");


        layer.textPitchAlignment = nil;
        XCTAssertTrue(rawLayer->getTextPitchAlignment().isUndefined(),
                      @"Unsetting textPitchAlignment should return text-pitch-alignment to the default value.");
        XCTAssertEqualObjects(layer.textPitchAlignment, defaultExpression,
                              @"textPitchAlignment should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textPitchAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textPitchAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-radial-offset
    {
        XCTAssertTrue(rawLayer->getTextRadialOffset().isUndefined(),
                      @"text-radial-offset should be unset initially.");
        NSExpression *defaultExpression = layer.textRadialOffset;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.textRadialOffset = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextRadialOffset(), propertyValue,
                       @"Setting textRadialOffset to a constant value expression should update text-radial-offset.");
        XCTAssertEqualObjects(layer.textRadialOffset, constantExpression,
                              @"textRadialOffset should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textRadialOffset = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextRadialOffset(), propertyValue,
                       @"Setting textRadialOffset to a camera expression should update text-radial-offset.");
        XCTAssertEqualObjects(layer.textRadialOffset, functionExpression,
                              @"textRadialOffset should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.textRadialOffset = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextRadialOffset(), propertyValue,
                       @"Setting textRadialOffset to a data expression should update text-radial-offset.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.textRadialOffset, pedanticFunctionExpression,
                              @"textRadialOffset should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.textRadialOffset = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getTextRadialOffset(), propertyValue,
                       @"Setting textRadialOffset to a camera-data expression should update text-radial-offset.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.textRadialOffset, pedanticFunctionExpression,
                              @"textRadialOffset should round-trip camera-data expressions.");

        layer.textRadialOffset = nil;
        XCTAssertTrue(rawLayer->getTextRadialOffset().isUndefined(),
                      @"Unsetting textRadialOffset should return text-radial-offset to the default value.");
        XCTAssertEqualObjects(layer.textRadialOffset, defaultExpression,
                              @"textRadialOffset should return the default value after being unset.");
    }

    // text-rotate
    {
        XCTAssertTrue(rawLayer->getTextRotate().isUndefined(),
                      @"text-rotate should be unset initially.");
        NSExpression *defaultExpression = layer.textRotation;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.textRotation = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextRotate(), propertyValue,
                       @"Setting textRotation to a constant value expression should update text-rotate.");
        XCTAssertEqualObjects(layer.textRotation, constantExpression,
                              @"textRotation should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textRotation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextRotate(), propertyValue,
                       @"Setting textRotation to a camera expression should update text-rotate.");
        XCTAssertEqualObjects(layer.textRotation, functionExpression,
                              @"textRotation should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.textRotation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextRotate(), propertyValue,
                       @"Setting textRotation to a data expression should update text-rotate.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.textRotation, pedanticFunctionExpression,
                              @"textRotation should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.textRotation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getTextRotate(), propertyValue,
                       @"Setting textRotation to a camera-data expression should update text-rotate.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.textRotation, pedanticFunctionExpression,
                              @"textRotation should round-trip camera-data expressions.");

        layer.textRotation = nil;
        XCTAssertTrue(rawLayer->getTextRotate().isUndefined(),
                      @"Unsetting textRotation should return text-rotate to the default value.");
        XCTAssertEqualObjects(layer.textRotation, defaultExpression,
                              @"textRotation should return the default value after being unset.");
    }

    // text-rotation-alignment
    {
        XCTAssertTrue(rawLayer->getTextRotationAlignment().isUndefined(),
                      @"text-rotation-alignment should be unset initially.");
        NSExpression *defaultExpression = layer.textRotationAlignment;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'auto'"];
        layer.textRotationAlignment = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Auto };
        XCTAssertEqual(rawLayer->getTextRotationAlignment(), propertyValue,
                       @"Setting textRotationAlignment to a constant value expression should update text-rotation-alignment.");
        XCTAssertEqualObjects(layer.textRotationAlignment, constantExpression,
                              @"textRotationAlignment should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'auto'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textRotationAlignment = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::AlignmentType>(
                step(zoom(), literal("auto"), 18.0, literal("auto"))
            );
        }

        XCTAssertEqual(rawLayer->getTextRotationAlignment(), propertyValue,
                       @"Setting textRotationAlignment to a camera expression should update text-rotation-alignment.");
        XCTAssertEqualObjects(layer.textRotationAlignment, functionExpression,
                              @"textRotationAlignment should round-trip camera expressions.");


        layer.textRotationAlignment = nil;
        XCTAssertTrue(rawLayer->getTextRotationAlignment().isUndefined(),
                      @"Unsetting textRotationAlignment should return text-rotation-alignment to the default value.");
        XCTAssertEqualObjects(layer.textRotationAlignment, defaultExpression,
                              @"textRotationAlignment should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textRotationAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textRotationAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-transform
    {
        XCTAssertTrue(rawLayer->getTextTransform().isUndefined(),
                      @"text-transform should be unset initially.");
        NSExpression *defaultExpression = layer.textTransform;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'lowercase'"];
        layer.textTransform = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::TextTransformType> propertyValue = { mbgl::style::TextTransformType::Lowercase };
        XCTAssertEqual(rawLayer->getTextTransform(), propertyValue,
                       @"Setting textTransform to a constant value expression should update text-transform.");
        XCTAssertEqualObjects(layer.textTransform, constantExpression,
                              @"textTransform should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'lowercase'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textTransform = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::TextTransformType>(
                step(zoom(), literal("lowercase"), 18.0, literal("lowercase"))
            );
        }

        XCTAssertEqual(rawLayer->getTextTransform(), propertyValue,
                       @"Setting textTransform to a camera expression should update text-transform.");
        XCTAssertEqualObjects(layer.textTransform, functionExpression,
                              @"textTransform should round-trip camera expressions.");


        layer.textTransform = nil;
        XCTAssertTrue(rawLayer->getTextTransform().isUndefined(),
                      @"Unsetting textTransform should return text-transform to the default value.");
        XCTAssertEqualObjects(layer.textTransform, defaultExpression,
                              @"textTransform should return the default value after being unset.");
    }

    // text-variable-anchor
    {
        XCTAssertTrue(rawLayer->getTextVariableAnchor().isUndefined(),
                      @"text-variable-anchor should be unset initially.");
        NSExpression *defaultExpression = layer.textVariableAnchor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"{'top','bottom'}"];
        layer.textVariableAnchor = constantExpression;
        mbgl::style::PropertyValue<std::vector<mbgl::style::SymbolAnchorType>> propertyValue = { { mbgl::style::SymbolAnchorType::Top, mbgl::style::SymbolAnchorType::Bottom } };
        XCTAssertEqual(rawLayer->getTextVariableAnchor(), propertyValue,
                       @"Setting textVariableAnchor to a constant value expression should update text-variable-anchor.");
        XCTAssertEqualObjects(layer.textVariableAnchor, constantExpression,
                              @"textVariableAnchor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{'top','bottom'}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textVariableAnchor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::vector<mbgl::style::SymbolAnchorType>>(
                step(zoom(), literal({"top", "bottom"}), 18.0, literal({"top", "bottom"}))
            );
        }

        XCTAssertEqual(rawLayer->getTextVariableAnchor(), propertyValue,
                       @"Setting textVariableAnchor to a camera expression should update text-variable-anchor.");
        XCTAssertEqualObjects(layer.textVariableAnchor, functionExpression,
                              @"textVariableAnchor should round-trip camera expressions.");


        layer.textVariableAnchor = nil;
        XCTAssertTrue(rawLayer->getTextVariableAnchor().isUndefined(),
                      @"Unsetting textVariableAnchor should return text-variable-anchor to the default value.");
        XCTAssertEqualObjects(layer.textVariableAnchor, defaultExpression,
                              @"textVariableAnchor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textVariableAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textVariableAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-writing-mode
    {
        XCTAssertTrue(rawLayer->getTextWritingMode().isUndefined(),
                      @"text-writing-mode should be unset initially.");
        NSExpression *defaultExpression = layer.textWritingModes;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"{'horizontal','vertical'}"];
        layer.textWritingModes = constantExpression;
        mbgl::style::PropertyValue<std::vector<mbgl::style::TextWritingModeType>> propertyValue = { { mbgl::style::TextWritingModeType::Horizontal, mbgl::style::TextWritingModeType::Vertical } };
        XCTAssertEqual(rawLayer->getTextWritingMode(), propertyValue,
                       @"Setting textWritingModes to a constant value expression should update text-writing-mode.");
        XCTAssertEqualObjects(layer.textWritingModes, constantExpression,
                              @"textWritingModes should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{'horizontal','vertical'}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textWritingModes = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::vector<mbgl::style::TextWritingModeType>>(
                step(zoom(), literal({"horizontal", "vertical"}), 18.0, literal({"horizontal", "vertical"}))
            );
        }

        XCTAssertEqual(rawLayer->getTextWritingMode(), propertyValue,
                       @"Setting textWritingModes to a camera expression should update text-writing-mode.");
        XCTAssertEqualObjects(layer.textWritingModes, functionExpression,
                              @"textWritingModes should round-trip camera expressions.");


        layer.textWritingModes = nil;
        XCTAssertTrue(rawLayer->getTextWritingMode().isUndefined(),
                      @"Unsetting textWritingModes should return text-writing-mode to the default value.");
        XCTAssertEqualObjects(layer.textWritingModes, defaultExpression,
                              @"textWritingModes should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textWritingModes = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textWritingModes = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // icon-color
    {
        XCTAssertTrue(rawLayer->getIconColor().isUndefined(),
                      @"icon-color should be unset initially.");
        NSExpression *defaultExpression = layer.iconColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.iconColor = constantExpression;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getIconColor(), propertyValue,
                       @"Setting iconColor to a constant value expression should update icon-color.");
        XCTAssertEqualObjects(layer.iconColor, constantExpression,
                              @"iconColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                step(zoom(), literal(mbgl::Color(1, 0, 0, 1)), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getIconColor(), propertyValue,
                       @"Setting iconColor to a camera expression should update icon-color.");
        XCTAssertEqualObjects(layer.iconColor, functionExpression,
                              @"iconColor should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.iconColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getIconColor(), propertyValue,
                       @"Setting iconColor to a data expression should update icon-color.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.iconColor, pedanticFunctionExpression,
                              @"iconColor should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.iconColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(mbgl::Color(1, 0, 0, 1))))
            );
        }

        XCTAssertEqual(rawLayer->getIconColor(), propertyValue,
                       @"Setting iconColor to a camera-data expression should update icon-color.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.iconColor, pedanticFunctionExpression,
                              @"iconColor should round-trip camera-data expressions.");

        layer.iconColor = nil;
        XCTAssertTrue(rawLayer->getIconColor().isUndefined(),
                      @"Unsetting iconColor should return icon-color to the default value.");
        XCTAssertEqualObjects(layer.iconColor, defaultExpression,
                              @"iconColor should return the default value after being unset.");
        // Transition property test
        layer.iconColorTransition = transitionTest;
        auto toptions = rawLayer->getIconColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition iconColorTransition = layer.iconColorTransition;
        XCTAssertEqual(iconColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(iconColorTransition.duration, transitionTest.duration);
    }

    // icon-halo-blur
    {
        XCTAssertTrue(rawLayer->getIconHaloBlur().isUndefined(),
                      @"icon-halo-blur should be unset initially.");
        NSExpression *defaultExpression = layer.iconHaloBlur;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.iconHaloBlur = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getIconHaloBlur(), propertyValue,
                       @"Setting iconHaloBlur to a constant value expression should update icon-halo-blur.");
        XCTAssertEqualObjects(layer.iconHaloBlur, constantExpression,
                              @"iconHaloBlur should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconHaloBlur = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconHaloBlur(), propertyValue,
                       @"Setting iconHaloBlur to a camera expression should update icon-halo-blur.");
        XCTAssertEqualObjects(layer.iconHaloBlur, functionExpression,
                              @"iconHaloBlur should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.iconHaloBlur = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconHaloBlur(), propertyValue,
                       @"Setting iconHaloBlur to a data expression should update icon-halo-blur.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.iconHaloBlur, pedanticFunctionExpression,
                              @"iconHaloBlur should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.iconHaloBlur = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getIconHaloBlur(), propertyValue,
                       @"Setting iconHaloBlur to a camera-data expression should update icon-halo-blur.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.iconHaloBlur, pedanticFunctionExpression,
                              @"iconHaloBlur should round-trip camera-data expressions.");

        layer.iconHaloBlur = nil;
        XCTAssertTrue(rawLayer->getIconHaloBlur().isUndefined(),
                      @"Unsetting iconHaloBlur should return icon-halo-blur to the default value.");
        XCTAssertEqualObjects(layer.iconHaloBlur, defaultExpression,
                              @"iconHaloBlur should return the default value after being unset.");
        // Transition property test
        layer.iconHaloBlurTransition = transitionTest;
        auto toptions = rawLayer->getIconHaloBlurTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition iconHaloBlurTransition = layer.iconHaloBlurTransition;
        XCTAssertEqual(iconHaloBlurTransition.delay, transitionTest.delay);
        XCTAssertEqual(iconHaloBlurTransition.duration, transitionTest.duration);
    }

    // icon-halo-color
    {
        XCTAssertTrue(rawLayer->getIconHaloColor().isUndefined(),
                      @"icon-halo-color should be unset initially.");
        NSExpression *defaultExpression = layer.iconHaloColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.iconHaloColor = constantExpression;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getIconHaloColor(), propertyValue,
                       @"Setting iconHaloColor to a constant value expression should update icon-halo-color.");
        XCTAssertEqualObjects(layer.iconHaloColor, constantExpression,
                              @"iconHaloColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconHaloColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                step(zoom(), literal(mbgl::Color(1, 0, 0, 1)), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getIconHaloColor(), propertyValue,
                       @"Setting iconHaloColor to a camera expression should update icon-halo-color.");
        XCTAssertEqualObjects(layer.iconHaloColor, functionExpression,
                              @"iconHaloColor should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.iconHaloColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getIconHaloColor(), propertyValue,
                       @"Setting iconHaloColor to a data expression should update icon-halo-color.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.iconHaloColor, pedanticFunctionExpression,
                              @"iconHaloColor should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.iconHaloColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(mbgl::Color(1, 0, 0, 1))))
            );
        }

        XCTAssertEqual(rawLayer->getIconHaloColor(), propertyValue,
                       @"Setting iconHaloColor to a camera-data expression should update icon-halo-color.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.iconHaloColor, pedanticFunctionExpression,
                              @"iconHaloColor should round-trip camera-data expressions.");

        layer.iconHaloColor = nil;
        XCTAssertTrue(rawLayer->getIconHaloColor().isUndefined(),
                      @"Unsetting iconHaloColor should return icon-halo-color to the default value.");
        XCTAssertEqualObjects(layer.iconHaloColor, defaultExpression,
                              @"iconHaloColor should return the default value after being unset.");
        // Transition property test
        layer.iconHaloColorTransition = transitionTest;
        auto toptions = rawLayer->getIconHaloColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition iconHaloColorTransition = layer.iconHaloColorTransition;
        XCTAssertEqual(iconHaloColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(iconHaloColorTransition.duration, transitionTest.duration);
    }

    // icon-halo-width
    {
        XCTAssertTrue(rawLayer->getIconHaloWidth().isUndefined(),
                      @"icon-halo-width should be unset initially.");
        NSExpression *defaultExpression = layer.iconHaloWidth;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.iconHaloWidth = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getIconHaloWidth(), propertyValue,
                       @"Setting iconHaloWidth to a constant value expression should update icon-halo-width.");
        XCTAssertEqualObjects(layer.iconHaloWidth, constantExpression,
                              @"iconHaloWidth should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconHaloWidth = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconHaloWidth(), propertyValue,
                       @"Setting iconHaloWidth to a camera expression should update icon-halo-width.");
        XCTAssertEqualObjects(layer.iconHaloWidth, functionExpression,
                              @"iconHaloWidth should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.iconHaloWidth = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconHaloWidth(), propertyValue,
                       @"Setting iconHaloWidth to a data expression should update icon-halo-width.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.iconHaloWidth, pedanticFunctionExpression,
                              @"iconHaloWidth should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.iconHaloWidth = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getIconHaloWidth(), propertyValue,
                       @"Setting iconHaloWidth to a camera-data expression should update icon-halo-width.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.iconHaloWidth, pedanticFunctionExpression,
                              @"iconHaloWidth should round-trip camera-data expressions.");

        layer.iconHaloWidth = nil;
        XCTAssertTrue(rawLayer->getIconHaloWidth().isUndefined(),
                      @"Unsetting iconHaloWidth should return icon-halo-width to the default value.");
        XCTAssertEqualObjects(layer.iconHaloWidth, defaultExpression,
                              @"iconHaloWidth should return the default value after being unset.");
        // Transition property test
        layer.iconHaloWidthTransition = transitionTest;
        auto toptions = rawLayer->getIconHaloWidthTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition iconHaloWidthTransition = layer.iconHaloWidthTransition;
        XCTAssertEqual(iconHaloWidthTransition.delay, transitionTest.delay);
        XCTAssertEqual(iconHaloWidthTransition.duration, transitionTest.duration);
    }

    // icon-opacity
    {
        XCTAssertTrue(rawLayer->getIconOpacity().isUndefined(),
                      @"icon-opacity should be unset initially.");
        NSExpression *defaultExpression = layer.iconOpacity;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.iconOpacity = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getIconOpacity(), propertyValue,
                       @"Setting iconOpacity to a constant value expression should update icon-opacity.");
        XCTAssertEqualObjects(layer.iconOpacity, constantExpression,
                              @"iconOpacity should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconOpacity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconOpacity(), propertyValue,
                       @"Setting iconOpacity to a camera expression should update icon-opacity.");
        XCTAssertEqualObjects(layer.iconOpacity, functionExpression,
                              @"iconOpacity should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.iconOpacity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getIconOpacity(), propertyValue,
                       @"Setting iconOpacity to a data expression should update icon-opacity.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.iconOpacity, pedanticFunctionExpression,
                              @"iconOpacity should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.iconOpacity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getIconOpacity(), propertyValue,
                       @"Setting iconOpacity to a camera-data expression should update icon-opacity.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.iconOpacity, pedanticFunctionExpression,
                              @"iconOpacity should round-trip camera-data expressions.");

        layer.iconOpacity = nil;
        XCTAssertTrue(rawLayer->getIconOpacity().isUndefined(),
                      @"Unsetting iconOpacity should return icon-opacity to the default value.");
        XCTAssertEqualObjects(layer.iconOpacity, defaultExpression,
                              @"iconOpacity should return the default value after being unset.");
        // Transition property test
        layer.iconOpacityTransition = transitionTest;
        auto toptions = rawLayer->getIconOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition iconOpacityTransition = layer.iconOpacityTransition;
        XCTAssertEqual(iconOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(iconOpacityTransition.duration, transitionTest.duration);
    }

    // icon-translate
    {
        XCTAssertTrue(rawLayer->getIconTranslate().isUndefined(),
                      @"icon-translate should be unset initially.");
        NSExpression *defaultExpression = layer.iconTranslation;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@",
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.iconTranslation = constantExpression;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getIconTranslate(), propertyValue,
                       @"Setting iconTranslation to a constant value expression should update icon-translate.");
        XCTAssertEqualObjects(layer.iconTranslation, constantExpression,
                              @"iconTranslation should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{1, 1}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconTranslation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::array<float, 2>>(
                step(zoom(), literal({ 1, 1 }), 18.0, literal({ 1, 1 }))
            );
        }

        XCTAssertEqual(rawLayer->getIconTranslate(), propertyValue,
                       @"Setting iconTranslation to a camera expression should update icon-translate.");
        XCTAssertEqualObjects(layer.iconTranslation, functionExpression,
                              @"iconTranslation should round-trip camera expressions.");


        layer.iconTranslation = nil;
        XCTAssertTrue(rawLayer->getIconTranslate().isUndefined(),
                      @"Unsetting iconTranslation should return icon-translate to the default value.");
        XCTAssertEqualObjects(layer.iconTranslation, defaultExpression,
                              @"iconTranslation should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.iconTranslation = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.iconTranslation = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // icon-translate-anchor
    {
        XCTAssertTrue(rawLayer->getIconTranslateAnchor().isUndefined(),
                      @"icon-translate-anchor should be unset initially.");
        NSExpression *defaultExpression = layer.iconTranslationAnchor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        layer.iconTranslationAnchor = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getIconTranslateAnchor(), propertyValue,
                       @"Setting iconTranslationAnchor to a constant value expression should update icon-translate-anchor.");
        XCTAssertEqualObjects(layer.iconTranslationAnchor, constantExpression,
                              @"iconTranslationAnchor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.iconTranslationAnchor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::TranslateAnchorType>(
                step(zoom(), literal("viewport"), 18.0, literal("viewport"))
            );
        }

        XCTAssertEqual(rawLayer->getIconTranslateAnchor(), propertyValue,
                       @"Setting iconTranslationAnchor to a camera expression should update icon-translate-anchor.");
        XCTAssertEqualObjects(layer.iconTranslationAnchor, functionExpression,
                              @"iconTranslationAnchor should round-trip camera expressions.");


        layer.iconTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getIconTranslateAnchor().isUndefined(),
                      @"Unsetting iconTranslationAnchor should return icon-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.iconTranslationAnchor, defaultExpression,
                              @"iconTranslationAnchor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.iconTranslationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.iconTranslationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-color
    {
        XCTAssertTrue(rawLayer->getTextColor().isUndefined(),
                      @"text-color should be unset initially.");
        NSExpression *defaultExpression = layer.textColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.textColor = constantExpression;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getTextColor(), propertyValue,
                       @"Setting textColor to a constant value expression should update text-color.");
        XCTAssertEqualObjects(layer.textColor, constantExpression,
                              @"textColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                step(zoom(), literal(mbgl::Color(1, 0, 0, 1)), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getTextColor(), propertyValue,
                       @"Setting textColor to a camera expression should update text-color.");
        XCTAssertEqualObjects(layer.textColor, functionExpression,
                              @"textColor should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.textColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getTextColor(), propertyValue,
                       @"Setting textColor to a data expression should update text-color.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.textColor, pedanticFunctionExpression,
                              @"textColor should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.textColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(mbgl::Color(1, 0, 0, 1))))
            );
        }

        XCTAssertEqual(rawLayer->getTextColor(), propertyValue,
                       @"Setting textColor to a camera-data expression should update text-color.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.textColor, pedanticFunctionExpression,
                              @"textColor should round-trip camera-data expressions.");

        layer.textColor = nil;
        XCTAssertTrue(rawLayer->getTextColor().isUndefined(),
                      @"Unsetting textColor should return text-color to the default value.");
        XCTAssertEqualObjects(layer.textColor, defaultExpression,
                              @"textColor should return the default value after being unset.");
        // Transition property test
        layer.textColorTransition = transitionTest;
        auto toptions = rawLayer->getTextColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition textColorTransition = layer.textColorTransition;
        XCTAssertEqual(textColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(textColorTransition.duration, transitionTest.duration);
    }

    // text-halo-blur
    {
        XCTAssertTrue(rawLayer->getTextHaloBlur().isUndefined(),
                      @"text-halo-blur should be unset initially.");
        NSExpression *defaultExpression = layer.textHaloBlur;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.textHaloBlur = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextHaloBlur(), propertyValue,
                       @"Setting textHaloBlur to a constant value expression should update text-halo-blur.");
        XCTAssertEqualObjects(layer.textHaloBlur, constantExpression,
                              @"textHaloBlur should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textHaloBlur = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextHaloBlur(), propertyValue,
                       @"Setting textHaloBlur to a camera expression should update text-halo-blur.");
        XCTAssertEqualObjects(layer.textHaloBlur, functionExpression,
                              @"textHaloBlur should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.textHaloBlur = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextHaloBlur(), propertyValue,
                       @"Setting textHaloBlur to a data expression should update text-halo-blur.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.textHaloBlur, pedanticFunctionExpression,
                              @"textHaloBlur should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.textHaloBlur = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getTextHaloBlur(), propertyValue,
                       @"Setting textHaloBlur to a camera-data expression should update text-halo-blur.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.textHaloBlur, pedanticFunctionExpression,
                              @"textHaloBlur should round-trip camera-data expressions.");

        layer.textHaloBlur = nil;
        XCTAssertTrue(rawLayer->getTextHaloBlur().isUndefined(),
                      @"Unsetting textHaloBlur should return text-halo-blur to the default value.");
        XCTAssertEqualObjects(layer.textHaloBlur, defaultExpression,
                              @"textHaloBlur should return the default value after being unset.");
        // Transition property test
        layer.textHaloBlurTransition = transitionTest;
        auto toptions = rawLayer->getTextHaloBlurTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition textHaloBlurTransition = layer.textHaloBlurTransition;
        XCTAssertEqual(textHaloBlurTransition.delay, transitionTest.delay);
        XCTAssertEqual(textHaloBlurTransition.duration, transitionTest.duration);
    }

    // text-halo-color
    {
        XCTAssertTrue(rawLayer->getTextHaloColor().isUndefined(),
                      @"text-halo-color should be unset initially.");
        NSExpression *defaultExpression = layer.textHaloColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.textHaloColor = constantExpression;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getTextHaloColor(), propertyValue,
                       @"Setting textHaloColor to a constant value expression should update text-halo-color.");
        XCTAssertEqualObjects(layer.textHaloColor, constantExpression,
                              @"textHaloColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textHaloColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                step(zoom(), literal(mbgl::Color(1, 0, 0, 1)), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getTextHaloColor(), propertyValue,
                       @"Setting textHaloColor to a camera expression should update text-halo-color.");
        XCTAssertEqualObjects(layer.textHaloColor, functionExpression,
                              @"textHaloColor should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.textHaloColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getTextHaloColor(), propertyValue,
                       @"Setting textHaloColor to a data expression should update text-halo-color.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.textHaloColor, pedanticFunctionExpression,
                              @"textHaloColor should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.textHaloColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(mbgl::Color(1, 0, 0, 1))))
            );
        }

        XCTAssertEqual(rawLayer->getTextHaloColor(), propertyValue,
                       @"Setting textHaloColor to a camera-data expression should update text-halo-color.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.textHaloColor, pedanticFunctionExpression,
                              @"textHaloColor should round-trip camera-data expressions.");

        layer.textHaloColor = nil;
        XCTAssertTrue(rawLayer->getTextHaloColor().isUndefined(),
                      @"Unsetting textHaloColor should return text-halo-color to the default value.");
        XCTAssertEqualObjects(layer.textHaloColor, defaultExpression,
                              @"textHaloColor should return the default value after being unset.");
        // Transition property test
        layer.textHaloColorTransition = transitionTest;
        auto toptions = rawLayer->getTextHaloColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition textHaloColorTransition = layer.textHaloColorTransition;
        XCTAssertEqual(textHaloColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(textHaloColorTransition.duration, transitionTest.duration);
    }

    // text-halo-width
    {
        XCTAssertTrue(rawLayer->getTextHaloWidth().isUndefined(),
                      @"text-halo-width should be unset initially.");
        NSExpression *defaultExpression = layer.textHaloWidth;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.textHaloWidth = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextHaloWidth(), propertyValue,
                       @"Setting textHaloWidth to a constant value expression should update text-halo-width.");
        XCTAssertEqualObjects(layer.textHaloWidth, constantExpression,
                              @"textHaloWidth should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textHaloWidth = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextHaloWidth(), propertyValue,
                       @"Setting textHaloWidth to a camera expression should update text-halo-width.");
        XCTAssertEqualObjects(layer.textHaloWidth, functionExpression,
                              @"textHaloWidth should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.textHaloWidth = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextHaloWidth(), propertyValue,
                       @"Setting textHaloWidth to a data expression should update text-halo-width.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.textHaloWidth, pedanticFunctionExpression,
                              @"textHaloWidth should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.textHaloWidth = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getTextHaloWidth(), propertyValue,
                       @"Setting textHaloWidth to a camera-data expression should update text-halo-width.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.textHaloWidth, pedanticFunctionExpression,
                              @"textHaloWidth should round-trip camera-data expressions.");

        layer.textHaloWidth = nil;
        XCTAssertTrue(rawLayer->getTextHaloWidth().isUndefined(),
                      @"Unsetting textHaloWidth should return text-halo-width to the default value.");
        XCTAssertEqualObjects(layer.textHaloWidth, defaultExpression,
                              @"textHaloWidth should return the default value after being unset.");
        // Transition property test
        layer.textHaloWidthTransition = transitionTest;
        auto toptions = rawLayer->getTextHaloWidthTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition textHaloWidthTransition = layer.textHaloWidthTransition;
        XCTAssertEqual(textHaloWidthTransition.delay, transitionTest.delay);
        XCTAssertEqual(textHaloWidthTransition.duration, transitionTest.duration);
    }

    // text-opacity
    {
        XCTAssertTrue(rawLayer->getTextOpacity().isUndefined(),
                      @"text-opacity should be unset initially.");
        NSExpression *defaultExpression = layer.textOpacity;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.textOpacity = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getTextOpacity(), propertyValue,
                       @"Setting textOpacity to a constant value expression should update text-opacity.");
        XCTAssertEqualObjects(layer.textOpacity, constantExpression,
                              @"textOpacity should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textOpacity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextOpacity(), propertyValue,
                       @"Setting textOpacity to a camera expression should update text-opacity.");
        XCTAssertEqualObjects(layer.textOpacity, functionExpression,
                              @"textOpacity should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.textOpacity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getTextOpacity(), propertyValue,
                       @"Setting textOpacity to a data expression should update text-opacity.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.textOpacity, pedanticFunctionExpression,
                              @"textOpacity should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.textOpacity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getTextOpacity(), propertyValue,
                       @"Setting textOpacity to a camera-data expression should update text-opacity.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.textOpacity, pedanticFunctionExpression,
                              @"textOpacity should round-trip camera-data expressions.");

        layer.textOpacity = nil;
        XCTAssertTrue(rawLayer->getTextOpacity().isUndefined(),
                      @"Unsetting textOpacity should return text-opacity to the default value.");
        XCTAssertEqualObjects(layer.textOpacity, defaultExpression,
                              @"textOpacity should return the default value after being unset.");
        // Transition property test
        layer.textOpacityTransition = transitionTest;
        auto toptions = rawLayer->getTextOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition textOpacityTransition = layer.textOpacityTransition;
        XCTAssertEqual(textOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(textOpacityTransition.duration, transitionTest.duration);
    }

    // text-translate
    {
        XCTAssertTrue(rawLayer->getTextTranslate().isUndefined(),
                      @"text-translate should be unset initially.");
        NSExpression *defaultExpression = layer.textTranslation;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@",
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.textTranslation = constantExpression;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getTextTranslate(), propertyValue,
                       @"Setting textTranslation to a constant value expression should update text-translate.");
        XCTAssertEqualObjects(layer.textTranslation, constantExpression,
                              @"textTranslation should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{1, 1}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textTranslation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::array<float, 2>>(
                step(zoom(), literal({ 1, 1 }), 18.0, literal({ 1, 1 }))
            );
        }

        XCTAssertEqual(rawLayer->getTextTranslate(), propertyValue,
                       @"Setting textTranslation to a camera expression should update text-translate.");
        XCTAssertEqualObjects(layer.textTranslation, functionExpression,
                              @"textTranslation should round-trip camera expressions.");


        layer.textTranslation = nil;
        XCTAssertTrue(rawLayer->getTextTranslate().isUndefined(),
                      @"Unsetting textTranslation should return text-translate to the default value.");
        XCTAssertEqualObjects(layer.textTranslation, defaultExpression,
                              @"textTranslation should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textTranslation = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textTranslation = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // text-translate-anchor
    {
        XCTAssertTrue(rawLayer->getTextTranslateAnchor().isUndefined(),
                      @"text-translate-anchor should be unset initially.");
        NSExpression *defaultExpression = layer.textTranslationAnchor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        layer.textTranslationAnchor = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getTextTranslateAnchor(), propertyValue,
                       @"Setting textTranslationAnchor to a constant value expression should update text-translate-anchor.");
        XCTAssertEqualObjects(layer.textTranslationAnchor, constantExpression,
                              @"textTranslationAnchor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.textTranslationAnchor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::TranslateAnchorType>(
                step(zoom(), literal("viewport"), 18.0, literal("viewport"))
            );
        }

        XCTAssertEqual(rawLayer->getTextTranslateAnchor(), propertyValue,
                       @"Setting textTranslationAnchor to a camera expression should update text-translate-anchor.");
        XCTAssertEqualObjects(layer.textTranslationAnchor, functionExpression,
                              @"textTranslationAnchor should round-trip camera expressions.");


        layer.textTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getTextTranslateAnchor().isUndefined(),
                      @"Unsetting textTranslationAnchor should return text-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.textTranslationAnchor, defaultExpression,
                              @"textTranslationAnchor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.textTranslationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.textTranslationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLSymbolLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"icon-allows-overlap" isBoolean:YES];
    [self testPropertyName:@"icon-anchor" isBoolean:NO];
    [self testPropertyName:@"icon-ignores-placement" isBoolean:YES];
    [self testPropertyName:@"icon-image-name" isBoolean:NO];
    [self testPropertyName:@"icon-offset" isBoolean:NO];
    [self testPropertyName:@"is-icon-optional" isBoolean:YES];
    [self testPropertyName:@"icon-padding" isBoolean:NO];
    [self testPropertyName:@"icon-pitch-alignment" isBoolean:NO];
    [self testPropertyName:@"icon-rotation" isBoolean:NO];
    [self testPropertyName:@"icon-rotation-alignment" isBoolean:NO];
    [self testPropertyName:@"icon-scale" isBoolean:NO];
    [self testPropertyName:@"icon-text-fit" isBoolean:NO];
    [self testPropertyName:@"icon-text-fit-padding" isBoolean:NO];
    [self testPropertyName:@"keeps-icon-upright" isBoolean:YES];
    [self testPropertyName:@"keeps-text-upright" isBoolean:YES];
    [self testPropertyName:@"maximum-text-angle" isBoolean:NO];
    [self testPropertyName:@"maximum-text-width" isBoolean:NO];
    [self testPropertyName:@"symbol-avoids-edges" isBoolean:YES];
    [self testPropertyName:@"symbol-placement" isBoolean:NO];
    [self testPropertyName:@"symbol-sort-key" isBoolean:NO];
    [self testPropertyName:@"symbol-spacing" isBoolean:NO];
    [self testPropertyName:@"symbol-z-order" isBoolean:NO];
    [self testPropertyName:@"text" isBoolean:NO];
    [self testPropertyName:@"text-allows-overlap" isBoolean:YES];
    [self testPropertyName:@"text-anchor" isBoolean:NO];
    [self testPropertyName:@"text-font-names" isBoolean:NO];
    [self testPropertyName:@"text-font-size" isBoolean:NO];
    [self testPropertyName:@"text-ignores-placement" isBoolean:YES];
    [self testPropertyName:@"text-justification" isBoolean:NO];
    [self testPropertyName:@"text-letter-spacing" isBoolean:NO];
    [self testPropertyName:@"text-line-height" isBoolean:NO];
    [self testPropertyName:@"text-offset" isBoolean:NO];
    [self testPropertyName:@"is-text-optional" isBoolean:YES];
    [self testPropertyName:@"text-padding" isBoolean:NO];
    [self testPropertyName:@"text-pitch-alignment" isBoolean:NO];
    [self testPropertyName:@"text-radial-offset" isBoolean:NO];
    [self testPropertyName:@"text-rotation" isBoolean:NO];
    [self testPropertyName:@"text-rotation-alignment" isBoolean:NO];
    [self testPropertyName:@"text-transform" isBoolean:NO];
    [self testPropertyName:@"text-variable-anchor" isBoolean:NO];
    [self testPropertyName:@"text-writing-modes" isBoolean:NO];
    [self testPropertyName:@"icon-color" isBoolean:NO];
    [self testPropertyName:@"icon-halo-blur" isBoolean:NO];
    [self testPropertyName:@"icon-halo-color" isBoolean:NO];
    [self testPropertyName:@"icon-halo-width" isBoolean:NO];
    [self testPropertyName:@"icon-opacity" isBoolean:NO];
    [self testPropertyName:@"icon-translation" isBoolean:NO];
    [self testPropertyName:@"icon-translation-anchor" isBoolean:NO];
    [self testPropertyName:@"text-color" isBoolean:NO];
    [self testPropertyName:@"text-halo-blur" isBoolean:NO];
    [self testPropertyName:@"text-halo-color" isBoolean:NO];
    [self testPropertyName:@"text-halo-width" isBoolean:NO];
    [self testPropertyName:@"text-opacity" isBoolean:NO];
    [self testPropertyName:@"text-translation" isBoolean:NO];
    [self testPropertyName:@"text-translation-anchor" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLIconAnchor:MGLIconAnchorCenter].MGLIconAnchorValue, MGLIconAnchorCenter);
    XCTAssertEqual([NSValue valueWithMGLIconAnchor:MGLIconAnchorLeft].MGLIconAnchorValue, MGLIconAnchorLeft);
    XCTAssertEqual([NSValue valueWithMGLIconAnchor:MGLIconAnchorRight].MGLIconAnchorValue, MGLIconAnchorRight);
    XCTAssertEqual([NSValue valueWithMGLIconAnchor:MGLIconAnchorTop].MGLIconAnchorValue, MGLIconAnchorTop);
    XCTAssertEqual([NSValue valueWithMGLIconAnchor:MGLIconAnchorBottom].MGLIconAnchorValue, MGLIconAnchorBottom);
    XCTAssertEqual([NSValue valueWithMGLIconAnchor:MGLIconAnchorTopLeft].MGLIconAnchorValue, MGLIconAnchorTopLeft);
    XCTAssertEqual([NSValue valueWithMGLIconAnchor:MGLIconAnchorTopRight].MGLIconAnchorValue, MGLIconAnchorTopRight);
    XCTAssertEqual([NSValue valueWithMGLIconAnchor:MGLIconAnchorBottomLeft].MGLIconAnchorValue, MGLIconAnchorBottomLeft);
    XCTAssertEqual([NSValue valueWithMGLIconAnchor:MGLIconAnchorBottomRight].MGLIconAnchorValue, MGLIconAnchorBottomRight);
    XCTAssertEqual([NSValue valueWithMGLIconPitchAlignment:MGLIconPitchAlignmentMap].MGLIconPitchAlignmentValue, MGLIconPitchAlignmentMap);
    XCTAssertEqual([NSValue valueWithMGLIconPitchAlignment:MGLIconPitchAlignmentViewport].MGLIconPitchAlignmentValue, MGLIconPitchAlignmentViewport);
    XCTAssertEqual([NSValue valueWithMGLIconPitchAlignment:MGLIconPitchAlignmentAuto].MGLIconPitchAlignmentValue, MGLIconPitchAlignmentAuto);
    XCTAssertEqual([NSValue valueWithMGLIconRotationAlignment:MGLIconRotationAlignmentMap].MGLIconRotationAlignmentValue, MGLIconRotationAlignmentMap);
    XCTAssertEqual([NSValue valueWithMGLIconRotationAlignment:MGLIconRotationAlignmentViewport].MGLIconRotationAlignmentValue, MGLIconRotationAlignmentViewport);
    XCTAssertEqual([NSValue valueWithMGLIconRotationAlignment:MGLIconRotationAlignmentAuto].MGLIconRotationAlignmentValue, MGLIconRotationAlignmentAuto);
    XCTAssertEqual([NSValue valueWithMGLIconTextFit:MGLIconTextFitNone].MGLIconTextFitValue, MGLIconTextFitNone);
    XCTAssertEqual([NSValue valueWithMGLIconTextFit:MGLIconTextFitWidth].MGLIconTextFitValue, MGLIconTextFitWidth);
    XCTAssertEqual([NSValue valueWithMGLIconTextFit:MGLIconTextFitHeight].MGLIconTextFitValue, MGLIconTextFitHeight);
    XCTAssertEqual([NSValue valueWithMGLIconTextFit:MGLIconTextFitBoth].MGLIconTextFitValue, MGLIconTextFitBoth);
    XCTAssertEqual([NSValue valueWithMGLSymbolPlacement:MGLSymbolPlacementPoint].MGLSymbolPlacementValue, MGLSymbolPlacementPoint);
    XCTAssertEqual([NSValue valueWithMGLSymbolPlacement:MGLSymbolPlacementLine].MGLSymbolPlacementValue, MGLSymbolPlacementLine);
    XCTAssertEqual([NSValue valueWithMGLSymbolPlacement:MGLSymbolPlacementLineCenter].MGLSymbolPlacementValue, MGLSymbolPlacementLineCenter);
    XCTAssertEqual([NSValue valueWithMGLSymbolZOrder:MGLSymbolZOrderAuto].MGLSymbolZOrderValue, MGLSymbolZOrderAuto);
    XCTAssertEqual([NSValue valueWithMGLSymbolZOrder:MGLSymbolZOrderViewportY].MGLSymbolZOrderValue, MGLSymbolZOrderViewportY);
    XCTAssertEqual([NSValue valueWithMGLSymbolZOrder:MGLSymbolZOrderSource].MGLSymbolZOrderValue, MGLSymbolZOrderSource);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorCenter].MGLTextAnchorValue, MGLTextAnchorCenter);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorLeft].MGLTextAnchorValue, MGLTextAnchorLeft);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorRight].MGLTextAnchorValue, MGLTextAnchorRight);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorTop].MGLTextAnchorValue, MGLTextAnchorTop);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorBottom].MGLTextAnchorValue, MGLTextAnchorBottom);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorTopLeft].MGLTextAnchorValue, MGLTextAnchorTopLeft);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorTopRight].MGLTextAnchorValue, MGLTextAnchorTopRight);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorBottomLeft].MGLTextAnchorValue, MGLTextAnchorBottomLeft);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorBottomRight].MGLTextAnchorValue, MGLTextAnchorBottomRight);
    XCTAssertEqual([NSValue valueWithMGLTextJustification:MGLTextJustificationAuto].MGLTextJustificationValue, MGLTextJustificationAuto);
    XCTAssertEqual([NSValue valueWithMGLTextJustification:MGLTextJustificationLeft].MGLTextJustificationValue, MGLTextJustificationLeft);
    XCTAssertEqual([NSValue valueWithMGLTextJustification:MGLTextJustificationCenter].MGLTextJustificationValue, MGLTextJustificationCenter);
    XCTAssertEqual([NSValue valueWithMGLTextJustification:MGLTextJustificationRight].MGLTextJustificationValue, MGLTextJustificationRight);
    XCTAssertEqual([NSValue valueWithMGLTextPitchAlignment:MGLTextPitchAlignmentMap].MGLTextPitchAlignmentValue, MGLTextPitchAlignmentMap);
    XCTAssertEqual([NSValue valueWithMGLTextPitchAlignment:MGLTextPitchAlignmentViewport].MGLTextPitchAlignmentValue, MGLTextPitchAlignmentViewport);
    XCTAssertEqual([NSValue valueWithMGLTextPitchAlignment:MGLTextPitchAlignmentAuto].MGLTextPitchAlignmentValue, MGLTextPitchAlignmentAuto);
    XCTAssertEqual([NSValue valueWithMGLTextRotationAlignment:MGLTextRotationAlignmentMap].MGLTextRotationAlignmentValue, MGLTextRotationAlignmentMap);
    XCTAssertEqual([NSValue valueWithMGLTextRotationAlignment:MGLTextRotationAlignmentViewport].MGLTextRotationAlignmentValue, MGLTextRotationAlignmentViewport);
    XCTAssertEqual([NSValue valueWithMGLTextRotationAlignment:MGLTextRotationAlignmentAuto].MGLTextRotationAlignmentValue, MGLTextRotationAlignmentAuto);
    XCTAssertEqual([NSValue valueWithMGLTextTransform:MGLTextTransformNone].MGLTextTransformValue, MGLTextTransformNone);
    XCTAssertEqual([NSValue valueWithMGLTextTransform:MGLTextTransformUppercase].MGLTextTransformValue, MGLTextTransformUppercase);
    XCTAssertEqual([NSValue valueWithMGLTextTransform:MGLTextTransformLowercase].MGLTextTransformValue, MGLTextTransformLowercase);
    XCTAssertEqual([NSValue valueWithMGLTextWritingMode:MGLTextWritingModeHorizontal].MGLTextWritingModeValue, MGLTextWritingModeHorizontal);
    XCTAssertEqual([NSValue valueWithMGLTextWritingMode:MGLTextWritingModeVertical].MGLTextWritingModeValue, MGLTextWritingModeVertical);
    XCTAssertEqual([NSValue valueWithMGLIconTranslationAnchor:MGLIconTranslationAnchorMap].MGLIconTranslationAnchorValue, MGLIconTranslationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLIconTranslationAnchor:MGLIconTranslationAnchorViewport].MGLIconTranslationAnchorValue, MGLIconTranslationAnchorViewport);
    XCTAssertEqual([NSValue valueWithMGLTextTranslationAnchor:MGLTextTranslationAnchorMap].MGLTextTranslationAnchorValue, MGLTextTranslationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLTextTranslationAnchor:MGLTextTranslationAnchorViewport].MGLTextTranslationAnchorValue, MGLTextTranslationAnchorViewport);
}

@end
