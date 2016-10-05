// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Label placement relative to its geometry.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerSymbolPlacement) {
    /**
     The label is placed at the point where the geometry is located.
     */
    MGLSymbolStyleLayerSymbolPlacementPoint,
    /**
     The label is placed along the line of the geometry. Can only be used on `LineString` and `Polygon` geometries.
     */
    MGLSymbolStyleLayerSymbolPlacementLine,
};

/**
 In combination with `symbolPlacement`, determines the rotation behavior of icons.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerIconRotationAlignment) {
    /**
     When `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementPoint`, aligns icons east-west. When `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementLine`, aligns icon x-axes with the line.
     */
    MGLSymbolStyleLayerIconRotationAlignmentMap,
    /**
     Produces icons whose x-axes are aligned with the x-axis of the viewport, regardless of the value of `symbolPlacement`.
     */
    MGLSymbolStyleLayerIconRotationAlignmentViewport,
    /**
     When `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementPoint`, this is equivalent to `MGLSymbolStyleLayerIconRotationAlignmentViewport`. When `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementLine`, this is equivalent to `MGLSymbolStyleLayerIconRotationAlignmentMap`.
     */
    MGLSymbolStyleLayerIconRotationAlignmentAuto,
};

/**
 Scales the icon to fit around the associated text.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerIconTextFit) {
    /**
     The icon is displayed at its intrinsic aspect ratio.
     */
    MGLSymbolStyleLayerIconTextFitNone,
    /**
     The icon is scaled in the x-dimension to fit the width of the text.
     */
    MGLSymbolStyleLayerIconTextFitWidth,
    /**
     The icon is scaled in the y-dimension to fit the height of the text.
     */
    MGLSymbolStyleLayerIconTextFitHeight,
    /**
     The icon is scaled in both x- and y-dimensions.
     */
    MGLSymbolStyleLayerIconTextFitBoth,
};

/**
 Orientation of text when map is pitched.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextPitchAlignment) {
    /**
     The text is aligned to the plane of the map.
     */
    MGLSymbolStyleLayerTextPitchAlignmentMap,
    /**
     The text is aligned to the plane of the viewport.
     */
    MGLSymbolStyleLayerTextPitchAlignmentViewport,
    /**
     Automatically matches the value of `textRotationAlignment`.
     */
    MGLSymbolStyleLayerTextPitchAlignmentAuto,
};

/**
 In combination with `symbolPlacement`, determines the rotation behavior of the individual glyphs forming the text.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextRotationAlignment) {
    /**
     When `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementPoint`, aligns text east-west. When `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementLine`, aligns text x-axes with the line.
     */
    MGLSymbolStyleLayerTextRotationAlignmentMap,
    /**
     Produces glyphs whose x-axes are aligned with the x-axis of the viewport, regardless of the value of `symbolPlacement`.
     */
    MGLSymbolStyleLayerTextRotationAlignmentViewport,
    /**
     When `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementPoint`, this is equivalent to `MGLSymbolStyleLayerTextRotationAlignmentViewport`. When `symbolPlacement` is set to `MGLSymbolStyleLayerSymbolPlacementLine`, this is equivalent to `MGLSymbolStyleLayerTextRotationAlignmentMap`.
     */
    MGLSymbolStyleLayerTextRotationAlignmentAuto,
};

/**
 Text justification options.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextJustify) {
    /**
     The text is aligned to the left.
     */
    MGLSymbolStyleLayerTextJustifyLeft,
    /**
     The text is centered.
     */
    MGLSymbolStyleLayerTextJustifyCenter,
    /**
     The text is aligned to the right.
     */
    MGLSymbolStyleLayerTextJustifyRight,
};

/**
 Part of the text placed closest to the anchor.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextAnchor) {
    /**
     The center of the text is placed closest to the anchor.
     */
    MGLSymbolStyleLayerTextAnchorCenter,
    /**
     The left side of the text is placed closest to the anchor.
     */
    MGLSymbolStyleLayerTextAnchorLeft,
    /**
     The right side of the text is placed closest to the anchor.
     */
    MGLSymbolStyleLayerTextAnchorRight,
    /**
     The top of the text is placed closest to the anchor.
     */
    MGLSymbolStyleLayerTextAnchorTop,
    /**
     The bottom of the text is placed closest to the anchor.
     */
    MGLSymbolStyleLayerTextAnchorBottom,
    /**
     The top left corner of the text is placed closest to the anchor.
     */
    MGLSymbolStyleLayerTextAnchorTopLeft,
    /**
     The top right corner of the text is placed closest to the anchor.
     */
    MGLSymbolStyleLayerTextAnchorTopRight,
    /**
     The bottom left corner of the text is placed closest to the anchor.
     */
    MGLSymbolStyleLayerTextAnchorBottomLeft,
    /**
     The bottom right corner of the text is placed closest to the anchor.
     */
    MGLSymbolStyleLayerTextAnchorBottomRight,
};

