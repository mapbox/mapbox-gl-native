// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLBackgroundStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

#pragma mark - Accessing the Paint Attributes

/**
 The color with which the background will be drawn.
 
 If this property is set to `nil`, the layer uses an implicit default value of an `NSColor` or `UIColor`object whose RGB value is 0, 0, 0 and whose alpha value is 1.

 This property is only applied to the style if `backgroundPattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> backgroundColor;

/**
 Name of image in sprite to use for drawing an image background. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> backgroundPattern;

/**
 The opacity at which the background will be drawn.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> backgroundOpacity;

@end

NS_ASSUME_NONNULL_END
