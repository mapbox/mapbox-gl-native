// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLFoundation.h"
#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Part of the icon placed closest to the anchor.

 Values of this type are used in the `MGLSymbolStyleLayer.iconAnchor`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLIconAnchor) {
    /**
     The center of the icon is placed closest to the anchor.
     */
    MGLIconAnchorCenter,
    /**
     The left side of the icon is placed closest to the anchor.
     */
    MGLIconAnchorLeft,
    /**
     The right side of the icon is placed closest to the anchor.
     */
    MGLIconAnchorRight,
    /**
     The top of the icon is placed closest to the anchor.
     */
    MGLIconAnchorTop,
    /**
     The bottom of the icon is placed closest to the anchor.
     */
    MGLIconAnchorBottom,
    /**
     The top left corner of the icon is placed closest to the anchor.
     */
    MGLIconAnchorTopLeft,
    /**
     The top right corner of the icon is placed closest to the anchor.
     */
    MGLIconAnchorTopRight,
    /**
     The bottom left corner of the icon is placed closest to the anchor.
     */
    MGLIconAnchorBottomLeft,
    /**
     The bottom right corner of the icon is placed closest to the anchor.
     */
    MGLIconAnchorBottomRight,
};

/**
 Orientation of icon when map is pitched.

 Values of this type are used in the `MGLSymbolStyleLayer.iconPitchAlignment`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLIconPitchAlignment) {
    /**
     The icon is aligned to the plane of the map.
     */
    MGLIconPitchAlignmentMap,
    /**
     The icon is aligned to the plane of the viewport.
     */
    MGLIconPitchAlignmentViewport,
    /**
     Automatically matches the value of `iconRotationAlignment`.
     */
    MGLIconPitchAlignmentAuto,
};

/**
 In combination with `symbolPlacement`, determines the rotation behavior of
 icons.

 Values of this type are used in the `MGLSymbolStyleLayer.iconRotationAlignment`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLIconRotationAlignment) {
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, aligns icons
     east-west. When `symbolPlacement` is set to `MGLSymbolPlacementLine`,
     aligns icon x-axes with the line.
     */
    MGLIconRotationAlignmentMap,
    /**
     Produces icons whose x-axes are aligned with the x-axis of the viewport,
     regardless of the value of `symbolPlacement`.
     */
    MGLIconRotationAlignmentViewport,
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, this is
     equivalent to `MGLIconRotationAlignmentViewport`. When `symbolPlacement` is
     set to `MGLSymbolPlacementLine`, this is equivalent to
     `MGLIconRotationAlignmentMap`.
     */
    MGLIconRotationAlignmentAuto,
};

/**
 Scales the icon to fit around the associated text.

 Values of this type are used in the `MGLSymbolStyleLayer.iconTextFit`
 property.
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

 Values of this type are used in the `MGLSymbolStyleLayer.symbolPlacement`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolPlacement) {
    /**
     The label is placed at the point where the geometry is located.
     */
    MGLSymbolPlacementPoint,
    /**
     The label is placed along the line of the geometry. Can only be used on
     `LineString` and `Polygon` geometries.
     */
    MGLSymbolPlacementLine,
};

/**
 Part of the text placed closest to the anchor.

 Values of this type are used in the `MGLSymbolStyleLayer.textAnchor`
 property.
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

 Values of this type are used in the `MGLSymbolStyleLayer.textJustification`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLTextJustification) {
    /**
     The text is aligned to the left.
     */
    MGLTextJustificationLeft,
    /**
     The text is centered.
     */
    MGLTextJustificationCenter,
    /**
     The text is aligned to the right.
     */
    MGLTextJustificationRight,
};

