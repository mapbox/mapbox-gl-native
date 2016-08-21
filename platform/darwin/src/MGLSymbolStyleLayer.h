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
 Label placement relative to its geometry. `MGLSymbolStyleLayerSymbolPlacementLine` can only be used on LineStrings and Polygons.
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLSymbolStyleLayerSymbolPlacementPoint`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> symbolPlacement;

/**
 Distance between two symbol anchors.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `250`.
 
 This property is only applied to the style if `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> symbolSpacing;

/**
 If true, the symbols will not cross tile edges to avoid mutual collisions. Recommended in layers that don't have enough padding in the vector tile to prevent collisions, or if it is a point symbol layer placed after a line symbol layer.
 
 If this property is set to `nil`, the layer uses an implicit default value of `NO`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> symbolAvoidEdges;

/**
 If true, the icon will be visible even if it collides with other previously drawn symbols.
 
 If this property is set to `nil`, the layer uses an implicit default value of `NO`.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconAllowOverlap;

/**
 If true, other symbols can be visible even if they collide with the icon.
 
 If this property is set to `nil`, the layer uses an implicit default value of `NO`.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconIgnorePlacement;

/**
 If true, text will display without their corresponding icons when the icon collides with other symbols and the text does not.
 
 If this property is set to `nil`, the layer uses an implicit default value of `NO`.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconOptional;

/**
 Orientation of icon when map is rotated.
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLSymbolStyleLayerIconRotationAlignmentViewport`.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconRotationAlignment;

/**
 Scale factor for icon. 1 is original size, 3 triples the size.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1`.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconSize;

/**
 Position and scale an icon by the its corresponding text.
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLSymbolStyleLayerIconTextFitNone`.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconTextFit;

/**
 Size of padding area around the text-fit size in clockwise order: top, right, bottom, left.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `NSEdgeInsetsZero` or `UIEdgeInsetsZero`.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `iconTextFit` is non-`nil`, and `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconTextFitPadding;

/**
 A string with {tokens} replaced, referencing the data property to pull from.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconImage;

/**
 Rotates the icon clockwise.

 This property is measured in degrees.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconRotate;

/**
 Size of the additional area around the icon bounding box used for detecting symbol collisions.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `2`.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconPadding;

/**
 If true, the icon may be flipped to prevent it from being rendered upside-down.
 
 If this property is set to `nil`, the layer uses an implicit default value of `NO`.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `iconRotationAlignment` is set to `MGLSymbolStyleLayerIconRotationAlignmentMap`, and `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconKeepUpright;

/**
 Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up.
 
 If this property is set to `nil`, the layer uses an implicit default value of 0 from the left and 0 from the top.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconOffset;

/**
 Aligns text to the plane of the `MGLSymbolStyleLayerTextPitchAlignmentViewport` or the `MGLSymbolStyleLayerTextPitchAlignmentMap` when the map is pitched. Matches `textRotationAlignment` if unspecified.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textPitchAlignment;

/**
 Orientation of text when map is rotated.
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLSymbolStyleLayerTextRotationAlignmentViewport`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textRotationAlignment;

/**
 Value to use for a text label. Feature properties are specified using tokens like {field_name}.
 
 If this property is set to `nil`, the layer uses an implicit default value of ``.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textField;

/**
 Font stack to use for displaying text.
 
 If this property is set to `nil`, the layer uses an implicit default value of `Open Sans Regular`, `Arial Unicode MS Regular`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textFont;

/**
 Font size.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `16`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textSize;

/**
 The maximum line width for text wrapping.

 This property is measured in ems.
 
 If this property is set to `nil`, the layer uses an implicit default value of `10`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textMaxWidth;

/**
 Text leading value for multi-line text.

 This property is measured in ems.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1.2`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textLineHeight;

/**
 Text tracking amount.

 This property is measured in ems.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textLetterSpacing;

/**
 Text justification options.
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLSymbolStyleLayerTextJustifyCenter`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textJustify;

/**
 Part of the text placed closest to the anchor.
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLSymbolStyleLayerTextAnchorCenter`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textAnchor;

/**
 Maximum angle change between adjacent characters.

 This property is measured in degrees.
 
 If this property is set to `nil`, the layer uses an implicit default value of `45`.
 
 This property is only applied to the style if `textField` is non-`nil`, and `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textMaxAngle;

/**
 Rotates the text clockwise.

 This property is measured in degrees.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textRotate;

/**
 Size of the additional area around the text bounding box used for detecting symbol collisions.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `2`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textPadding;

/**
 If true, the text may be flipped vertically to prevent it from being rendered upside-down.
 
 If this property is set to `nil`, the layer uses an implicit default value of `YES`.
 
 This property is only applied to the style if `textField` is non-`nil`, and `textRotationAlignment` is set to `MGLSymbolStyleLayerTextRotationAlignmentMap`, and `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textKeepUpright;

/**
 Specifies how to capitalize text, similar to the CSS `text-transform` property.
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLSymbolStyleLayerTextTransformNone`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textTransform;

/**
 Offset distance of text from its anchor. Positive values indicate right and down, while negative values indicate left and up.

 This property is measured in ems.
 
 If this property is set to `nil`, the layer uses an implicit default value of 0 ems from the left and 0 ems from the top.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textOffset;

/**
 If true, the text will be visible even if it collides with other previously drawn symbols.
 
 If this property is set to `nil`, the layer uses an implicit default value of `NO`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textAllowOverlap;

/**
 If true, other symbols can be visible even if they collide with the text.
 
 If this property is set to `nil`, the layer uses an implicit default value of `NO`.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textIgnorePlacement;

/**
 If true, icons will display without their corresponding text when the text collides with other symbols and the icon does not.
 
 If this property is set to `nil`, the layer uses an implicit default value of `NO`.
 
 This property is only applied to the style if `textField` is non-`nil`, and `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textOptional;

#pragma mark - Accessing the Paint Attributes

/**
 The opacity at which the icon will be drawn.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1`.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconOpacity;

/**
 The color of the icon. This can only be used with sdf icons.
 
 If this property is set to `nil`, the layer uses an implicit default value of `blackColor` with an alpha value of 1.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconColor;

/**
 The color of the icon's halo. Icon halos can only be used with sdf icons.
 
 If this property is set to `nil`, the layer uses an implicit default value of `blackColor` with an alpha value of 0.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconHaloColor;

/**
 Distance of halo to the icon outline.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconHaloWidth;

/**
 Fade out the halo towards the outside.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconHaloBlur;

/**
 Distance that the icon's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of 0 points from the left and 0 points from the top.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen).
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLSymbolStyleLayerIconTranslateAnchorMap`.

 This property is only applied to the style if `iconImage` is non-`nil`, and `iconTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> iconTranslateAnchor;

/**
 The opacity at which the text will be drawn.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1`.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textOpacity;

/**
 The color with which the text will be drawn.
 
 If this property is set to `nil`, the layer uses an implicit default value of `blackColor` with an alpha value of 1.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textColor;

/**
 The color of the text's halo, which helps it stand out from backgrounds.
 
 If this property is set to `nil`, the layer uses an implicit default value of `blackColor` with an alpha value of 0.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textHaloColor;

/**
 Distance of halo to the font outline. Max text halo width is 1/4 of the font-size.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textHaloWidth;

/**
 The halo's fadeout distance towards the outside.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textHaloBlur;

/**
 Distance that the text's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of 0 points from the left and 0 points from the top.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen).
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLSymbolStyleLayerTextTranslateAnchorMap`.

 This property is only applied to the style if `textField` is non-`nil`, and `textTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> textTranslateAnchor;

@end

NS_ASSUME_NONNULL_END
