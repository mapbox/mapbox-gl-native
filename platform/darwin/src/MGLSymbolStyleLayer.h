// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLFoundation.h"
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
     Automatically matches the value of
     `MGLSymbolStyleLayer.iconRotationAlignment`.
     */
    MGLIconPitchAlignmentAuto,
};

/**
 In combination with `MGLSymbolStyleLayer.symbolPlacement`, determines the
 rotation behavior of icons.

 Values of this type are used in the `MGLSymbolStyleLayer.iconRotationAlignment`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLIconRotationAlignment) {
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, aligns icons
     east-west. When `symbolPlacement` is set to `MGLSymbolPlacementLine` or
     `MGLSymbolPlacementLineCenter`, aligns icon x-axes with the line.
     */
    MGLIconRotationAlignmentMap,
    /**
     Produces icons whose x-axes are aligned with the x-axis of the viewport,
     regardless of the value of `MGLSymbolStyleLayer.symbolPlacement`.
     */
    MGLIconRotationAlignmentViewport,
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, this is
     equivalent to `MGLIconRotationAlignmentViewport`. When `symbolPlacement` is
     set to `MGLSymbolPlacementLine` or `MGLSymbolPlacementLineCenter`, this is
     equivalent to `MGLIconRotationAlignmentMap`.
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
    /**
     The label is placed at the center of the line of the geometry. Can only be
     used on `LineString` and `Polygon` geometries. Note that a single feature
     in a vector tile may contain multiple line geometries.
     */
    MGLSymbolPlacementLineCenter,
};

/**
 Controls the order in which overlapping symbols in the same layer are rendered

 Values of this type are used in the `MGLSymbolStyleLayer.symbolZOrder`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLSymbolZOrder) {
    /**
     Specify this z order if symbols’ appearance relies on lower features
     overlapping higher features. For example, symbols with a pin-like
     appearance would require this z order.
     */
    MGLSymbolZOrderViewportY,
    /**
     Specify this z order if the order in which features appear in the source is
     significant.
     */
    MGLSymbolZOrderSource,
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
     Automatically matches the value of
     `MGLSymbolStyleLayer.textRotationAlignment`.
     */
    MGLTextPitchAlignmentAuto,
};

