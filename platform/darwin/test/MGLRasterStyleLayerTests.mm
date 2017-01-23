// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/raster_layer.hpp>

@interface MGLRasterLayerTests : MGLStyleLayerTests
@end

@implementation MGLRasterLayerTests

+ (NSString *)layerType {
    return @"raster";
}

- (void)testProperties {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    
    MGLRasterStyleLayer *layer = [[MGLRasterStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::RasterLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::RasterLayer>();
    
    // raster-brightness-max
    {
        XCTAssertTrue(rawLayer->getRasterBrightnessMax().isUndefined(),
                      @"raster-brightness-max should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.maximumRasterBrightness;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.maximumRasterBrightness = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterBrightnessMax(), propertyValue,
                       @"Setting maximumRasterBrightness to a constant value should update raster-brightness-max.");
        XCTAssertEqualObjects(layer.maximumRasterBrightness, styleValue,
                              @"maximumRasterBrightness should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.maximumRasterBrightness = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getRasterBrightnessMax(), propertyValue,
                       @"Setting maximumRasterBrightness to a function should update raster-brightness-max.");
        XCTAssertEqualObjects(layer.maximumRasterBrightness, styleValue,
                              @"maximumRasterBrightness should round-trip functions.");
        
        layer.maximumRasterBrightness = nil;
        XCTAssertTrue(rawLayer->getRasterBrightnessMax().isUndefined(),
                      @"Unsetting maximumRasterBrightness should return raster-brightness-max to the default value.");
        XCTAssertEqualObjects(layer.maximumRasterBrightness, defaultStyleValue,
                              @"maximumRasterBrightness should return the default value after being unset.");
    }
    
    // raster-brightness-min
    {
        XCTAssertTrue(rawLayer->getRasterBrightnessMin().isUndefined(),
                      @"raster-brightness-min should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.minimumRasterBrightness;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.minimumRasterBrightness = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterBrightnessMin(), propertyValue,
                       @"Setting minimumRasterBrightness to a constant value should update raster-brightness-min.");
        XCTAssertEqualObjects(layer.minimumRasterBrightness, styleValue,
                              @"minimumRasterBrightness should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.minimumRasterBrightness = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getRasterBrightnessMin(), propertyValue,
                       @"Setting minimumRasterBrightness to a function should update raster-brightness-min.");
        XCTAssertEqualObjects(layer.minimumRasterBrightness, styleValue,
                              @"minimumRasterBrightness should round-trip functions.");
        
        layer.minimumRasterBrightness = nil;
        XCTAssertTrue(rawLayer->getRasterBrightnessMin().isUndefined(),
                      @"Unsetting minimumRasterBrightness should return raster-brightness-min to the default value.");
        XCTAssertEqualObjects(layer.minimumRasterBrightness, defaultStyleValue,
                              @"minimumRasterBrightness should return the default value after being unset.");
    }
    
    // raster-contrast
    {
        XCTAssertTrue(rawLayer->getRasterContrast().isUndefined(),
                      @"raster-contrast should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.rasterContrast;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.rasterContrast = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterContrast(), propertyValue,
                       @"Setting rasterContrast to a constant value should update raster-contrast.");
        XCTAssertEqualObjects(layer.rasterContrast, styleValue,
                              @"rasterContrast should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.rasterContrast = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getRasterContrast(), propertyValue,
                       @"Setting rasterContrast to a function should update raster-contrast.");
        XCTAssertEqualObjects(layer.rasterContrast, styleValue,
                              @"rasterContrast should round-trip functions.");
        
        layer.rasterContrast = nil;
        XCTAssertTrue(rawLayer->getRasterContrast().isUndefined(),
                      @"Unsetting rasterContrast should return raster-contrast to the default value.");
        XCTAssertEqualObjects(layer.rasterContrast, defaultStyleValue,
                              @"rasterContrast should return the default value after being unset.");
    }
    
    // raster-fade-duration
    {
        XCTAssertTrue(rawLayer->getRasterFadeDuration().isUndefined(),
                      @"raster-fade-duration should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.rasterFadeDuration;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.rasterFadeDuration = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterFadeDuration(), propertyValue,
                       @"Setting rasterFadeDuration to a constant value should update raster-fade-duration.");
        XCTAssertEqualObjects(layer.rasterFadeDuration, styleValue,
                              @"rasterFadeDuration should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.rasterFadeDuration = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getRasterFadeDuration(), propertyValue,
                       @"Setting rasterFadeDuration to a function should update raster-fade-duration.");
        XCTAssertEqualObjects(layer.rasterFadeDuration, styleValue,
                              @"rasterFadeDuration should round-trip functions.");
        
        layer.rasterFadeDuration = nil;
        XCTAssertTrue(rawLayer->getRasterFadeDuration().isUndefined(),
                      @"Unsetting rasterFadeDuration should return raster-fade-duration to the default value.");
        XCTAssertEqualObjects(layer.rasterFadeDuration, defaultStyleValue,
                              @"rasterFadeDuration should return the default value after being unset.");
    }
    
    // raster-hue-rotate
    {
        XCTAssertTrue(rawLayer->getRasterHueRotate().isUndefined(),
                      @"raster-hue-rotate should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.rasterHueRotation;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.rasterHueRotation = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterHueRotate(), propertyValue,
                       @"Setting rasterHueRotation to a constant value should update raster-hue-rotate.");
        XCTAssertEqualObjects(layer.rasterHueRotation, styleValue,
                              @"rasterHueRotation should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.rasterHueRotation = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getRasterHueRotate(), propertyValue,
                       @"Setting rasterHueRotation to a function should update raster-hue-rotate.");
        XCTAssertEqualObjects(layer.rasterHueRotation, styleValue,
                              @"rasterHueRotation should round-trip functions.");
        
        layer.rasterHueRotation = nil;
        XCTAssertTrue(rawLayer->getRasterHueRotate().isUndefined(),
                      @"Unsetting rasterHueRotation should return raster-hue-rotate to the default value.");
        XCTAssertEqualObjects(layer.rasterHueRotation, defaultStyleValue,
                              @"rasterHueRotation should return the default value after being unset.");
    }
    
    // raster-opacity
    {
        XCTAssertTrue(rawLayer->getRasterOpacity().isUndefined(),
                      @"raster-opacity should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.rasterOpacity;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.rasterOpacity = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterOpacity(), propertyValue,
                       @"Setting rasterOpacity to a constant value should update raster-opacity.");
        XCTAssertEqualObjects(layer.rasterOpacity, styleValue,
                              @"rasterOpacity should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.rasterOpacity = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getRasterOpacity(), propertyValue,
                       @"Setting rasterOpacity to a function should update raster-opacity.");
        XCTAssertEqualObjects(layer.rasterOpacity, styleValue,
                              @"rasterOpacity should round-trip functions.");
        
        layer.rasterOpacity = nil;
        XCTAssertTrue(rawLayer->getRasterOpacity().isUndefined(),
                      @"Unsetting rasterOpacity should return raster-opacity to the default value.");
        XCTAssertEqualObjects(layer.rasterOpacity, defaultStyleValue,
                              @"rasterOpacity should return the default value after being unset.");
    }
    
    // raster-saturation
    {
        XCTAssertTrue(rawLayer->getRasterSaturation().isUndefined(),
                      @"raster-saturation should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.rasterSaturation;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.rasterSaturation = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterSaturation(), propertyValue,
                       @"Setting rasterSaturation to a constant value should update raster-saturation.");
        XCTAssertEqualObjects(layer.rasterSaturation, styleValue,
                              @"rasterSaturation should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.rasterSaturation = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getRasterSaturation(), propertyValue,
                       @"Setting rasterSaturation to a function should update raster-saturation.");
        XCTAssertEqualObjects(layer.rasterSaturation, styleValue,
                              @"rasterSaturation should round-trip functions.");
        
        layer.rasterSaturation = nil;
        XCTAssertTrue(rawLayer->getRasterSaturation().isUndefined(),
                      @"Unsetting rasterSaturation should return raster-saturation to the default value.");
        XCTAssertEqualObjects(layer.rasterSaturation, defaultStyleValue,
                              @"rasterSaturation should return the default value after being unset.");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"maximum-raster-brightness" isBoolean:NO];
    [self testPropertyName:@"minimum-raster-brightness" isBoolean:NO];
    [self testPropertyName:@"raster-contrast" isBoolean:NO];
    [self testPropertyName:@"raster-fade-duration" isBoolean:NO];
    [self testPropertyName:@"raster-hue-rotation" isBoolean:NO];
    [self testPropertyName:@"raster-opacity" isBoolean:NO];
    [self testPropertyName:@"raster-saturation" isBoolean:NO];
}

@end