/**
 Specifies how to capitalize text, similar to the CSS `text-transform` property.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextTransform) {
    /**
     The text is not altered.
     */
    MGLSymbolStyleLayerTextTransformNone,
    /**
     Forces all letters to be displayed in uppercase.
     */
    MGLSymbolStyleLayerTextTransformUppercase,
    /**
     Forces all letters to be displayed in lowercase.
     */
    MGLSymbolStyleLayerTextTransformLowercase,
};

/**
 Controls the translation reference point.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerIconTranslateAnchor) {
    /**
     Icons are translated relative to the map.
     */
    MGLSymbolStyleLayerIconTranslateAnchorMap,
    /**
     Icons are translated relative to the viewport.
     */
    MGLSymbolStyleLayerIconTranslateAnchorViewport,
};

/**
 Controls the translation reference point.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextTranslateAnchor) {
    /**
     The text is translated relative to the map.
     */
    MGLSymbolStyleLayerTextTranslateAnchorMap,
    /**
     The text is translated relative to the viewport.
     */
    MGLSymbolStyleLayerTextTranslateAnchorViewport,
};

/**
 A symbol layer which allows customization of styling properties at runtime. You may 
 instantiate a new symbol layer to add to a map style or you may query an 
 `MGLMapView` for its `style` and obtain existing layers using the 
 `-[MGLStyle layerWithIdentifier:]` method. 
 */
@interface MGLSymbolStyleLayer : MGLVectorStyleLayer

#pragma mark - Accessing the Layout Attributes

