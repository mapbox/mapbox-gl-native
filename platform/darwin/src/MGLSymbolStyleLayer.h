// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeValue.h"

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

@interface MGLSymbolStyleLayer : NSObject <MGLStyleLayer>

#pragma mark - Accessing the Layout Attributes

/**
  Label placement relative to its geometry. `line` can only be used on LineStrings and Polygons.
*/
@property (nonatomic) id <MGLStyleAttributeValue> symbolPlacement;

/**
  Distance between two symbol anchors.
*/
@property (nonatomic) id <MGLStyleAttributeValue> symbolSpacing;

/**
  If true, the symbols will not cross tile edges to avoid mutual collisions. Recommended in layers that don't have enough padding in the vector tile to prevent collisions, or if it is a point symbol layer placed after a line symbol layer.
*/
@property (nonatomic) id <MGLStyleAttributeValue> symbolAvoidEdges;

/**
  If true, the icon will be visible even if it collides with other previously drawn symbols.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconAllowOverlap;

/**
  If true, other symbols can be visible even if they collide with the icon.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconIgnorePlacement;

/**
  If true, text will display without their corresponding icons when the icon collides with other symbols and the text does not.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconOptional;

/**
  Orientation of icon when map is rotated.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconRotationAlignment;

/**
  Scale factor for icon. 1 is original size, 3 triples the size.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconSize;

/**
  Position and scale an icon by the its corresponding text.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconTextFit;

/**
  Size of padding area around the text-fit size in clockwise order: top, right, bottom, left.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconTextFitPadding;

/**
  A string with {tokens} replaced, referencing the data property to pull from.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconImage;

/**
  Rotates the icon clockwise.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconRotate;

/**
  Size of the additional area around the icon bounding box used for detecting symbol collisions.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconPadding;

/**
  If true, the icon may be flipped to prevent it from being rendered upside-down.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconKeepUpright;

/**
  Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconOffset;

/**
  Aligns text to the plane of the `viewport` or the `map` when the map is pitched. Matches `text-rotation-alignment` if unspecified.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textPitchAlignment;

/**
  Orientation of text when map is rotated.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textRotationAlignment;

/**
  Value to use for a text label. Feature properties are specified using tokens like {field_name}.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textField;

/**
  Font stack to use for displaying text.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textFont;

/**
  Font size.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textSize;

/**
  The maximum line width for text wrapping.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textMaxWidth;

/**
  Text leading value for multi-line text.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textLineHeight;

/**
  Text tracking amount.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textLetterSpacing;

/**
  Text justification options.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textJustify;

/**
  Part of the text placed closest to the anchor.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textAnchor;

/**
  Maximum angle change between adjacent characters.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textMaxAngle;

/**
  Rotates the text clockwise.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textRotate;

/**
  Size of the additional area around the text bounding box used for detecting symbol collisions.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textPadding;

/**
  If true, the text may be flipped vertically to prevent it from being rendered upside-down.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textKeepUpright;

/**
  Specifies how to capitalize text, similar to the CSS `text-transform` property.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textTransform;

/**
  Offset distance of text from its anchor. Positive values indicate right and down, while negative values indicate left and up.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textOffset;

/**
  If true, the text will be visible even if it collides with other previously drawn symbols.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textAllowOverlap;

/**
  If true, other symbols can be visible even if they collide with the text.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textIgnorePlacement;

/**
  If true, icons will display without their corresponding text when the text collides with other symbols and the icon does not.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textOptional;

#pragma mark - Accessing the Paint Attributes

/**
  The opacity at which the icon will be drawn.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconOpacity;

/**
  The color of the icon. This can only be used with sdf icons.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconColor;

/**
  The color of the icon's halo. Icon halos can only be used with sdf icons.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconHaloColor;

/**
  Distance of halo to the icon outline.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconHaloWidth;

/**
  Fade out the halo towards the outside.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconHaloBlur;

/**
  Distance that the icon's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconTranslate;

/**
  Control whether the translation is relative to the map (north) or viewport (screen).
*/
@property (nonatomic) id <MGLStyleAttributeValue> iconTranslateAnchor;

/**
  The opacity at which the text will be drawn.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textOpacity;

/**
  The color with which the text will be drawn.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textColor;

/**
  The color of the text's halo, which helps it stand out from backgrounds.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textHaloColor;

/**
  Distance of halo to the font outline. Max text halo width is 1/4 of the font-size.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textHaloWidth;

/**
  The halo's fadeout distance towards the outside.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textHaloBlur;

/**
  Distance that the text's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.
*/
@property (nonatomic) id <MGLStyleAttributeValue> textTranslate;

/**
  Control whether the translation is relative to the map (north) or viewport (screen).
*/
@property (nonatomic) id <MGLStyleAttributeValue> textTranslateAnchor;

@end

