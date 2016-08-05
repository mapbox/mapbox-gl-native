// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

@interface MGLRasterStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

#pragma mark - Accessing the Paint Attributes

/**
 The opacity at which the image will be drawn.
 */
@property (nonatomic) id <MGLStyleAttributeValue> rasterOpacity;

/**
 Rotates hues around the color wheel.
 */
@property (nonatomic) id <MGLStyleAttributeValue> rasterHueRotate;

/**
 Increase or reduce the brightness of the image. The value is the minimum brightness.
 */
@property (nonatomic) id <MGLStyleAttributeValue> rasterBrightnessMin;

/**
 Increase or reduce the brightness of the image. The value is the maximum brightness.
 */
@property (nonatomic) id <MGLStyleAttributeValue> rasterBrightnessMax;

/**
 Increase or reduce the saturation of the image.
 */
@property (nonatomic) id <MGLStyleAttributeValue> rasterSaturation;

/**
 Increase or reduce the contrast of the image.
 */
@property (nonatomic) id <MGLStyleAttributeValue> rasterContrast;

/**
 Fade duration when a new tile is added.
 */
@property (nonatomic) id <MGLStyleAttributeValue> rasterFadeDuration;

@end
