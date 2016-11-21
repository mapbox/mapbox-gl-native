// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 In combination with `symbolPlacement`, determines the rotation behavior of icons.
 
 Values of this type are used in the `iconRotationAlignment` property of `MGLSymbolStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLIconRotationAlignment) {
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, aligns icons east-west. When `symbolPlacement` is set to `MGLSymbolPlacementLine`, aligns icon x-axes with the line.
     */
    MGLIconRotationAlignmentMap,
    /**
     Produces icons whose x-axes are aligned with the x-axis of the viewport, regardless of the value of `symbolPlacement`.
     */
    MGLIconRotationAlignmentViewport,
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, this is equivalent to `MGLIconRotationAlignmentViewport`. When `symbolPlacement` is set to `MGLSymbolPlacementLine`, this is equivalent to `MGLIconRotationAlignmentMap`.
     */
    MGLIconRotationAlignmentAuto,
};

/**
 Scales the icon to fit around the associated text.
 
 Values of this type are used in the `iconTextFit` property of `MGLSymbolStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLIconTextFit) {
    /**
     The icon is displayed at its intrinsic aspect ratio.
     */
    MGLIconTextFitNone,
    /**
     The icon is scaled in the x-dimension to fit the width of the text.
     */
    MGLIconTextFitWidth,
    /**
     The icon is scaled in the y-dimension to fit the height of the text.
     */
    MGLIconTextFitHeight,
    /**
     The icon is scaled in both x- and y-dimensions.
     */
    MGLIconTextFitBoth,
};

/**
 Label placement relative to its geometry.
 
 Values of this type are used in the `symbolPlacement` property of `MGLSymbolStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolPlacement) {
    /**
     The label is placed at the point where the geometry is located.
     */
    MGLSymbolPlacementPoint,
    /**
     The label is placed along the line of the geometry. Can only be used on `LineString` and `Polygon` geometries.
     */
    MGLSymbolPlacementLine,
};

/**
 Part of the text placed closest to the anchor.
 
 Values of this type are used in the `textAnchor` property of `MGLSymbolStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLTextAnchor) {
    /**
     The center of the text is placed closest to the anchor.
     */
    MGLTextAnchorCenter,
    /**
     The left side of the text is placed closest to the anchor.
     */
    MGLTextAnchorLeft,
    /**
     The right side of the text is placed closest to the anchor.
     */
    MGLTextAnchorRight,
    /**
     The top of the text is placed closest to the anchor.
     */
    MGLTextAnchorTop,
    /**
     The bottom of the text is placed closest to the anchor.
     */
    MGLTextAnchorBottom,
    /**
     The top left corner of the text is placed closest to the anchor.
     */
    MGLTextAnchorTopLeft,
    /**
     The top right corner of the text is placed closest to the anchor.
     */
    MGLTextAnchorTopRight,
    /**
     The bottom left corner of the text is placed closest to the anchor.
     */
    MGLTextAnchorBottomLeft,
    /**
     The bottom right corner of the text is placed closest to the anchor.
     */
    MGLTextAnchorBottomRight,
};

/**
 Text justification options.
 
 Values of this type are used in the `textJustify` property of `MGLSymbolStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLTextJustify) {
    /**
     The text is aligned to the left.
     */
    MGLTextJustifyLeft,
    /**
     The text is centered.
     */
    MGLTextJustifyCenter,
    /**
     The text is aligned to the right.
     */
    MGLTextJustifyRight,
};

/**
 Orientation of text when map is pitched.
 
 Values of this type are used in the `textPitchAlignment` property of `MGLSymbolStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLTextPitchAlignment) {
    /**
     The text is aligned to the plane of the map.
     */
    MGLTextPitchAlignmentMap,
    /**
     The text is aligned to the plane of the viewport.
     */
    MGLTextPitchAlignmentViewport,
    /**
     Automatically matches the value of `textRotationAlignment`.
     */
    MGLTextPitchAlignmentAuto,
};

/**
 In combination with `symbolPlacement`, determines the rotation behavior of the individual glyphs forming the text.
 
 Values of this type are used in the `textRotationAlignment` property of `MGLSymbolStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLTextRotationAlignment) {
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, aligns text east-west. When `symbolPlacement` is set to `MGLSymbolPlacementLine`, aligns text x-axes with the line.
     */
    MGLTextRotationAlignmentMap,
    /**
     Produces glyphs whose x-axes are aligned with the x-axis of the viewport, regardless of the value of `symbolPlacement`.
     */
    MGLTextRotationAlignmentViewport,
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, this is equivalent to `MGLTextRotationAlignmentViewport`. When `symbolPlacement` is set to `MGLSymbolPlacementLine`, this is equivalent to `MGLTextRotationAlignmentMap`.
     */
    MGLTextRotationAlignmentAuto,
};

