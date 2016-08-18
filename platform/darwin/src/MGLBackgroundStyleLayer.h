// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLBackgroundStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

#pragma mark - Accessing the Paint Attributes

/**
 The color with which the background will be drawn.
 
 The default value of this property is an `NSColor` or `UIColor`object whose RGB value is 0, 0, 0 and whose alpha value is 1. Set this property to `nil` to reset it to the default.
 
 This property is only applied to the style if `backgroundPattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> backgroundColor;

/**
 Name of image in sprite to use for drawing an image background. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> backgroundPattern;

/**
 The opacity at which the background will be drawn.
 
 The default value of this property is `1`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> backgroundOpacity;

@end

NS_ASSUME_NONNULL_END
