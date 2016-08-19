// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLRasterStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

#pragma mark - Accessing the Paint Attributes

/**
 The opacity at which the image will be drawn.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> rasterOpacity;

/**
 Rotates hues around the color wheel.

 This property is measured in degrees.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> rasterHueRotate;

/**
 Increase or reduce the brightness of the image. The value is the minimum brightness.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> rasterBrightnessMin;

/**
 Increase or reduce the brightness of the image. The value is the maximum brightness.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> rasterBrightnessMax;

/**
 Increase or reduce the saturation of the image.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> rasterSaturation;

/**
 Increase or reduce the contrast of the image.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> rasterContrast;

/**
 Fade duration when a new tile is added.

 This property is measured in milliseconds.
 
 If this property is set to `nil`, the layer uses an implicit default value of `300`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> rasterFadeDuration;

@end

NS_ASSUME_NONNULL_END