/**
 In combination with `MGLSymbolStyleLayer.symbolPlacement`, determines the
 rotation behavior of the individual glyphs forming the text.

 Values of this type are used in the `MGLSymbolStyleLayer.textRotationAlignment`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLTextRotationAlignment) {
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, aligns text
     east-west. When `symbolPlacement` is set to `MGLSymbolPlacementLine` or
     `MGLSymbolPlacementLineCenter`, aligns text x-axes with the line.
     */
    MGLTextRotationAlignmentMap,
    /**
     Produces glyphs whose x-axes are aligned with the x-axis of the viewport,
     regardless of the value of `MGLSymbolStyleLayer.symbolPlacement`.
     */
    MGLTextRotationAlignmentViewport,
    /**
     When `symbolPlacement` is set to `MGLSymbolPlacementPoint`, this is
     equivalent to `MGLTextRotationAlignmentViewport`. When `symbolPlacement` is
     set to `MGLSymbolPlacementLine` or `MGLSymbolPlacementLineCenter`, this is
     equivalent to `MGLTextRotationAlignmentMap`.
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
 Controls the frame of reference for `MGLSymbolStyleLayer.iconTranslation`.

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
 Controls the frame of reference for `MGLSymbolStyleLayer.textTranslation`.

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
 
 Use a symbol style layer to configure the visual appearance of feature labels.
 These features can come from vector tiles loaded by an `MGLVectorTileSource`
 object, or they can be `MGLShape` or `MGLFeature` instances in an
 `MGLShapeSource` or `MGLComputedShapeSource` object.

 You can access an existing symbol style layer using the
 `-[MGLStyle layerWithIdentifier:]` method if you know its identifier;
 otherwise, find it using the `MGLStyle.layers` property. You can also create a
 new symbol style layer and add it to the style using a method such as
 `-[MGLStyle addLayer:]`.

 ### Example

 ```swift
 let layer = MGLSymbolStyleLayer(identifier: "coffeeshops", source: pois)
 layer.sourceLayerIdentifier = "pois"
 layer.iconImageName = NSExpression(forConstantValue: "coffee")
 layer.iconScale = NSExpression(forConstantValue: 0.5)
 layer.text = NSExpression(forKeyPath: "name")
 layer.textTranslation = NSExpression(forConstantValue: NSValue(cgVector: CGVector(dx: 10, dy: 0)))
 layer.textJustification = NSExpression(forConstantValue: "left")
 layer.textAnchor = NSExpression(forConstantValue: "left")
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
 
 The default value of this property is an expression that evaluates to `NO`. Set
 this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-allow-overlap"><code>icon-allow-overlap</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant Boolean values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconAllowsOverlap;


@property (nonatomic, null_resettable) NSExpression *iconAllowOverlap __attribute__((unavailable("Use iconAllowsOverlap instead.")));

/**
 Part of the icon placed closest to the anchor.
 
 The default value of this property is an expression that evaluates to `center`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLIconAnchor` values
 * Any of the following constant string values:
   * `center`: The center of the icon is placed closest to the anchor.
   * `left`: The left side of the icon is placed closest to the anchor.
   * `right`: The right side of the icon is placed closest to the anchor.
   * `top`: The top of the icon is placed closest to the anchor.
   * `bottom`: The bottom of the icon is placed closest to the anchor.
   * `top-left`: The top left corner of the icon is placed closest to the
 anchor.
   * `top-right`: The top right corner of the icon is placed closest to the
 anchor.
   * `bottom-left`: The bottom left corner of the icon is placed closest to the
 anchor.
   * `bottom-right`: The bottom right corner of the icon is placed closest to
 the anchor.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconAnchor;

/**
 If true, other symbols can be visible even if they collide with the icon.
 
 The default value of this property is an expression that evaluates to `NO`. Set
 this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-ignore-placement"><code>icon-ignore-placement</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant Boolean values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconIgnoresPlacement;


@property (nonatomic, null_resettable) NSExpression *iconIgnorePlacement __attribute__((unavailable("Use iconIgnoresPlacement instead.")));

/**
 Name of a style image to use for drawing an image background.
 
 Use the `+[MGLStyle setImage:forName:]` method to associate an image with a
 name that you can set this property to.
 
 Within a constant string value, a feature attribute name enclosed in curly
 braces (e.g., `{token}`) is replaced with the value of the named attribute.
 Tokens inside non-constant expressions are ignored; instead, use `mgl_join:`
 and key path expressions.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-image"><code>icon-image</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant string values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconImageName;


@property (nonatomic, null_resettable) NSExpression *iconImage __attribute__((unavailable("Use iconImageName instead.")));

#if TARGET_OS_IPHONE
/**
 Offset distance of icon from its anchor.
 
 The default value of this property is an expression that evaluates to an
 `NSValue` object containing a `CGVector` struct set to 0 rightward and 0
 downward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `CGVector` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconOffset;
#else
/**
 Offset distance of icon from its anchor.
 
 The default value of this property is an expression that evaluates to an
 `NSValue` object containing a `CGVector` struct set to 0 rightward and 0
 upward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `CGVector` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconOffset;
#endif

/**
 If true, text will display without their corresponding icons when the icon
 collides with other symbols and the text does not.
 
 The default value of this property is an expression that evaluates to `NO`. Set
 this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `text` is non-`nil`. Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant Boolean values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable, getter=isIconOptional) NSExpression *iconOptional;

/**
 Size of the additional area around the icon bounding box used for detecting
 symbol collisions.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to the float
 `2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values no less than 0
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconPadding;

/**
 Orientation of icon when map is pitched.
 
 The default value of this property is an expression that evaluates to `auto`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLIconPitchAlignment` values
 * Any of the following constant string values:
   * `map`: The icon is aligned to the plane of the map.
   * `viewport`: The icon is aligned to the plane of the viewport.
   * `auto`: Automatically matches the value of `icon-rotation-alignment`.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconPitchAlignment;

/**
 Rotates the icon clockwise.
 
 This property is measured in degrees.
 
 The default value of this property is an expression that evaluates to the float
 `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-rotate"><code>icon-rotate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconRotation;


@property (nonatomic, null_resettable) NSExpression *iconRotate __attribute__((unavailable("Use iconRotation instead.")));

/**
 In combination with `symbolPlacement`, determines the rotation behavior of
 icons.
 
 The default value of this property is an expression that evaluates to `auto`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLIconRotationAlignment` values
 * Any of the following constant string values:
   * `map`: When `symbol-placement` is set to `point`, aligns icons east-west.
 When `symbol-placement` is set to `line` or `line-center`, aligns icon x-axes
 with the line.
   * `viewport`: Produces icons whose x-axes are aligned with the x-axis of the
 viewport, regardless of the value of `symbol-placement`.
   * `auto`: When `symbol-placement` is set to `point`, this is equivalent to
 `viewport`. When `symbol-placement` is set to `line` or `line-center`, this is
 equivalent to `map`.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconRotationAlignment;

/**
 Scales the original size of the icon by the provided factor. The new point size
 of the image will be the original point size multiplied by `iconScale`. 1 is
 the original size; 3 triples the size of the image.
 
 This property is measured in factor of the original icon sizes.
 
 The default value of this property is an expression that evaluates to the float
 `1`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-size"><code>icon-size</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values no less than 0
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconScale;


@property (nonatomic, null_resettable) NSExpression *iconSize __attribute__((unavailable("Use iconScale instead.")));

/**
 Scales the icon to fit around the associated text.
 
 The default value of this property is an expression that evaluates to `none`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `text` is non-`nil`. Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLIconTextFit` values
 * Any of the following constant string values:
   * `none`: The icon is displayed at its intrinsic aspect ratio.
   * `width`: The icon is scaled in the x-dimension to fit the width of the
 text.
   * `height`: The icon is scaled in the y-dimension to fit the height of the
 text.
   * `both`: The icon is scaled in both x- and y-dimensions.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconTextFit;

#if TARGET_OS_IPHONE
/**
 Size of the additional area added to dimensions determined by `iconTextFit`.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to an
 `NSValue` object containing `UIEdgeInsetsZero`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `text` is non-`nil`, and `iconTextFit` is set to an expression that evaluates
 to `MGLIconTextFitBoth`, `MGLIconTextFitWidth`, or `MGLIconTextFitHeight`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `UIEdgeInsets` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconTextFitPadding;
#else
/**
 Size of the additional area added to dimensions determined by `iconTextFit`.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to an
 `NSValue` object containing `NSEdgeInsetsZero`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `text` is non-`nil`, and `iconTextFit` is set to an expression that evaluates
 to `MGLIconTextFitBoth`, `MGLIconTextFitWidth`, or `MGLIconTextFitHeight`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `NSEdgeInsets` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconTextFitPadding;
#endif

/**
 If true, the icon may be flipped to prevent it from being rendered upside-down.
 
 The default value of this property is an expression that evaluates to `NO`. Set
 this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `iconRotationAlignment` is set to an expression that evaluates to `map`, and
 `symbolPlacement` is set to an expression that evaluates to either
 `MGLSymbolPlacementLine` or `MGLSymbolPlacementLineCenter`. Otherwise, it is
 ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-icon-keep-upright"><code>icon-keep-upright</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant Boolean values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *keepsIconUpright;


@property (nonatomic, null_resettable) NSExpression *iconKeepUpright __attribute__((unavailable("Use keepsIconUpright instead.")));

/**
 If true, the text may be flipped vertically to prevent it from being rendered
 upside-down.
 
 The default value of this property is an expression that evaluates to `YES`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`, and
 `textRotationAlignment` is set to an expression that evaluates to `map`, and
 `symbolPlacement` is set to an expression that evaluates to either
 `MGLSymbolPlacementLine` or `MGLSymbolPlacementLineCenter`. Otherwise, it is
 ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-keep-upright"><code>text-keep-upright</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant Boolean values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *keepsTextUpright;


@property (nonatomic, null_resettable) NSExpression *textKeepUpright __attribute__((unavailable("Use keepsTextUpright instead.")));

/**
 Maximum angle change between adjacent characters.
 
 This property is measured in degrees.
 
 The default value of this property is an expression that evaluates to the float
 `45`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`, and
 `symbolPlacement` is set to an expression that evaluates to either
 `MGLSymbolPlacementLine` or `MGLSymbolPlacementLineCenter`. Otherwise, it is
 ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-max-angle"><code>text-max-angle</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *maximumTextAngle;


@property (nonatomic, null_resettable) NSExpression *textMaxAngle __attribute__((unavailable("Use maximumTextAngle instead.")));

/**
 The maximum line width for text wrapping.
 
 This property is measured in ems.
 
 The default value of this property is an expression that evaluates to the float
 `10`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-max-width"><code>text-max-width</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values no less than 0
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *maximumTextWidth;


@property (nonatomic, null_resettable) NSExpression *textMaxWidth __attribute__((unavailable("Use maximumTextWidth instead.")));

/**
 If true, the symbols will not cross tile edges to avoid mutual collisions.
 Recommended in layers that don't have enough padding in the vector tile to
 prevent collisions, or if it is a point symbol layer placed after a line symbol
 layer.
 
 The default value of this property is an expression that evaluates to `NO`. Set
 this property to `nil` to reset it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-symbol-avoid-edges"><code>symbol-avoid-edges</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant Boolean values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *symbolAvoidsEdges;


@property (nonatomic, null_resettable) NSExpression *symbolAvoidEdges __attribute__((unavailable("Use symbolAvoidsEdges instead.")));

/**
 Label placement relative to its geometry.
 
 The default value of this property is an expression that evaluates to `point`.
 Set this property to `nil` to reset it to the default value.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLSymbolPlacement` values
 * Any of the following constant string values:
   * `point`: The label is placed at the point where the geometry is located.
   * `line`: The label is placed along the line of the geometry. Can only be
 used on `LineString` and `Polygon` geometries.
   * `line-center`: The label is placed at the center of the line of the
 geometry. Can only be used on `LineString` and `Polygon` geometries. Note that
 a single feature in a vector tile may contain multiple line geometries.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *symbolPlacement;

/**
 Distance between two symbol anchors.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to the float
 `250`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `symbolPlacement` is set to an
 expression that evaluates to `line`. Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values no less than 1
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *symbolSpacing;

/**
 Controls the order in which overlapping symbols in the same layer are rendered
 
 The default value of this property is an expression that evaluates to
 `viewport-y`. Set this property to `nil` to reset it to the default value.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLSymbolZOrder` values
 * Any of the following constant string values:
   * `viewport-y`: Specify this z order if symbols’ appearance relies on lower
 features overlapping higher features. For example, symbols with a pin-like
 appearance would require this z order.
   * `source`: Specify this z order if the order in which features appear in the
 source is significant.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *symbolZOrder;

/**
 Value to use for a text label.
 
 Within a constant string value, a feature attribute name enclosed in curly
 braces (e.g., `{token}`) is replaced with the value of the named attribute.
 Tokens inside non-constant expressions are ignored; instead, use `mgl_join:`
 and key path expressions.
 
 The default value of this property is an expression that evaluates to the empty
 string. Set this property to `nil` to reset it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-field"><code>text-field</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant string values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *text;


@property (nonatomic, null_resettable) NSExpression *textField __attribute__((unavailable("Use text instead.")));

/**
 If true, the text will be visible even if it collides with other previously
 drawn symbols.
 
 The default value of this property is an expression that evaluates to `NO`. Set
 this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-allow-overlap"><code>text-allow-overlap</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant Boolean values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *textAllowsOverlap;


@property (nonatomic, null_resettable) NSExpression *textAllowOverlap __attribute__((unavailable("Use textAllowsOverlap instead.")));

/**
 Part of the text placed closest to the anchor.
 
 The default value of this property is an expression that evaluates to `center`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLTextAnchor` values
 * Any of the following constant string values:
   * `center`: The center of the text is placed closest to the anchor.
   * `left`: The left side of the text is placed closest to the anchor.
   * `right`: The right side of the text is placed closest to the anchor.
   * `top`: The top of the text is placed closest to the anchor.
   * `bottom`: The bottom of the text is placed closest to the anchor.
   * `top-left`: The top left corner of the text is placed closest to the
 anchor.
   * `top-right`: The top right corner of the text is placed closest to the
 anchor.
   * `bottom-left`: The bottom left corner of the text is placed closest to the
 anchor.
   * `bottom-right`: The bottom right corner of the text is placed closest to
 the anchor.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textAnchor;

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
 
 The default value of this property is an expression that evaluates to the array
 `Open Sans Regular`, `Arial Unicode MS Regular`. Set this property to `nil` to
 reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-font"><code>text-font</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant array values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textFontNames;


@property (nonatomic, null_resettable) NSExpression *textFont __attribute__((unavailable("Use textFontNames instead.")));

/**
 Font size.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to the float
 `16`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-size"><code>text-size</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values no less than 0
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textFontSize;


@property (nonatomic, null_resettable) NSExpression *textSize __attribute__((unavailable("Use textFontSize instead.")));

/**
 If true, other symbols can be visible even if they collide with the text.
 
 The default value of this property is an expression that evaluates to `NO`. Set
 this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-ignore-placement"><code>text-ignore-placement</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant Boolean values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *textIgnoresPlacement;


@property (nonatomic, null_resettable) NSExpression *textIgnorePlacement __attribute__((unavailable("Use textIgnoresPlacement instead.")));

/**
 Text justification options.
 
 The default value of this property is an expression that evaluates to `center`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-justify"><code>text-justify</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLTextJustification` values
 * Any of the following constant string values:
   * `left`: The text is aligned to the left.
   * `center`: The text is centered.
   * `right`: The text is aligned to the right.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textJustification;


@property (nonatomic, null_resettable) NSExpression *textJustify __attribute__((unavailable("Use textJustification instead.")));

/**
 Text tracking amount.
 
 This property is measured in ems.
 
 The default value of this property is an expression that evaluates to the float
 `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textLetterSpacing;

/**
 Text leading value for multi-line text.
 
 This property is measured in ems.
 
 The default value of this property is an expression that evaluates to the float
 `1.2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *textLineHeight;

#if TARGET_OS_IPHONE
/**
 Offset distance of text from its anchor.
 
 This property is measured in ems.
 
 The default value of this property is an expression that evaluates to an
 `NSValue` object containing a `CGVector` struct set to 0 ems rightward and 0
 ems downward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `CGVector` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textOffset;
#else
/**
 Offset distance of text from its anchor.
 
 This property is measured in ems.
 
 The default value of this property is an expression that evaluates to an
 `NSValue` object containing a `CGVector` struct set to 0 ems rightward and 0
 ems upward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `CGVector` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textOffset;
#endif

/**
 If true, icons will display without their corresponding text when the text
 collides with other symbols and the icon does not.
 
 The default value of this property is an expression that evaluates to `NO`. Set
 this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`, and
 `iconImageName` is non-`nil`. Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant Boolean values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable, getter=isTextOptional) NSExpression *textOptional;

/**
 Size of the additional area around the text bounding box used for detecting
 symbol collisions.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to the float
 `2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values no less than 0
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *textPadding;

/**
 Orientation of text when map is pitched.
 
 The default value of this property is an expression that evaluates to `auto`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLTextPitchAlignment` values
 * Any of the following constant string values:
   * `map`: The text is aligned to the plane of the map.
   * `viewport`: The text is aligned to the plane of the viewport.
   * `auto`: Automatically matches the value of `text-rotation-alignment`.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *textPitchAlignment;

/**
 Rotates the text clockwise.
 
 This property is measured in degrees.
 
 The default value of this property is an expression that evaluates to the float
 `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-symbol-text-rotate"><code>text-rotate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textRotation;


@property (nonatomic, null_resettable) NSExpression *textRotate __attribute__((unavailable("Use textRotation instead.")));

/**
 In combination with `symbolPlacement`, determines the rotation behavior of the
 individual glyphs forming the text.
 
 The default value of this property is an expression that evaluates to `auto`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLTextRotationAlignment` values
 * Any of the following constant string values:
   * `map`: When `symbol-placement` is set to `point`, aligns text east-west.
 When `symbol-placement` is set to `line` or `line-center`, aligns text x-axes
 with the line.
   * `viewport`: Produces glyphs whose x-axes are aligned with the x-axis of the
 viewport, regardless of the value of `symbol-placement`.
   * `auto`: When `symbol-placement` is set to `point`, this is equivalent to
 `viewport`. When `symbol-placement` is set to `line` or `line-center`, this is
 equivalent to `map`.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *textRotationAlignment;

/**
 Specifies how to capitalize text.
 
 The default value of this property is an expression that evaluates to `none`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLTextTransform` values
 * Any of the following constant string values:
   * `none`: The text is not altered.
   * `uppercase`: Forces all letters to be displayed in uppercase.
   * `lowercase`: Forces all letters to be displayed in lowercase.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textTransform;

#pragma mark - Accessing the Paint Attributes

#if TARGET_OS_IPHONE
/**
 The tint color to apply to the icon. The `iconImageName` property must be set
 to a template image.
 
 The default value of this property is an expression that evaluates to
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `UIColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconColor;
#else
/**
 The tint color to apply to the icon. The `iconImageName` property must be set
 to a template image.
 
 The default value of this property is an expression that evaluates to
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `NSColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconColor;
#endif

/**
 The transition affecting any changes to this layer’s `iconColor` property.

 This property corresponds to the `icon-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconColorTransition;

/**
 Fade out the halo towards the outside.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to the float
 `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values no less than 0
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconHaloBlur;

/**
 The transition affecting any changes to this layer’s `iconHaloBlur` property.

 This property corresponds to the `icon-halo-blur-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconHaloBlurTransition;

#if TARGET_OS_IPHONE
/**
 The color of the icon’s halo. The `iconImageName` property must be set to a
 template image.
 
 The default value of this property is an expression that evaluates to
 `UIColor.clearColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `UIColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconHaloColor;
#else
/**
 The color of the icon’s halo. The `iconImageName` property must be set to a
 template image.
 
 The default value of this property is an expression that evaluates to
 `NSColor.clearColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `NSColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconHaloColor;
#endif

/**
 The transition affecting any changes to this layer’s `iconHaloColor` property.

 This property corresponds to the `icon-halo-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconHaloColorTransition;

/**
 Distance of halo to the icon outline.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to the float
 `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values no less than 0
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconHaloWidth;

/**
 The transition affecting any changes to this layer’s `iconHaloWidth` property.

 This property corresponds to the `icon-halo-width-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconHaloWidthTransition;

/**
 The opacity at which the icon will be drawn.
 
 The default value of this property is an expression that evaluates to the float
 `1`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values between 0 and 1 inclusive
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *iconOpacity;

/**
 The transition affecting any changes to this layer’s `iconOpacity` property.

 This property corresponds to the `icon-opacity-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconOpacityTransition;

#if TARGET_OS_IPHONE
/**
 Distance that the icon's anchor is moved from its original placement.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points downward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-icon-translate"><code>icon-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `CGVector` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconTranslation;
#else
/**
 Distance that the icon's anchor is moved from its original placement.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points upward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-icon-translate"><code>icon-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `CGVector` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconTranslation;
#endif

/**
 The transition affecting any changes to this layer’s `iconTranslation` property.

 This property corresponds to the `icon-translate-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition iconTranslationTransition;

@property (nonatomic, null_resettable) NSExpression *iconTranslate __attribute__((unavailable("Use iconTranslation instead.")));

/**
 Controls the frame of reference for `iconTranslation`.
 
 The default value of this property is an expression that evaluates to `map`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `iconImageName` is non-`nil`, and
 `iconTranslation` is non-`nil`. Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-icon-translate-anchor"><code>icon-translate-anchor</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLIconTranslationAnchor` values
 * Any of the following constant string values:
   * `map`: Icons are translated relative to the map.
   * `viewport`: Icons are translated relative to the viewport.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *iconTranslationAnchor;

@property (nonatomic, null_resettable) NSExpression *iconTranslateAnchor __attribute__((unavailable("Use iconTranslationAnchor instead.")));

#if TARGET_OS_IPHONE
/**
 The color with which the text will be drawn.
 
 The default value of this property is an expression that evaluates to
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `UIColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textColor;
#else
/**
 The color with which the text will be drawn.
 
 The default value of this property is an expression that evaluates to
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `NSColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textColor;
#endif

/**
 The transition affecting any changes to this layer’s `textColor` property.

 This property corresponds to the `text-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textColorTransition;

/**
 The halo's fadeout distance towards the outside.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to the float
 `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values no less than 0
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textHaloBlur;

/**
 The transition affecting any changes to this layer’s `textHaloBlur` property.

 This property corresponds to the `text-halo-blur-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textHaloBlurTransition;

#if TARGET_OS_IPHONE
/**
 The color of the text's halo, which helps it stand out from backgrounds.
 
 The default value of this property is an expression that evaluates to
 `UIColor.clearColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `UIColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textHaloColor;
#else
/**
 The color of the text's halo, which helps it stand out from backgrounds.
 
 The default value of this property is an expression that evaluates to
 `NSColor.clearColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `NSColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textHaloColor;
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
 
 The default value of this property is an expression that evaluates to the float
 `0`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values no less than 0
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textHaloWidth;

/**
 The transition affecting any changes to this layer’s `textHaloWidth` property.

 This property corresponds to the `text-halo-width-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textHaloWidthTransition;

/**
 The opacity at which the text will be drawn.
 
 The default value of this property is an expression that evaluates to the float
 `1`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values between 0 and 1 inclusive
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable and/or
 feature attributes
 */
