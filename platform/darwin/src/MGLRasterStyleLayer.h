// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-ios`.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"


@interface MGLRasterStyleLayer : MGLStyleLayer

#pragma mark - Accessing the Paint Attributes

/**
  The opacity at which the image will be drawn.
*/
@property (nonatomic) CGFloat rasterOpacity;

/**
  Rotates hues around the color wheel.
*/
@property (nonatomic) CGFloat rasterHueRotate;

/**
  Increase or reduce the brightness of the image. The value is the minimum brightness.
*/
@property (nonatomic) CGFloat rasterBrightnessMin;

/**
  Increase or reduce the brightness of the image. The value is the maximum brightness.
*/
@property (nonatomic) CGFloat rasterBrightnessMax;

/**
  Increase or reduce the saturation of the image.
*/
@property (nonatomic) CGFloat rasterSaturation;

/**
  Increase or reduce the contrast of the image.
*/
@property (nonatomic) CGFloat rasterContrast;

/**
  Fade duration when a new tile is added.
*/
@property (nonatomic) CGFloat rasterFadeDuration;

@end

