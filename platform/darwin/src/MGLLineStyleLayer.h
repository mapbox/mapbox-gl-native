// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeValue.h"

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

@interface MGLLineStyleLayer : MGLStyleLayer

#pragma mark - Accessing the Layout Attributes

/**
  The display of line endings.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineCap;

/**
  The display of lines when joining.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineJoin;

/**
  Used to automatically convert miter joins to bevel joins for sharp angles.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineMiterLimit;

/**
  Used to automatically convert round joins to miter joins for shallow angles.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineRoundLimit;

#pragma mark - Accessing the Paint Attributes

/**
  The opacity at which the line will be drawn.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineOpacity;

/**
  The color with which the line will be drawn.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineColor;

/**
  The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineTranslate;

/**
  Control whether the translation is relative to the map (north) or viewport (screen)
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineTranslateAnchor;

/**
  Stroke thickness.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineWidth;

/**
  Draws a line casing outside of a line's actual path. Value indicates the width of the inner gap.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineGapWidth;

/**
  The line's offset perpendicular to its direction. Values may be positive or negative, where positive indicates "rightwards" (if you were moving in the direction of the line) and negative indicates "leftwards."
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineOffset;

/**
  Blur applied to the line, in pixels.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineBlur;

/**
  Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to pixels, multiply the length by the current line width.
*/
@property (nonatomic) id <MGLStyleAttributeValue> lineDasharray;

/**
  Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512).
*/
@property (nonatomic) id <MGLStyleAttributeValue> linePattern;

@end

