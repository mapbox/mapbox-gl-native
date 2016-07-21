// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-ios`.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"

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
@property (nonatomic) MGLLineStyleLayerLineCap lineCap;

/**
  The display of lines when joining.
*/
@property (nonatomic) MGLLineStyleLayerLineJoin lineJoin;

/**
  Used to automatically convert miter joins to bevel joins for sharp angles.
*/
@property (nonatomic) CGFloat lineMiterLimit;

/**
  Used to automatically convert round joins to miter joins for shallow angles.
*/
@property (nonatomic) CGFloat lineRoundLimit;

#pragma mark - Accessing the Paint Attributes

/**
  The opacity at which the line will be drawn.
*/
@property (nonatomic) CGFloat lineOpacity;

/**
  The color with which the line will be drawn.
*/
@property (nonatomic) MGLColor *lineColor;

/**
  The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
*/
@property (nonatomic) NSArray *lineTranslate;

/**
  Control whether the translation is relative to the map (north) or viewport (screen)
*/
@property (nonatomic) MGLLineStyleLayerLineTranslateAnchor lineTranslateAnchor;

/**
  Stroke thickness.
*/
@property (nonatomic) CGFloat lineWidth;

/**
  Draws a line casing outside of a line's actual path. Value indicates the width of the inner gap.
*/
@property (nonatomic) CGFloat lineGapWidth;

/**
  The line's offset perpendicular to its direction. Values may be positive or negative, where positive indicates "rightwards" (if you were moving in the direction of the line) and negative indicates "leftwards."
*/
@property (nonatomic) CGFloat lineOffset;

/**
  Blur applied to the line, in pixels.
*/
@property (nonatomic) CGFloat lineBlur;

/**
  Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to pixels, multiply the length by the current line width.
*/
@property (nonatomic) NSArray *lineDasharray;

/**
  Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512).
*/
@property (nonatomic) NSString *linePattern;

@end