/**
 Specifies how to capitalize text.
 
 Values of this type are used in the `textTransform` property of `MGLSymbolStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLTextTransform) {
    /**
     The text is not altered.
     */
    MGLTextTransformNone,
    /**
     Forces all letters to be displayed in uppercase.
     */
    MGLTextTransformUppercase,
    /**
     Forces all letters to be displayed in lowercase.
     */
    MGLTextTransformLowercase,
};

/**
 Controls the translation reference point.
 
 Values of this type are used in the `iconTranslateAnchor` property of `MGLSymbolStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLIconTranslateAnchor) {
    /**
     Icons are translated relative to the map.
     */
    MGLIconTranslateAnchorMap,
    /**
     Icons are translated relative to the viewport.
     */
    MGLIconTranslateAnchorViewport,
};

/**
 Controls the translation reference point.
 
 Values of this type are used in the `textTranslateAnchor` property of `MGLSymbolStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLTextTranslateAnchor) {
    /**
     The text is translated relative to the map.
     */
    MGLTextTranslateAnchorMap,
    /**
     The text is translated relative to the viewport.
     */
    MGLTextTranslateAnchorViewport,
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
 If true, the icon will be visible even if it collides with other previously drawn symbols.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconAllowOverlap;

/**
 If true, other symbols can be visible even if they collide with the icon.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconIgnorePlacement;

/**
 A string with {tokens} replaced, referencing the data property to pull from.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSString *> *iconImageName;

/**
 If true, the icon may be flipped to prevent it from being rendered upside-down.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `iconRotationAlignment` is set to an `MGLStyleValue` object containing an `NSValue` object containing `MGLIconRotationAlignmentMap`, and `symbolPlacement` is set to an `MGLStyleValue` object containing an `NSValue` object containing `MGLSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconKeepUpright;

/**
 Offset distance of icon from its anchor.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing a `CGVector` struct set to 0 from the left and 0 from the top. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconOffset;

/**
 If true, text will display without their corresponding icons when the icon collides with other symbols and the text does not.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconOptional;

/**
 Size of the additional area around the icon bounding box used for detecting symbol collisions.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconPadding;

/**
 Rotates the icon clockwise.

 This property is measured in degrees.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconRotate;

/**
 In combination with `symbolPlacement`, determines the rotation behavior of icons.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLIconRotationAlignmentAuto`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconRotationAlignment;

/**
 Scale factor for icon. 1 is original size, 3 triples the size.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconSize;

/**
 Scales the icon to fit around the associated text.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLIconTextFitNone`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTextFit;

/**
 Size of the additional area added to dimensions determined by `iconTextFit`.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `NSEdgeInsetsZero` or `UIEdgeInsetsZero`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImage` is non-`nil`, and `textField` is non-`nil`, and `iconTextFit` is set to an `MGLStyleValue` object containing an `NSValue` object containing `MGLIconTextFitBoth`, `MGLIconTextFitWidth`, or `MGLIconTextFitHeight`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTextFitPadding;

/**
 If true, the symbols will not cross tile edges to avoid mutual collisions. Recommended in layers that don't have enough padding in the vector tile to prevent collisions, or if it is a point symbol layer placed after a line symbol layer.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *symbolAvoidEdges;

/**
 Label placement relative to its geometry.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLSymbolPlacementPoint`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *symbolPlacement;

/**
 Distance between two symbol anchors.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `250`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `symbolPlacement` is set to an `MGLStyleValue` object containing an `NSValue` object containing `MGLSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *symbolSpacing;

/**
 If true, the text will be visible even if it collides with other previously drawn symbols.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textAllowOverlap;

/**
 Part of the text placed closest to the anchor.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLTextAnchorCenter`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textAnchor;

/**
 Value to use for a text label. Feature properties are specified using tokens like {field_name}.
 
 The default value of this property is an `MGLStyleValue` object containing the empty string. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSString *> *textField;

/**
 Font stack to use for displaying text.
 
 The default value of this property is an `MGLStyleValue` object containing the array `Open Sans Regular`, `Arial Unicode MS Regular`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSArray<NSString *> *> *textFont;

/**
 If true, other symbols can be visible even if they collide with the text.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textIgnorePlacement;

/**
 Text justification options.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLTextJustifyCenter`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textJustify;

/**
 If true, the text may be flipped vertically to prevent it from being rendered upside-down.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing `YES`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`, and `textRotationAlignment` is set to an `MGLStyleValue` object containing an `NSValue` object containing `MGLTextRotationAlignmentMap`, and `symbolPlacement` is set to an `MGLStyleValue` object containing an `NSValue` object containing `MGLSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textKeepUpright;

/**
 Text tracking amount.

 This property is measured in ems.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textLetterSpacing;

/**
 Text leading value for multi-line text.

 This property is measured in ems.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `1.2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textLineHeight;

/**
 Maximum angle change between adjacent characters.

 This property is measured in degrees.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `45`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`, and `symbolPlacement` is set to an `MGLStyleValue` object containing an `NSValue` object containing `MGLSymbolPlacementLine`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textMaxAngle;

/**
 The maximum line width for text wrapping.

 This property is measured in ems.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `10`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textMaxWidth;

/**
 Offset distance of text from its anchor.

 This property is measured in ems.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing a `CGVector` struct set to 0 ems from the left and 0 ems from the top. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textOffset;

/**
 If true, icons will display without their corresponding text when the text collides with other symbols and the icon does not.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing `NO`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`, and `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textOptional;

/**
 Size of the additional area around the text bounding box used for detecting symbol collisions.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textPadding;

/**
 Orientation of text when map is pitched.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLTextPitchAlignmentAuto`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textPitchAlignment;

/**
 Rotates the text clockwise.

 This property is measured in degrees.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textRotate;

/**
 In combination with `symbolPlacement`, determines the rotation behavior of the individual glyphs forming the text.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLTextRotationAlignmentAuto`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textRotationAlignment;

/**
 Font size.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `16`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textSize;

/**
 Specifies how to capitalize text.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLTextTransformNone`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textTransform;

#pragma mark - Accessing the Paint Attributes

#if TARGET_OS_IPHONE
/**
 The color of the icon. This can only be used with sdf icons.
 
 The default value of this property is an `MGLStyleValue` object containing `UIColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *iconColor;
#else
/**
 The color of the icon. This can only be used with sdf icons.
 
 The default value of this property is an `MGLStyleValue` object containing `NSColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *iconColor;
#endif

/**
 Fade out the halo towards the outside.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconHaloBlur;

#if TARGET_OS_IPHONE
/**
 The color of the icon's halo. Icon halos can only be used with SDF icons.
 
 The default value of this property is an `MGLStyleValue` object containing `UIColor.clearColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *iconHaloColor;
#else
/**
 The color of the icon's halo. Icon halos can only be used with SDF icons.
 
 The default value of this property is an `MGLStyleValue` object containing `NSColor.clearColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *iconHaloColor;
#endif

/**
 Distance of halo to the icon outline.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconHaloWidth;

/**
 The opacity at which the icon will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconOpacity;

/**
 Distance that the icon's anchor is moved from its original placement.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing a `CGVector` struct set to 0 points from the left and 0 points from the top. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTranslate;

/**
 Controls the translation reference point.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLIconTranslateAnchorMap`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `iconImage` is non-`nil`, and `iconTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTranslateAnchor;

#if TARGET_OS_IPHONE
/**
 The color with which the text will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing `UIColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *textColor;
#else
/**
 The color with which the text will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing `NSColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *textColor;
#endif

/**
 The halo's fadeout distance towards the outside.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textHaloBlur;

#if TARGET_OS_IPHONE
/**
 The color of the text's halo, which helps it stand out from backgrounds.
 
 The default value of this property is an `MGLStyleValue` object containing `UIColor.clearColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *textHaloColor;
#else
/**
 The color of the text's halo, which helps it stand out from backgrounds.
 
 The default value of this property is an `MGLStyleValue` object containing `NSColor.clearColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *textHaloColor;
#endif

/**
 Distance of halo to the font outline. Max text halo width is 1/4 of the font-size.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textHaloWidth;

/**
 The opacity at which the text will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textOpacity;

/**
 Distance that the text's anchor is moved from its original placement.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing a `CGVector` struct set to 0 points from the left and 0 points from the top. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textTranslate;

/**
 Controls the translation reference point.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLTextTranslateAnchorMap`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `textField` is non-`nil`, and `textTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textTranslateAnchor;

@end

NS_ASSUME_NONNULL_END
