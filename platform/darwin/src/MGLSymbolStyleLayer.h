// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-ios`.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"

typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerSymbolPlacement) {
    MGLSymbolStyleLayerSymbolPlacementPoint,
    MGLSymbolStyleLayerSymbolPlacementLine,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerIconRotationAlignment) {
    MGLSymbolStyleLayerIconRotationAlignmentMap,
    MGLSymbolStyleLayerIconRotationAlignmentViewport,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerIconTextFit) {
    MGLSymbolStyleLayerIconTextFitNone,
    MGLSymbolStyleLayerIconTextFitBoth,
    MGLSymbolStyleLayerIconTextFitWidth,
    MGLSymbolStyleLayerIconTextFitHeight,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextPitchAlignment) {
    MGLSymbolStyleLayerTextPitchAlignmentMap,
    MGLSymbolStyleLayerTextPitchAlignmentViewport,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextRotationAlignment) {
    MGLSymbolStyleLayerTextRotationAlignmentMap,
    MGLSymbolStyleLayerTextRotationAlignmentViewport,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextJustify) {
    MGLSymbolStyleLayerTextJustifyLeft,
    MGLSymbolStyleLayerTextJustifyCenter,
    MGLSymbolStyleLayerTextJustifyRight,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextAnchor) {
    MGLSymbolStyleLayerTextAnchorCenter,
    MGLSymbolStyleLayerTextAnchorLeft,
    MGLSymbolStyleLayerTextAnchorRight,
    MGLSymbolStyleLayerTextAnchorTop,
    MGLSymbolStyleLayerTextAnchorBottom,
    MGLSymbolStyleLayerTextAnchorTopLeft,
    MGLSymbolStyleLayerTextAnchorTopRight,
    MGLSymbolStyleLayerTextAnchorBottomLeft,
    MGLSymbolStyleLayerTextAnchorBottomRight,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextTransform) {
    MGLSymbolStyleLayerTextTransformNone,
    MGLSymbolStyleLayerTextTransformUppercase,
    MGLSymbolStyleLayerTextTransformLowercase,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerIconTranslateAnchor) {
    MGLSymbolStyleLayerIconTranslateAnchorMap,
    MGLSymbolStyleLayerIconTranslateAnchorViewport,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextTranslateAnchor) {
    MGLSymbolStyleLayerTextTranslateAnchorMap,
    MGLSymbolStyleLayerTextTranslateAnchorViewport,
};

@interface MGLSymbolStyleLayer : MGLStyleLayer

#pragma mark - Accessing the Layout Attributes

/**
  Label placement relative to its geometry. `line` can only be used on LineStrings and Polygons.
*/
@property (nonatomic) MGLSymbolStyleLayerSymbolPlacement symbolPlacement;

/**
  Distance between two symbol anchors.
*/
@property (nonatomic) CGFloat symbolSpacing;

/**
  If true, the symbols will not cross tile edges to avoid mutual collisions. Recommended in layers that don't have enough padding in the vector tile to prevent collisions, or if it is a point symbol layer placed after a line symbol layer.
*/
@property (nonatomic) BOOL symbolAvoidEdges;

/**
  If true, the icon will be visible even if it collides with other previously drawn symbols.
*/
@property (nonatomic) BOOL iconAllowOverlap;

/**
  If true, other symbols can be visible even if they collide with the icon.
*/
@property (nonatomic) BOOL iconIgnorePlacement;

/**
  If true, text will display without their corresponding icons when the icon collides with other symbols and the text does not.
*/
@property (nonatomic) BOOL iconOptional;

/**
  Orientation of icon when map is rotated.
*/
@property (nonatomic) MGLSymbolStyleLayerIconRotationAlignment iconRotationAlignment;

/**
  Scale factor for icon. 1 is original size, 3 triples the size.
*/
@property (nonatomic) CGFloat iconSize;

/**
  Position and scale an icon by the its corresponding text.
*/
@property (nonatomic) MGLSymbolStyleLayerIconTextFit iconTextFit;

/**
  Size of padding area around the text-fit size in clockwise order: top, right, bottom, left.
*/
@property (nonatomic) NSArray *iconTextFitPadding;

/**
  A string with {tokens} replaced, referencing the data property to pull from.
*/
@property (nonatomic) NSString *iconImage;

/**
  Rotates the icon clockwise.
*/
@property (nonatomic) CGFloat iconRotate;

/**
  Size of the additional area around the icon bounding box used for detecting symbol collisions.
*/
@property (nonatomic) CGFloat iconPadding;

