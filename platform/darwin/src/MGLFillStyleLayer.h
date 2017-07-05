// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLFoundation.h"
#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Controls the translation reference point.

 Values of this type are used in the `MGLFillStyleLayer.fillTranslationAnchor`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLFillTranslationAnchor) {
    /**
     The fill is translated relative to the map.
     */
    MGLFillTranslationAnchorMap,
    /**
     The fill is translated relative to the viewport.
     */
    MGLFillTranslationAnchorViewport,
};

/**
 An `MGLFillStyleLayer` is a style layer that renders one or more filled (and
 optionally stroked) polygons on the map.
 
 Use a fill style layer to configure the visual appearance of polygon or
 multipolygon features in vector tiles loaded by an `MGLVectorSource` object or
 `MGLPolygon`, `MGLPolygonFeature`, `MGLMultiPolygon`, or
 `MGLMultiPolygonFeature` instances in an `MGLShapeSource` object.

 You can access an existing fill style layer using the
 `-[MGLStyle layerWithIdentifier:]` method if you know its identifier;
 otherwise, find it using the `MGLStyle.layers` property. You can also create a
 new fill style layer and add it to the style using a method such as
 `-[MGLStyle addLayer:]`.

 ### Example

 ```swift
 let layer = MGLFillStyleLayer(identifier: "parks", source: parks)
 layer.sourceLayerIdentifier = "parks"
 layer.fillColor = MGLStyleValue(rawValue: .green)
 layer.predicate = NSPredicate(format: "type == %@", "national-park")
 mapView.style?.addLayer(layer)
 ```
 */
MGL_EXPORT
@interface MGLFillStyleLayer : MGLVectorStyleLayer

/**
 Returns a fill style layer initialized with an identifier and source.

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

#pragma mark - Accessing the Paint Attributes

/**
 Whether or not the fill should be antialiased.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing `YES`. Set this property to `nil` to reset it to
 the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-fill-antialias"><code>fill-antialias</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable, getter=isFillAntialiased) MGLStyleValue<NSNumber *> *fillAntialiased;

@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *fillAntialias __attribute__((unavailable("Use fillAntialiased instead.")));

#if TARGET_OS_IPHONE
/**
 The color of the filled part of this layer.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `fillPattern` is set to `nil`.
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
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *fillColor;
#else
/**
 The color of the filled part of this layer.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `fillPattern` is set to `nil`.
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
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *fillColor;
#endif

/**
 The transition affecting any changes to this layer’s `fillColor` property.

 This property corresponds to the `fill-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition fillColorTransition;

/**
 The opacity of the entire fill layer. In contrast to the `fillColor`, this
 value will also affect the 1pt stroke around the fill, if the stroke is used.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1`. Set this property to `nil` to reset
 it to the default value.
 
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
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *fillOpacity;

/**
 The transition affecting any changes to this layer’s `fillOpacity` property.

 This property corresponds to the `fill-opacity-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition fillOpacityTransition;

#if TARGET_OS_IPHONE
/**
 The outline color of the fill. Matches the value of `fillColor` if unspecified.
 
 This property is only applied to the style if `fillPattern` is set to `nil`,
 and `fillAntialiased` is set to an `MGLStyleValue` object containing an
 `NSNumber` object containing `YES`. Otherwise, it is ignored.
 
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
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *fillOutlineColor;
#else
/**
 The outline color of the fill. Matches the value of `fillColor` if unspecified.
 
 This property is only applied to the style if `fillPattern` is set to `nil`,
 and `fillAntialiased` is set to an `MGLStyleValue` object containing an
 `NSNumber` object containing `YES`. Otherwise, it is ignored.
 
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
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *fillOutlineColor;
#endif

/**
 The transition affecting any changes to this layer’s `fillOutlineColor` property.

 This property corresponds to the `fill-outline-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition fillOutlineColorTransition;

/**
 Name of image in sprite to use for drawing image fills. For seamless patterns,
 image width and height must be a factor of two (2, 4, 8, ..., 512).
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSString *> *fillPattern;

/**
 The transition affecting any changes to this layer’s `fillPattern` property.

 This property corresponds to the `fill-pattern-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition fillPatternTransition;

#if TARGET_OS_IPHONE
/**
 The geometry's offset.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points downward. Set this property to `nil` to reset it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-fill-translate"><code>fill-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *fillTranslation;
#else
/**
 The geometry's offset.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points upward. Set this property to `nil` to reset it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-fill-translate"><code>fill-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *fillTranslation;
#endif

/**
 The transition affecting any changes to this layer’s `fillTranslation` property.

 This property corresponds to the `fill-translate-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition fillTranslationTransition;

@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *fillTranslate __attribute__((unavailable("Use fillTranslation instead.")));

/**
 Controls the translation reference point.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLFillTranslationAnchorMap`. Set this property to
 `nil` to reset it to the default value.
 
 This property is only applied to the style if `fillTranslation` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-fill-translate-anchor"><code>fill-translate-anchor</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *fillTranslationAnchor;

@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *fillTranslateAnchor __attribute__((unavailable("Use fillTranslationAnchor instead.")));

@end

/**
 Methods for wrapping an enumeration value for a style layer attribute in an
 `MGLFillStyleLayer` object and unwrapping its raw value.
 */
@interface NSValue (MGLFillStyleLayerAdditions)

#pragma mark Working with Fill Style Layer Attribute Values

/**
 Creates a new value object containing the given `MGLFillTranslationAnchor` enumeration.

 @param fillTranslationAnchor The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLFillTranslationAnchor:(MGLFillTranslationAnchor)fillTranslationAnchor;

/**
 The `MGLFillTranslationAnchor` enumeration representation of the value.
 */
@property (readonly) MGLFillTranslationAnchor MGLFillTranslationAnchorValue;

@end

NS_ASSUME_NONNULL_END