/**
 Orientation of text when map is pitched.

 Values of this type are used in the `MGLSymbolStyleLayer.textPitchAlignment`
 property.
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
 In combination with `symbolPlacement`, determines the rotation behavior of the
 individual glyphs forming the text.

 Values of this type are used in the `MGLSymbolStyleLayer.textRotationAlignment`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLTextRotationAlignment) {
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, aligns text
     east-west. When `symbolPlacement` is set to `MGLSymbolPlacementLine`,
     aligns text x-axes with the line.
     */
    MGLTextRotationAlignmentMap,
    /**
     Produces glyphs whose x-axes are aligned with the x-axis of the viewport,
     regardless of the value of `symbolPlacement`.
     */
    MGLTextRotationAlignmentViewport,
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, this is
     equivalent to `MGLTextRotationAlignmentViewport`. When `symbolPlacement` is
     set to `MGLSymbolPlacementLine`, this is equivalent to
     `MGLTextRotationAlignmentMap`.
     */
    MGLTextRotationAlignmentAuto,
};

/**
 Specifies how to capitalize text.

 Values of this type are used in the `MGLSymbolStyleLayer.textTransform`
 property.
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

 Values of this type are used in the `MGLSymbolStyleLayer.iconTranslationAnchor`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLIconTranslationAnchor) {
    /**
     Icons are translated relative to the map.
     */
    MGLIconTranslationAnchorMap,
    /**
     Icons are translated relative to the viewport.
     */
    MGLIconTranslationAnchorViewport,
};

/**
 Controls the translation reference point.

 Values of this type are used in the `MGLSymbolStyleLayer.textTranslationAnchor`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLTextTranslationAnchor) {
    /**
     The text is translated relative to the map.
     */
    MGLTextTranslationAnchorMap,
    /**
     The text is translated relative to the viewport.
     */
    MGLTextTranslationAnchorViewport,
};

/**
 An `MGLSymbolStyleLayer` is a style layer that renders icon and text labels at
 points or along lines on the map.
 
 Use a symbol style layer to configure the visual appearance of labels for
 features in vector tiles loaded by an `MGLVectorSource` object or `MGLShape` or
 `MGLFeature` instances in an `MGLShapeSource` object.

 You can access an existing symbol style layer using the
 `-[MGLStyle layerWithIdentifier:]` method if you know its identifier;
 otherwise, find it using the `MGLStyle.layers` property. You can also create a
 new symbol style layer and add it to the style using a method such as
 `-[MGLStyle addLayer:]`.

 ### Example

 ```swift
 let layer = MGLSymbolStyleLayer(identifier: "coffeeshops", source: pois)
 layer.sourceLayerIdentifier = "pois"
 layer.iconImageName = MGLStyleValue(rawValue: "coffee")
 layer.iconScale = MGLStyleValue(rawValue: 0.5)
 layer.text = MGLStyleValue(rawValue: "{name}")
 layer.textTranslation = MGLStyleValue(rawValue: NSValue(cgVector: CGVector(dx: 10, dy: 0)))
 layer.textJustification = MGLStyleValue(rawValue: NSValue(mglTextJustification: .left))
 layer.textAnchor = MGLStyleValue(rawValue: NSValue(mglTextAnchor: .left))
 layer.predicate = NSPredicate(format: "%K == %@", "venue-type", "coffee")
 mapView.style?.addLayer(layer)
 ```
 */
MGL_EXPORT
@interface MGLSymbolStyleLayer : MGLVectorStyleLayer