/**
  If true, the icon may be flipped to prevent it from being rendered upside-down.
*/
@property (nonatomic) BOOL iconKeepUpright;

/**
  Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up.
*/
@property (nonatomic) NSArray *iconOffset;

/**
  Aligns text to the plane of the `viewport` or the `map` when the map is pitched. Matches `text-rotation-alignment` if unspecified.
*/
@property (nonatomic) MGLSymbolStyleLayerTextPitchAlignment textPitchAlignment;

/**
  Orientation of text when map is rotated.
*/
@property (nonatomic) MGLSymbolStyleLayerTextRotationAlignment textRotationAlignment;

/**
  Value to use for a text label. Feature properties are specified using tokens like {field_name}.
*/
@property (nonatomic) NSString *textField;

/**
  Font stack to use for displaying text.
*/
@property (nonatomic) NSArray *textFont;

/**
  Font size.
*/
@property (nonatomic) CGFloat textSize;

/**
  The maximum line width for text wrapping.
*/
@property (nonatomic) CGFloat textMaxWidth;

/**
  Text leading value for multi-line text.
*/
@property (nonatomic) CGFloat textLineHeight;

/**
  Text tracking amount.
*/
@property (nonatomic) CGFloat textLetterSpacing;

/**
  Text justification options.
*/
@property (nonatomic) MGLSymbolStyleLayerTextJustify textJustify;

/**
  Part of the text placed closest to the anchor.
*/
@property (nonatomic) MGLSymbolStyleLayerTextAnchor textAnchor;

/**
  Maximum angle change between adjacent characters.
*/
@property (nonatomic) CGFloat textMaxAngle;

/**
  Rotates the text clockwise.
*/
@property (nonatomic) CGFloat textRotate;

/**
  Size of the additional area around the text bounding box used for detecting symbol collisions.
*/
@property (nonatomic) CGFloat textPadding;

/**
  If true, the text may be flipped vertically to prevent it from being rendered upside-down.
*/
@property (nonatomic) BOOL textKeepUpright;

/**
  Specifies how to capitalize text, similar to the CSS `text-transform` property.
*/
@property (nonatomic) MGLSymbolStyleLayerTextTransform textTransform;

/**
  Offset distance of text from its anchor. Positive values indicate right and down, while negative values indicate left and up.
*/
@property (nonatomic) NSArray *textOffset;

/**
  If true, the text will be visible even if it collides with other previously drawn symbols.
*/
@property (nonatomic) BOOL textAllowOverlap;

/**
  If true, other symbols can be visible even if they collide with the text.
*/
@property (nonatomic) BOOL textIgnorePlacement;

/**
  If true, icons will display without their corresponding text when the text collides with other symbols and the icon does not.
*/
@property (nonatomic) BOOL textOptional;

#pragma mark - Accessing the Paint Attributes

/**
  The opacity at which the icon will be drawn.
*/
@property (nonatomic) CGFloat iconOpacity;

/**
  The color of the icon. This can only be used with sdf icons.
*/
@property (nonatomic) MGLColor *iconColor;

/**
  The color of the icon's halo. Icon halos can only be used with sdf icons.
*/
@property (nonatomic) MGLColor *iconHaloColor;

/**
  Distance of halo to the icon outline.
*/
@property (nonatomic) CGFloat iconHaloWidth;

/**
  Fade out the halo towards the outside.
*/
@property (nonatomic) CGFloat iconHaloBlur;

/**
  Distance that the icon's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
*/
@property (nonatomic) NSArray *iconTranslate;

/**
  Control whether the translation is relative to the map (north) or viewport (screen).
*/
@property (nonatomic) MGLSymbolStyleLayerIconTranslateAnchor iconTranslateAnchor;

/**
  The opacity at which the text will be drawn.
*/
@property (nonatomic) CGFloat textOpacity;

/**
  The color with which the text will be drawn.
*/
@property (nonatomic) MGLColor *textColor;

/**
  The color of the text's halo, which helps it stand out from backgrounds.
*/
@property (nonatomic) MGLColor *textHaloColor;

/**
  Distance of halo to the font outline. Max text halo width is 1/4 of the font-size.
*/
@property (nonatomic) CGFloat textHaloWidth;

/**
  The halo's fadeout distance towards the outside.
*/
@property (nonatomic) CGFloat textHaloBlur;

/**
  Distance that the text's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
*/
@property (nonatomic) NSArray *textTranslate;

/**
  Control whether the translation is relative to the map (north) or viewport (screen).
*/
@property (nonatomic) MGLSymbolStyleLayerTextTranslateAnchor textTranslateAnchor;

@end

