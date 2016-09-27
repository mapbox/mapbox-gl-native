// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 A map style's background layer is the bottommost layer and is used to style a color 
 or pattern to show below all other map features. You can query an `MGLMapView` for its 
 `style` and obtain the background layer using the `-[MGLStyle layerWithIdentifier:]` 
 method and passing `background` for the identifier. 
 */
@interface MGLBackgroundStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier;

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier source:(MGLSource *)source;


#pragma mark - Accessing the Paint Attributes

#if TARGET_OS_IPHONE
/**
 The color with which the background will be drawn.
 
 The default value of this property is `UIColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `backgroundPattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> backgroundColor;
#else
/**
 The color with which the background will be drawn.
 
 The default value of this property is `NSColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `backgroundPattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> backgroundColor;
#endif

/**
 Name of image in sprite to use for drawing an image background. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> backgroundPattern;

/**
 The opacity at which the background will be drawn.
 
 The default value of this property is an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> backgroundOpacity;

@end

NS_ASSUME_NONNULL_END