@property (nonatomic, null_resettable) NSExpression *textOpacity;

/**
 The transition affecting any changes to this layer’s `textOpacity` property.

 This property corresponds to the `text-opacity-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textOpacityTransition;

#if TARGET_OS_IPHONE
/**
 Distance that the text's anchor is moved from its original placement.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points downward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-text-translate"><code>text-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `CGVector` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *textTranslation;
#else
/**
 Distance that the text's anchor is moved from its original placement.
 
 This property is measured in points.
 
 The default value of this property is an expression that evaluates to an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points upward. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`. Otherwise,
 it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-text-translate"><code>text-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `CGVector` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *textTranslation;
#endif

/**
 The transition affecting any changes to this layer’s `textTranslation` property.

 This property corresponds to the `text-translate-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition textTranslationTransition;

@property (nonatomic, null_resettable) NSExpression *textTranslate __attribute__((unavailable("Use textTranslation instead.")));

/**
 Controls the frame of reference for `textTranslation`.
 
 The default value of this property is an expression that evaluates to `map`.
 Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `text` is non-`nil`, and
 `textTranslation` is non-`nil`. Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-text-translate-anchor"><code>text-translate-anchor</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLTextTranslationAnchor` values
 * Any of the following constant string values:
   * `map`: The text is translated relative to the map.
   * `viewport`: The text is translated relative to the viewport.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *textTranslationAnchor;

@property (nonatomic, null_resettable) NSExpression *textTranslateAnchor __attribute__((unavailable("Use textTranslationAnchor instead.")));

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
 Creates a new value object containing the given `MGLSymbolZOrder` enumeration.

 @param symbolZOrder The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLSymbolZOrder:(MGLSymbolZOrder)symbolZOrder;

/**
 The `MGLSymbolZOrder` enumeration representation of the value.
 */
@property (readonly) MGLSymbolZOrder MGLSymbolZOrderValue;

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
