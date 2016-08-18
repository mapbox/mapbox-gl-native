// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, MGLFillStyleLayerFillTranslateAnchor) {
    MGLFillStyleLayerFillTranslateAnchorMap,
    MGLFillStyleLayerFillTranslateAnchorViewport,
};

@interface MGLFillStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

#pragma mark - Accessing the Paint Attributes

/**
 Whether or not the fill should be antialiased.
 
 The default value of this property is `YES`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> fillAntialias;

/**
 The opacity of the entire fill layer. In contrast to the fill-color, this value will also affect the 1pt stroke around the fill, if the stroke is used.
 
 The default value of this property is `1`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> fillOpacity;

/**
 The color of the filled part of this layer. This color can be specified as rgba with an alpha component and the color's opacity will not affect the opacity of the 1pt stroke, if it is used.
 
 The default value of this property is an `NSColor` or `UIColor`object whose RGB value is 0, 0, 0 and whose alpha value is 1. Set this property to `nil` to reset it to the default.
 
 This property is only applied to the style if `fillPattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> fillColor;

/**
 The outline color of the fill. Matches the value of `fillColor` if unspecified.
 
 This property is only applied to the style if `fillPattern` is set to `nil`, and `fillAntialias` is set to `YES`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> fillOutlineColor;

/**
 The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.

 This property is measured in points.
 
 The default value of this property is 0 points from the left and 0 points from the top. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> fillTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen)
 
 The default value of this property is `MGLFillStyleLayerFillTranslateAnchorMap`. Set this property to `nil` to reset it to the default.
 
 This property is only applied to the style if `fillTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> fillTranslateAnchor;

/**
 Name of image in sprite to use for drawing image fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> fillPattern;

@end

NS_ASSUME_NONNULL_END
