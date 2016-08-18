// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

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

@interface MGLSymbolStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

#pragma mark - Accessing the Layout Attributes

/**
 Label placement relative to its geometry. `line` can only be used on LineStrings and Polygons.

 The default value of this property is `point`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> symbolPlacement;

/**
 Distance between two symbol anchors.

 The default value of this property is `250`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> symbolSpacing;

/**
 If true, the symbols will not cross tile edges to avoid mutual collisions. Recommended in layers that don't have enough padding in the vector tile to prevent collisions, or if it is a point symbol layer placed after a line symbol layer.

 The default value of this property is `false`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> symbolAvoidEdges;

/**
 If true, the icon will be visible even if it collides with other previously drawn symbols.

 The default value of this property is `false`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconAllowOverlap;

/**
 If true, other symbols can be visible even if they collide with the icon.

 The default value of this property is `false`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconIgnorePlacement;

/**
 If true, text will display without their corresponding icons when the icon collides with other symbols and the text does not.

 The default value of this property is `false`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconOptional;

/**
 Orientation of icon when map is rotated.

 The default value of this property is `viewport`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconRotationAlignment;

/**
 Scale factor for icon. 1 is original size, 3 triples the size.

 The default value of this property is `1`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconSize;

/**
 Position and scale an icon by the its corresponding text.

 The default value of this property is `none`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconTextFit;

/**
 Size of padding area around the text-fit size in clockwise order: top, right, bottom, left.

 The default value of this property is `0,0,0,0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconTextFitPadding;

/**
 A string with {tokens} replaced, referencing the data property to pull from.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconImage;

/**
 Rotates the icon clockwise.

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconRotate;

/**
 Size of the additional area around the icon bounding box used for detecting symbol collisions.

 The default value of this property is `2`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconPadding;

/**
 If true, the icon may be flipped to prevent it from being rendered upside-down.

 The default value of this property is `false`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconKeepUpright;

/**
 Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up.

 The default value of this property is `0,0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconOffset;

/**
 Aligns text to the plane of the `viewport` or the `map` when the map is pitched. Matches `textRotationAlignment` if unspecified.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textPitchAlignment;

/**
 Orientation of text when map is rotated.

 The default value of this property is `viewport`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textRotationAlignment;

/**
 Value to use for a text label. Feature properties are specified using tokens like {field_name}.

 The default value of this property is ``. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textField;

/**
 Font stack to use for displaying text.

 The default value of this property is `Open Sans Regular,Arial Unicode MS Regular`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textFont;

/**
 Font size.

 The default value of this property is `16`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textSize;

/**
 The maximum line width for text wrapping.

 The default value of this property is `10`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textMaxWidth;

/**
 Text leading value for multi-line text.

 The default value of this property is `1.2`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textLineHeight;

/**
 Text tracking amount.

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textLetterSpacing;

/**
 Text justification options.

 The default value of this property is `center`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textJustify;

/**
 Part of the text placed closest to the anchor.

 The default value of this property is `center`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textAnchor;

/**
 Maximum angle change between adjacent characters.

 The default value of this property is `45`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textMaxAngle;

/**
 Rotates the text clockwise.

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textRotate;

/**
 Size of the additional area around the text bounding box used for detecting symbol collisions.

 The default value of this property is `2`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textPadding;

/**
 If true, the text may be flipped vertically to prevent it from being rendered upside-down.

 The default value of this property is `true`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textKeepUpright;

/**
 Specifies how to capitalize text, similar to the CSS `text-transform` property.

 The default value of this property is `none`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textTransform;

/**
 Offset distance of text from its anchor. Positive values indicate right and down, while negative values indicate left and up.

 The default value of this property is `0,0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textOffset;

/**
 If true, the text will be visible even if it collides with other previously drawn symbols.

 The default value of this property is `false`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textAllowOverlap;

/**
 If true, other symbols can be visible even if they collide with the text.

 The default value of this property is `false`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textIgnorePlacement;

/**
 If true, icons will display without their corresponding text when the text collides with other symbols and the icon does not.

 The default value of this property is `false`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textOptional;

#pragma mark - Accessing the Paint Attributes

/**
 The opacity at which the icon will be drawn.

 The default value of this property is `1`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconOpacity;

/**
 The color of the icon. This can only be used with sdf icons.

 The default value of this property is `#000000`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconColor;

/**
 The color of the icon's halo. Icon halos can only be used with sdf icons.

 The default value of this property is `rgba(0, 0, 0, 0)`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconHaloColor;

/**
 Distance of halo to the icon outline.

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconHaloWidth;

/**
 Fade out the halo towards the outside.

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconHaloBlur;

/**
 Distance that the icon's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.

 The default value of this property is `0,0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen).

 The default value of this property is `map`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconTranslateAnchor;

/**
 The opacity at which the text will be drawn.

 The default value of this property is `1`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textOpacity;

/**
 The color with which the text will be drawn.

 The default value of this property is `#000000`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textColor;

/**
 The color of the text's halo, which helps it stand out from backgrounds.

 The default value of this property is `rgba(0, 0, 0, 0)`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textHaloColor;

/**
 Distance of halo to the font outline. Max text halo width is 1/4 of the font-size.

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textHaloWidth;

/**
 The halo's fadeout distance towards the outside.

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textHaloBlur;

/**
 Distance that the text's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.

 The default value of this property is `0,0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen).

 The default value of this property is `map`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textTranslateAnchor;

@end

NS_ASSUME_NONNULL_END
