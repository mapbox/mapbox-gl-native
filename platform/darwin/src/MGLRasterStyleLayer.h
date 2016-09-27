// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 A raster layer which allows customization of styling properties at runtime. You may 
 instantiate a new raster layer to add to a map style or you may query an 
 `MGLMapView` for its `style` and obtain existing layers using the 
 `-[MGLStyle layerWithIdentifier:]` method. 
 */
@interface MGLRasterStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier source:(MGLSource *)source;


#pragma mark - Accessing the Paint Attributes

/**
 The opacity at which the image will be drawn.
 
 The default value of this property is an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> rasterOpacity;

/**
 Rotates hues around the color wheel.

 This property is measured in degrees.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> rasterHueRotate;

/**
 Increase or reduce the brightness of the image. The value is the minimum brightness.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> rasterBrightnessMin;

/**
 Increase or reduce the brightness of the image. The value is the maximum brightness.
 
 The default value of this property is an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> rasterBrightnessMax;

/**
 Increase or reduce the saturation of the image.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> rasterSaturation;

/**
 Increase or reduce the contrast of the image.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> rasterContrast;

/**
 Fade duration when a new tile is added.

 This property is measured in milliseconds.
 
 The default value of this property is an `NSNumber` object containing the float `300`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> rasterFadeDuration;

@end

NS_ASSUME_NONNULL_END