/**
 Returns a symbol style layer initialized with an identifier and source.

 After initializing and configuring the style layer, add it to a map view’s
 style using the `-[MGLStyle addLayer:]` or
 `-[MGLStyle insertLayer:belowLayer:]` method.

 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param source The source from which to obtain the data to style. If the source
    has not yet been added to the current style, the behavior is undefined.
 @return An initialized foreground style layer.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source;

#pragma mark - Accessing the Layout Attributes

/**
 If true, the icon will be visible even if it collides with other previously
 drawn symbols.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing `NO`. Set this property to `nil` to reset it to
 the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-allow-overlap"><code>icon-allow-overlap</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconAllowsOverlap;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconAllowOverlap __attribute__((unavailable("Use iconAllowsOverlap instead.")));

/**
 Part of the icon placed closest to the anchor.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLIconAnchorCenter`. Set this property to `nil`
 to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconAnchor;

/**
 If true, other symbols can be visible even if they collide with the icon.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing `NO`. Set this property to `nil` to reset it to
 the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-ignore-placement"><code>icon-ignore-placement</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconIgnoresPlacement;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconIgnorePlacement __attribute__((unavailable("Use iconIgnoresPlacement instead.")));

/**
 Name of image in sprite to use for drawing an image background. A string with
 {tokens} replaced, referencing the data property to pull from.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-image"><code>icon-image</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSString *> *iconImageName;


@property (nonatomic, null_resettable) MGLStyleValue<NSString *> *iconImage __attribute__((unavailable("Use iconImageName instead.")));

#if TARGET_OS_IPHONE
/**
 Offset distance of icon from its anchor.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 rightward and 0
 downward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconOffset;
#else
/**
 Offset distance of icon from its anchor.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 rightward and 0
 upward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconOffset;
#endif

/**
 If true, text will display without their corresponding icons when the icon
 collides with other symbols and the text does not.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing `NO`. Set this property to `nil` to reset it to
 the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `text` is non-`nil`. Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable, getter=isIconOptional) MGLStyleValue<NSNumber *> *iconOptional;

/**
 Size of the additional area around the icon bounding box used for detecting
 symbol collisions.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `2`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconPadding;

/**
 Orientation of icon when map is pitched.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLIconPitchAlignmentAuto`. Set this property to
 `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconPitchAlignment;

/**
 Rotates the icon clockwise.
 
 This property is measured in degrees.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-rotate"><code>icon-rotate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconRotation;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconRotate __attribute__((unavailable("Use iconRotation instead.")));

/**
 In combination with `symbolPlacement`, determines the rotation behavior of
 icons.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLIconRotationAlignmentAuto`. Set this property
 to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconRotationAlignment;

/**
 Scales the original size of the icon by the provided factor. The new point size
 of the image will be the original point size multiplied by `iconSize`. 1 is the
 original size; 3 triples the size of the image.
 
 This property is measured in factor of the original icon sizes.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-size"><code>icon-size</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconScale;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconSize __attribute__((unavailable("Use iconScale instead.")));

/**
 Scales the icon to fit around the associated text.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLIconTextFitNone`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `text` is non-`nil`. Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTextFit;

#if TARGET_OS_IPHONE
/**
 Size of the additional area added to dimensions determined by `iconTextFit`.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `UIEdgeInsetsZero`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `text` is non-`nil`, and `iconTextFit` is set to an `MGLStyleValue` object
 containing an `NSValue` object containing `MGLIconTextFitBoth`,
 `MGLIconTextFitWidth`, or `MGLIconTextFitHeight`. Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTextFitPadding;
#else
/**
 Size of the additional area added to dimensions determined by `iconTextFit`.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `NSEdgeInsetsZero`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `text` is non-`nil`, and `iconTextFit` is set to an `MGLStyleValue` object
 containing an `NSValue` object containing `MGLIconTextFitBoth`,
 `MGLIconTextFitWidth`, or `MGLIconTextFitHeight`. Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTextFitPadding;
#endif

/**
 If true, the icon may be flipped to prevent it from being rendered upside-down.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing `NO`. Set this property to `nil` to reset it to
 the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `iconRotationAlignment` is set to an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLIconRotationAlignmentMap`, and
 `symbolPlacement` is set to an `MGLStyleValue` object containing an `NSValue`
 object containing `MGLSymbolPlacementLine`. Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-keep-upright"><code>icon-keep-upright</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *keepsIconUpright;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconKeepUpright __attribute__((unavailable("Use keepsIconUpright instead.")));

/**
 If true, the text may be flipped vertically to prevent it from being rendered
 upside-down.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing `YES`. Set this property to `nil` to reset it to
 the default value.
 
 This property is only applied to the style if `text` is non-`nil`, and
 `textRotationAlignment` is set to an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLTextRotationAlignmentMap`, and
 `symbolPlacement` is set to an `MGLStyleValue` object containing an `NSValue`
 object containing `MGLSymbolPlacementLine`. Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-keep-upright"><code>text-keep-upright</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *keepsTextUpright;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textKeepUpright __attribute__((unavailable("Use keepsTextUpright instead.")));

/**
 Maximum angle change between adjacent characters.
 
 This property is measured in degrees.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `45`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`, and
 `symbolPlacement` is set to an `MGLStyleValue` object containing an `NSValue`
 object containing `MGLSymbolPlacementLine`. Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-max-angle"><code>text-max-angle</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *maximumTextAngle;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textMaxAngle __attribute__((unavailable("Use maximumTextAngle instead.")));

/**
 The maximum line width for text wrapping.
 
 This property is measured in ems.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `10`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-max-width"><code>text-max-width</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *maximumTextWidth;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textMaxWidth __attribute__((unavailable("Use maximumTextWidth instead.")));

/**
 If true, the symbols will not cross tile edges to avoid mutual collisions.
 Recommended in layers that don't have enough padding in the vector tile to
 prevent collisions, or if it is a point symbol layer placed after a line symbol
 layer.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing `NO`. Set this property to `nil` to reset it to
 the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-symbol-avoid-edges"><code>symbol-avoid-edges</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *symbolAvoidsEdges;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *symbolAvoidEdges __attribute__((unavailable("Use symbolAvoidsEdges instead.")));

/**
 Label placement relative to its geometry.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLSymbolPlacementPoint`. Set this property to
 `nil` to reset it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *symbolPlacement;

/**
 Distance between two symbol anchors.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `250`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `symbolPlacement` is set to an
 `MGLStyleValue` object containing an `NSValue` object containing
 `MGLSymbolPlacementLine`. Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *symbolSpacing;

/**
 Value to use for a text label. Feature properties are specified using tokens
 like {field_name}.  (Token replacement is only supported for literal
 `textField` values--not for property functions.)
 
 The default value of this property is an `MGLStyleValue` object containing the
 empty string. Set this property to `nil` to reset it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-field"><code>text-field</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSString *> *text;


@property (nonatomic, null_resettable) MGLStyleValue<NSString *> *textField __attribute__((unavailable("Use text instead.")));

/**
 If true, the text will be visible even if it collides with other previously
 drawn symbols.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing `NO`. Set this property to `nil` to reset it to
 the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-allow-overlap"><code>text-allow-overlap</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textAllowsOverlap;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textAllowOverlap __attribute__((unavailable("Use textAllowsOverlap instead.")));

/**
 Part of the text placed closest to the anchor.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLTextAnchorCenter`. Set this property to `nil`
 to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textAnchor;

/**
 An array of font face names used to display the text.
 
 Each font name must be included in the `{fontstack}` portion of the JSON
 stylesheet’s <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#glyphs"><code>glyphs</code></a>
 property. You can register a custom font when designing the style in Mapbox
 Studio. Fonts installed on the system are not used.
 
 The first font named in the array is applied to the text. For each character in
 the text, if the first font lacks a glyph for the character, the next font is
 applied as a fallback, and so on.
 
 The default value of this property is an `MGLStyleValue` object containing the
 array `Open Sans Regular`, `Arial Unicode MS Regular`. Set this property to
 `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-font"><code>text-font</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSArray<NSString *> *> *textFontNames;


@property (nonatomic, null_resettable) MGLStyleValue<NSArray<NSString *> *> *textFont __attribute__((unavailable("Use textFontNames instead.")));

/**
 Font size.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `16`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-size"><code>text-size</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textFontSize;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textSize __attribute__((unavailable("Use textFontSize instead.")));

/**
 If true, other symbols can be visible even if they collide with the text.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing `NO`. Set this property to `nil` to reset it to
 the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-ignore-placement"><code>text-ignore-placement</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textIgnoresPlacement;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textIgnorePlacement __attribute__((unavailable("Use textIgnoresPlacement instead.")));

/**
 Text justification options.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLTextJustificationCenter`. Set this property to
 `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-justify"><code>text-justify</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textJustification;


@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textJustify __attribute__((unavailable("Use textJustification instead.")));

/**
 Text tracking amount.
 
 This property is measured in ems.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textLetterSpacing;

/**
 Text leading value for multi-line text.
 
 This property is measured in ems.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1.2`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textLineHeight;

#if TARGET_OS_IPHONE
/**
 Offset distance of text from its anchor.
 
 This property is measured in ems.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 ems rightward and 0
 ems downward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textOffset;
#else
/**
 Offset distance of text from its anchor.
 
 This property is measured in ems.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 ems rightward and 0
 ems upward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textOffset;
#endif

/**
 If true, icons will display without their corresponding text when the text
 collides with other symbols and the icon does not.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing `NO`. Set this property to `nil` to reset it to
 the default value.
 
 This property is only applied to the style if `text` is non-`nil`, and
 `iconImageName` is non-`nil`. Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable, getter=isTextOptional) MGLStyleValue<NSNumber *> *textOptional;

/**
 Size of the additional area around the text bounding box used for detecting
 symbol collisions.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `2`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textPadding;

/**
 Orientation of text when map is pitched.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLTextPitchAlignmentAuto`. Set this property to
 `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textPitchAlignment;

/**
 Rotates the text clockwise.
 
 This property is measured in degrees.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-rotate"><code>text-rotate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textRotation;


@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textRotate __attribute__((unavailable("Use textRotation instead.")));

/**
 In combination with `symbolPlacement`, determines the rotation behavior of the
 individual glyphs forming the text.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLTextRotationAlignmentAuto`. Set this property
 to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textRotationAlignment;

/**
 Specifies how to capitalize text.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLTextTransformNone`. Set this property to `nil`
 to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textTransform;

#pragma mark - Accessing the Paint Attributes

#if TARGET_OS_IPHONE
/**
 The tint color to apply to the icon. The `iconImageName` property must be set
 to a template image.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *iconColor;
#else
/**
 The tint color to apply to the icon. The `iconImageName` property must be set
 to a template image.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *iconColor;
#endif

/**
 The transition affecting any changes to this layer’s `iconColor` property.

 This property corresponds to the `icon-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconColorTransition;

/**
 Fade out the halo towards the outside.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconHaloBlur;

/**
 The transition affecting any changes to this layer’s `iconHaloBlur` property.

 This property corresponds to the `icon-halo-blur-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconHaloBlurTransition;

#if TARGET_OS_IPHONE
/**
 The color of the icon’s halo. The `iconImageName` property must be set to a
 template image.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.clearColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *iconHaloColor;
#else
/**
 The color of the icon’s halo. The `iconImageName` property must be set to a
 template image.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.clearColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *iconHaloColor;
#endif

/**
 The transition affecting any changes to this layer’s `iconHaloColor` property.

 This property corresponds to the `icon-halo-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconHaloColorTransition;

/**
 Distance of halo to the icon outline.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconHaloWidth;

/**
 The transition affecting any changes to this layer’s `iconHaloWidth` property.

 This property corresponds to the `icon-halo-width-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconHaloWidthTransition;

/**
 The opacity at which the icon will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *iconOpacity;

/**
 The transition affecting any changes to this layer’s `iconOpacity` property.

 This property corresponds to the `icon-opacity-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconOpacityTransition;

#if TARGET_OS_IPHONE
/**
 Distance that the icon's anchor is moved from its original placement.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points downward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-icon-translate"><code>icon-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTranslation;
#else
/**
 Distance that the icon's anchor is moved from its original placement.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points upward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-icon-translate"><code>icon-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTranslation;
#endif

/**
 The transition affecting any changes to this layer’s `iconTranslation` property.

 This property corresponds to the `icon-translate-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconTranslationTransition;

@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTranslate __attribute__((unavailable("Use iconTranslation instead.")));

/**
 Controls the translation reference point.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLIconTranslationAnchorMap`. Set this property to
 `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `iconTranslation` is non-`nil`. Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-icon-translate-anchor"><code>icon-translate-anchor</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTranslationAnchor;

@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *iconTranslateAnchor __attribute__((unavailable("Use iconTranslationAnchor instead.")));

#if TARGET_OS_IPHONE
/**
 The color with which the text will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *textColor;
#else
/**
 The color with which the text will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *textColor;
#endif

/**
 The transition affecting any changes to this layer’s `textColor` property.

 This property corresponds to the `text-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textColorTransition;

/**
 The halo's fadeout distance towards the outside.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textHaloBlur;

/**
 The transition affecting any changes to this layer’s `textHaloBlur` property.

 This property corresponds to the `text-halo-blur-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textHaloBlurTransition;

#if TARGET_OS_IPHONE
/**
 The color of the text's halo, which helps it stand out from backgrounds.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.clearColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *textHaloColor;
#else
/**
 The color of the text's halo, which helps it stand out from backgrounds.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.clearColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *textHaloColor;
#endif

/**
 The transition affecting any changes to this layer’s `textHaloColor` property.

 This property corresponds to the `text-halo-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textHaloColorTransition;

/**
 Distance of halo to the font outline. Max text halo width is 1/4 of the
 font-size.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textHaloWidth;

/**
 The transition affecting any changes to this layer’s `textHaloWidth` property.

 This property corresponds to the `text-halo-width-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textHaloWidthTransition;

/**
 The opacity at which the text will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 * `MGLSourceStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
   * `MGLInterpolationModeIdentity`
 * `MGLCompositeStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
   * `MGLInterpolationModeCategorical`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *textOpacity;

/**
 The transition affecting any changes to this layer’s `textOpacity` property.

 This property corresponds to the `text-opacity-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textOpacityTransition;

#if TARGET_OS_IPHONE
/**
 Distance that the text's anchor is moved from its original placement.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points downward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-text-translate"><code>text-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textTranslation;
#else
/**
 Distance that the text's anchor is moved from its original placement.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points upward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-text-translate"><code>text-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textTranslation;
#endif

/**
 The transition affecting any changes to this layer’s `textTranslation` property.

 This property corresponds to the `text-translate-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textTranslationTransition;

@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textTranslate __attribute__((unavailable("Use textTranslation instead.")));

/**
 Controls the translation reference point.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLTextTranslationAnchorMap`. Set this property to
 `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`, and
 `textTranslation` is non-`nil`. Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-text-translate-anchor"><code>text-translate-anchor</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textTranslationAnchor;

@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *textTranslateAnchor __attribute__((unavailable("Use textTranslationAnchor instead.")));

@end

/**
 Methods for wrapping an enumeration value for a style layer attribute in an
 `MGLSymbolStyleLayer` object and unwrapping its raw value.
 */