/**
 Label placement relative to its geometry.
 
 The default value of this property is an `NSValue` object containing `MGLSymbolStyleLayerSymbolPlacementPoint`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> symbolPlacement;

/**
 Distance between two symbol anchors.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `250`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `symbolPlacement` is set to an `NSValue` object containing `MGLSymbolStyleLayerSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> symbolSpacing;

/**
 If true, the symbols will not cross tile edges to avoid mutual collisions. Recommended in layers that don't have enough padding in the vector tile to prevent collisions, or if it is a point symbol layer placed after a line symbol layer.
 
 The default value of this property is an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> symbolAvoidEdges;

/**
 If true, the icon will be visible even if it collides with other previously drawn symbols.
 
 The default value of this property is an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconAllowOverlap;

/**
 If true, other symbols can be visible even if they collide with the icon.
 
 The default value of this property is an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconIgnorePlacement;

/**
 If true, text will display without their corresponding icons when the icon collides with other symbols and the text does not.
 
 The default value of this property is an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconOptional;

/**
 In combination with `symbolPlacement`, determines the rotation behavior of icons.
 
 The default value of this property is an `NSValue` object containing `MGLSymbolStyleLayerIconRotationAlignmentAuto`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconRotationAlignment;

/**
 Scale factor for icon. 1 is original size, 3 triples the size.
 
 The default value of this property is an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconSize;

/**
 Scales the icon to fit around the associated text.
 
 The default value of this property is an `NSValue` object containing `MGLSymbolStyleLayerIconTextFitNone`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconTextFit;

/**
 Size of the additional area added to dimensions determined by `iconTextFit`, in clockwise order: top, right, bottom, left.

 This property is measured in points.
 
 The default value of this property is an `NSValue` object containing `NSEdgeInsetsZero` or `UIEdgeInsetsZero`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `textField` is non-`nil`, and `iconTextFit` is set to an `NSValue` object containing `MGLSymbolStyleLayerIconTextFitBoth`, `MGLSymbolStyleLayerIconTextFitWidth`, or `MGLSymbolStyleLayerIconTextFitHeight`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconTextFitPadding;

/**
 A string with {tokens} replaced, referencing the data property to pull from.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconImage;

/**
 Rotates the icon clockwise.

 This property is measured in degrees.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconRotate;

/**
 Size of the additional area around the icon bounding box used for detecting symbol collisions.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconPadding;

/**
 If true, the icon may be flipped to prevent it from being rendered upside-down.
 
 The default value of this property is an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `iconRotationAlignment` is set to an `NSValue` object containing `MGLSymbolStyleLayerIconRotationAlignmentMap`, and `symbolPlacement` is set to an `NSValue` object containing `MGLSymbolStyleLayerSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconKeepUpright;

/**
 Offset distance of icon from its anchor. Positive values indicate right and down, while negative values indicate left and up.
 
 The default value of this property is an `NSValue` object containing a `CGVector` struct set to 0 from the left and 0 from the top. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconOffset;

/**
 Orientation of text when map is pitched.
 
 The default value of this property is an `NSValue` object containing `MGLSymbolStyleLayerTextPitchAlignmentAuto`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textPitchAlignment;

/**
 In combination with `symbolPlacement`, determines the rotation behavior of the individual glyphs forming the text.
 
 The default value of this property is an `NSValue` object containing `MGLSymbolStyleLayerTextRotationAlignmentAuto`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textRotationAlignment;

/**
 Value to use for a text label. Feature properties are specified using tokens like {field_name}.
 
 The default value of this property is the string ``. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textField;

/**
 Font stack to use for displaying text.
 
 The default value of this property is the array `Open Sans Regular`, `Arial Unicode MS Regular`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textFont;

/**
 Font size.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `16`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textSize;

/**
 The maximum line width for text wrapping.

 This property is measured in ems.
 
 The default value of this property is an `NSNumber` object containing the float `10`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textMaxWidth;

/**
 Text leading value for multi-line text.

 This property is measured in ems.
 
 The default value of this property is an `NSNumber` object containing the float `1.2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textLineHeight;

/**
 Text tracking amount.

 This property is measured in ems.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textLetterSpacing;

/**
 Text justification options.
 
 The default value of this property is an `NSValue` object containing `MGLSymbolStyleLayerTextJustifyCenter`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textJustify;

/**
 Part of the text placed closest to the anchor.
 
 The default value of this property is an `NSValue` object containing `MGLSymbolStyleLayerTextAnchorCenter`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textAnchor;

/**
 Maximum angle change between adjacent characters.

 This property is measured in degrees.
 
 The default value of this property is an `NSNumber` object containing the float `45`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`, and `symbolPlacement` is set to an `NSValue` object containing `MGLSymbolStyleLayerSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textMaxAngle;

/**
 Rotates the text clockwise.

 This property is measured in degrees.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textRotate;

/**
 Size of the additional area around the text bounding box used for detecting symbol collisions.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textPadding;

/**
 If true, the text may be flipped vertically to prevent it from being rendered upside-down.
 
 The default value of this property is an `NSNumber` object containing `YES`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`, and `textRotationAlignment` is set to an `NSValue` object containing `MGLSymbolStyleLayerTextRotationAlignmentMap`, and `symbolPlacement` is set to an `NSValue` object containing `MGLSymbolStyleLayerSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textKeepUpright;

/**
 Specifies how to capitalize text, similar to the CSS `text-transform` property.
 
 The default value of this property is an `NSValue` object containing `MGLSymbolStyleLayerTextTransformNone`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textTransform;

/**
 Offset distance of text from its anchor. Positive values indicate right and down, while negative values indicate left and up.

 This property is measured in ems.
 
 The default value of this property is an `NSValue` object containing a `CGVector` struct set to 0 ems from the left and 0 ems from the top. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textOffset;

/**
 If true, the text will be visible even if it collides with other previously drawn symbols.
 
 The default value of this property is an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textAllowOverlap;

/**
 If true, other symbols can be visible even if they collide with the text.
 
 The default value of this property is an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textIgnorePlacement;

/**
 If true, icons will display without their corresponding text when the text collides with other symbols and the icon does not.
 
 The default value of this property is an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`, and `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textOptional;

#pragma mark - Accessing the Paint Attributes

/**
 The opacity at which the icon will be drawn.
 
 The default value of this property is an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconOpacity;

#if TARGET_OS_IPHONE
/**
 The color of the icon. This can only be used with sdf icons.
 
 The default value of this property is `UIColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconColor;
#else
/**
 The color of the icon. This can only be used with sdf icons.
 
 The default value of this property is `NSColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconColor;
#endif

#if TARGET_OS_IPHONE
/**
 The color of the icon's halo. Icon halos can only be used with SDF icons.
 
 The default value of this property is `UIColor.clearColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconHaloColor;
#else
/**
 The color of the icon's halo. Icon halos can only be used with SDF icons.
 
 The default value of this property is `NSColor.clearColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconHaloColor;
#endif

/**
 Distance of halo to the icon outline.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconHaloWidth;

/**
 Fade out the halo towards the outside.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconHaloBlur;

/**
 Distance that the icon's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.

 This property is measured in points.
 
 The default value of this property is an `NSValue` object containing a `CGVector` struct set to 0 points from the left and 0 points from the top. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconTranslate;

/**
 Controls the translation reference point.
 
 The default value of this property is an `NSValue` object containing `MGLSymbolStyleLayerIconTranslateAnchorMap`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`, and `iconTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> iconTranslateAnchor;

/**
 The opacity at which the text will be drawn.
 
 The default value of this property is an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textOpacity;

#if TARGET_OS_IPHONE
/**
 The color with which the text will be drawn.
 
 The default value of this property is `UIColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textColor;
#else
/**
 The color with which the text will be drawn.
 
 The default value of this property is `NSColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textColor;
#endif

#if TARGET_OS_IPHONE
/**
 The color of the text's halo, which helps it stand out from backgrounds.
 
 The default value of this property is `UIColor.clearColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textHaloColor;
#else
/**
 The color of the text's halo, which helps it stand out from backgrounds.
 
 The default value of this property is `NSColor.clearColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textHaloColor;
#endif

/**
 Distance of halo to the font outline. Max text halo width is 1/4 of the font-size.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textHaloWidth;

/**
 The halo's fadeout distance towards the outside.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textHaloBlur;

/**
 Distance that the text's anchor is moved from its original placement. Positive values indicate right and down, while negative values indicate left and up.

 This property is measured in points.
 
 The default value of this property is an `NSValue` object containing a `CGVector` struct set to 0 points from the left and 0 points from the top. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textTranslate;

/**
 Controls the translation reference point.
 
 The default value of this property is an `NSValue` object containing `MGLSymbolStyleLayerTextTranslateAnchorMap`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`, and `textTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> textTranslateAnchor;

@end

NS_ASSUME_NONNULL_END
