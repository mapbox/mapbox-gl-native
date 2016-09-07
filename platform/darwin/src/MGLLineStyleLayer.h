// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineCap) {
    MGLLineStyleLayerLineCapButt,
    MGLLineStyleLayerLineCapRound,
    MGLLineStyleLayerLineCapSquare,
};

typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineJoin) {
    MGLLineStyleLayerLineJoinBevel,
    MGLLineStyleLayerLineJoinRound,
    MGLLineStyleLayerLineJoinMiter,
};

typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineTranslateAnchor) {
    MGLLineStyleLayerLineTranslateAnchorMap,
    MGLLineStyleLayerLineTranslateAnchorViewport,
};

@interface MGLLineStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

#pragma mark - Accessing the Layout Attributes

/**
 The display of line endings.
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLLineStyleLayerLineCapButt`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineCap;

/**
 The display of lines when joining.
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLLineStyleLayerLineJoinMiter`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineJoin;

/**
 Used to automatically convert miter joins to bevel joins for sharp angles.
 
 If this property is set to `nil`, the layer uses an implicit default value of `2`.
 
 This property is only applied to the style if `lineJoin` is set to `MGLLineStyleLayerLineJoinMiter`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineMiterLimit;

/**
 Used to automatically convert round joins to miter joins for shallow angles.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1.05`.
 
 This property is only applied to the style if `lineJoin` is set to `MGLLineStyleLayerLineJoinRound`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineRoundLimit;

#pragma mark - Accessing the Paint Attributes

/**
 The opacity at which the line will be drawn.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineOpacity;

/**
 The color with which the line will be drawn.
 
 If this property is set to `nil`, the layer uses an implicit default value of `blackColor`.

 This property is only applied to the style if `linePattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineColor;

/**
 The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of 0 points from the left and 0 points from the top.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen)
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLLineStyleLayerLineTranslateAnchorMap`.

 This property is only applied to the style if `lineTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineTranslateAnchor;

/**
 Stroke thickness.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineWidth;

/**
 Draws a line casing outside of a line's actual path. Value indicates the width of the inner gap.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineGapWidth;

/**
 The line's offset perpendicular to its direction. Values may be positive or negative, where positive indicates "rightwards" (if you were moving in the direction of the line) and negative indicates "leftwards."

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineOffset;

/**
 Blur applied to the line, in points.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineBlur;

/**
 Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to points, multiply the length by the current line width.

 This property is measured in line widths.

 This property is only applied to the style if `linePattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> lineDasharray;

/**
 Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512).
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> linePattern;

@end

NS_ASSUME_NONNULL_END