@interface NSValue (MGLSymbolStyleLayerAdditions)

#pragma mark Working with Symbol Style Layer Attribute Values

/**
 Creates a new value object containing the given `MGLIconAnchor` enumeration.

 @param iconAnchor The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLIconAnchor:(MGLIconAnchor)iconAnchor;

/**
 The `MGLIconAnchor` enumeration representation of the value.
 */
@property (readonly) MGLIconAnchor MGLIconAnchorValue;

/**
 Creates a new value object containing the given `MGLIconPitchAlignment` enumeration.

 @param iconPitchAlignment The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLIconPitchAlignment:(MGLIconPitchAlignment)iconPitchAlignment;

/**
 The `MGLIconPitchAlignment` enumeration representation of the value.
 */
@property (readonly) MGLIconPitchAlignment MGLIconPitchAlignmentValue;

/**
 Creates a new value object containing the given `MGLIconRotationAlignment` enumeration.

 @param iconRotationAlignment The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLIconRotationAlignment:(MGLIconRotationAlignment)iconRotationAlignment;

/**
 The `MGLIconRotationAlignment` enumeration representation of the value.
 */
@property (readonly) MGLIconRotationAlignment MGLIconRotationAlignmentValue;

/**
 Creates a new value object containing the given `MGLIconTextFit` enumeration.

 @param iconTextFit The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLIconTextFit:(MGLIconTextFit)iconTextFit;

/**
 The `MGLIconTextFit` enumeration representation of the value.
 */
@property (readonly) MGLIconTextFit MGLIconTextFitValue;

/**
 Creates a new value object containing the given `MGLSymbolPlacement` enumeration.

 @param symbolPlacement The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLSymbolPlacement:(MGLSymbolPlacement)symbolPlacement;

/**
 The `MGLSymbolPlacement` enumeration representation of the value.
 */
@property (readonly) MGLSymbolPlacement MGLSymbolPlacementValue;

/**
 Creates a new value object containing the given `MGLTextAnchor` enumeration.

 @param textAnchor The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLTextAnchor:(MGLTextAnchor)textAnchor;

/**
 The `MGLTextAnchor` enumeration representation of the value.
 */
@property (readonly) MGLTextAnchor MGLTextAnchorValue;

/**
 Creates a new value object containing the given `MGLTextJustification` enumeration.

 @param textJustification The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLTextJustification:(MGLTextJustification)textJustification;

/**
 The `MGLTextJustification` enumeration representation of the value.
 */
@property (readonly) MGLTextJustification MGLTextJustificationValue;

/**
 Creates a new value object containing the given `MGLTextPitchAlignment` enumeration.

 @param textPitchAlignment The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLTextPitchAlignment:(MGLTextPitchAlignment)textPitchAlignment;

/**
 The `MGLTextPitchAlignment` enumeration representation of the value.
 */
@property (readonly) MGLTextPitchAlignment MGLTextPitchAlignmentValue;

/**
 Creates a new value object containing the given `MGLTextRotationAlignment` enumeration.

 @param textRotationAlignment The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLTextRotationAlignment:(MGLTextRotationAlignment)textRotationAlignment;

/**
 The `MGLTextRotationAlignment` enumeration representation of the value.
 */
@property (readonly) MGLTextRotationAlignment MGLTextRotationAlignmentValue;

/**
 Creates a new value object containing the given `MGLTextTransform` enumeration.

 @param textTransform The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLTextTransform:(MGLTextTransform)textTransform;

/**
 The `MGLTextTransform` enumeration representation of the value.
 */
@property (readonly) MGLTextTransform MGLTextTransformValue;

/**
 Creates a new value object containing the given `MGLIconTranslationAnchor` enumeration.

 @param iconTranslationAnchor The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLIconTranslationAnchor:(MGLIconTranslationAnchor)iconTranslationAnchor;

/**
 The `MGLIconTranslationAnchor` enumeration representation of the value.
 */
@property (readonly) MGLIconTranslationAnchor MGLIconTranslationAnchorValue;

/**
 Creates a new value object containing the given `MGLTextTranslationAnchor` enumeration.

 @param textTranslationAnchor The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLTextTranslationAnchor:(MGLTextTranslationAnchor)textTranslationAnchor;

/**
 The `MGLTextTranslationAnchor` enumeration representation of the value.
 */
@property (readonly) MGLTextTranslationAnchor MGLTextTranslationAnchorValue;

@end

NS_ASSUME_NONNULL_END
