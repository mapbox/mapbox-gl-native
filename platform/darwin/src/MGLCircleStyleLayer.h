// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLFoundation.h"
#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Orientation of circle when map is pitched.

 Values of this type are used in the `MGLCircleStyleLayer.circlePitchAlignment`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLCirclePitchAlignment) {
    /**
     The circle is aligned to the plane of the map.
     */
    MGLCirclePitchAlignmentMap,
    /**
     The circle is aligned to the plane of the viewport.
     */
    MGLCirclePitchAlignmentViewport,
};

/**
 Controls the scaling behavior of the circle when the map is pitched.

 Values of this type are used in the `MGLCircleStyleLayer.circleScaleAlignment`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLCircleScaleAlignment) {
    /**
     Circles are scaled according to their apparent distance to the camera.
     */
    MGLCircleScaleAlignmentMap,
    /**
     Circles are not scaled.
     */
    MGLCircleScaleAlignmentViewport,
};

/**
 Controls the translation reference point.

 Values of this type are used in the `MGLCircleStyleLayer.circleTranslationAnchor`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLCircleTranslationAnchor) {
    /**
     The circle is translated relative to the map.
     */
    MGLCircleTranslationAnchorMap,
    /**
     The circle is translated relative to the viewport.
     */
    MGLCircleTranslationAnchorViewport,
};

/**
 An `MGLCircleStyleLayer` is a style layer that renders one or more filled
 circles on the map.
 
 Use a circle style layer to configure the visual appearance of point or point
 collection features in vector tiles loaded by an `MGLVectorSource` object or
 `MGLPointAnnotation`, `MGLPointFeature`, `MGLPointCollection`, or
 `MGLPointCollectionFeature` instances in an `MGLShapeSource` object.
 
 A circle style layer renders circles whose radii are measured in screen units.
 To display circles on the map whose radii correspond to real-world distances,
 use many-sided regular polygons and configure their appearance using an
 `MGLFillStyleLayer` object.

 You can access an existing circle style layer using the
 `-[MGLStyle layerWithIdentifier:]` method if you know its identifier;
 otherwise, find it using the `MGLStyle.layers` property. You can also create a
 new circle style layer and add it to the style using a method such as
 `-[MGLStyle addLayer:]`.

 ### Example

 ```swift
 let layer = MGLCircleStyleLayer(identifier: "circles", source: population)
 layer.sourceLayerIdentifier = "population"
 layer.circleColor = MGLStyleValue(rawValue: .green)
 layer.circleRadius = MGLStyleValue(interpolationMode: .exponential,
                                    cameraStops: [12: MGLStyleValue(rawValue: 2),
                                                  22: MGLStyleValue(rawValue: 180)],
                                    options: [.interpolationBase: 1.75])
 layer.circleOpacity = MGLStyleValue(rawValue: 0.7)
 layer.predicate = NSPredicate(format: "%K == %@", "marital-status", "married")
 mapView.style?.addLayer(layer)
 ```
 */
MGL_EXPORT
@interface MGLCircleStyleLayer : MGLVectorStyleLayer

/**
 Returns a circle style layer initialized with an identifier and source.

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
 Amount to blur the circle. 1 blurs the circle such that only the centerpoint is
 full opacity.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
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
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleBlur;

/**
 The transition affecting any changes to this layer’s `circleBlur` property.

 This property corresponds to the `circle-blur-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition circleBlurTransition;

#if TARGET_OS_IPHONE
/**
 The fill color of the circle.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
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
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *circleColor;
#else
/**
 The fill color of the circle.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
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
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *circleColor;
#endif

/**
 The transition affecting any changes to this layer’s `circleColor` property.

 This property corresponds to the `circle-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition circleColorTransition;

/**
 The opacity at which the circle will be drawn.
 
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
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleOpacity;

/**
 The transition affecting any changes to this layer’s `circleOpacity` property.

 This property corresponds to the `circle-opacity-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition circleOpacityTransition;

/**
 Orientation of circle when map is pitched.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLCirclePitchAlignmentViewport`. Set this
 property to `nil` to reset it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circlePitchAlignment;

/**
 Circle radius.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `5`. Set this property to `nil` to reset
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
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleRadius;

/**
 The transition affecting any changes to this layer’s `circleRadius` property.

 This property corresponds to the `circle-radius-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition circleRadiusTransition;

/**
 Controls the scaling behavior of the circle when the map is pitched.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLCircleScaleAlignmentMap`. Set this property to
 `nil` to reset it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-circle-pitch-scale"><code>circle-pitch-scale</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circleScaleAlignment;

@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circlePitchScale __attribute__((unavailable("Use circleScaleAlignment instead.")));

#if TARGET_OS_IPHONE
/**
 The stroke color of the circle.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
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
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *circleStrokeColor;
#else
/**
 The stroke color of the circle.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
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
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *circleStrokeColor;
#endif

/**
 The transition affecting any changes to this layer’s `circleStrokeColor` property.

 This property corresponds to the `circle-stroke-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition circleStrokeColorTransition;

/**
 The opacity of the circle's stroke.
 
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
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleStrokeOpacity;

/**
 The transition affecting any changes to this layer’s `circleStrokeOpacity` property.

 This property corresponds to the `circle-stroke-opacity-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition circleStrokeOpacityTransition;

/**
 The width of the circle's stroke. Strokes are placed outside of the
 `circleRadius`.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
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
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleStrokeWidth;

/**
 The transition affecting any changes to this layer’s `circleStrokeWidth` property.

 This property corresponds to the `circle-stroke-width-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition circleStrokeWidthTransition;

#if TARGET_OS_IPHONE
/**
 The geometry's offset.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points downward. Set this property to `nil` to reset it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-circle-translate"><code>circle-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circleTranslation;
#else
/**
 The geometry's offset.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points upward. Set this property to `nil` to reset it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-circle-translate"><code>circle-translate</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circleTranslation;
#endif

/**
 The transition affecting any changes to this layer’s `circleTranslation` property.

 This property corresponds to the `circle-translate-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition circleTranslationTransition;

@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circleTranslate __attribute__((unavailable("Use circleTranslation instead.")));

/**
 Controls the translation reference point.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLCircleTranslationAnchorMap`. Set this property
 to `nil` to reset it to the default value.
 
 This property is only applied to the style if `circleTranslation` is non-`nil`.
 Otherwise, it is ignored.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-circle-translate-anchor"><code>circle-translate-anchor</code></a>
 layout property in the Mapbox Style Specification.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circleTranslationAnchor;

@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circleTranslateAnchor __attribute__((unavailable("Use circleTranslationAnchor instead.")));

@end

/**
 Methods for wrapping an enumeration value for a style layer attribute in an
 `MGLCircleStyleLayer` object and unwrapping its raw value.
 */
@interface NSValue (MGLCircleStyleLayerAdditions)

#pragma mark Working with Circle Style Layer Attribute Values

/**
 Creates a new value object containing the given `MGLCirclePitchAlignment` enumeration.

 @param circlePitchAlignment The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLCirclePitchAlignment:(MGLCirclePitchAlignment)circlePitchAlignment;

/**
 The `MGLCirclePitchAlignment` enumeration representation of the value.
 */
@property (readonly) MGLCirclePitchAlignment MGLCirclePitchAlignmentValue;

/**
 Creates a new value object containing the given `MGLCircleScaleAlignment` enumeration.

 @param circleScaleAlignment The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLCircleScaleAlignment:(MGLCircleScaleAlignment)circleScaleAlignment;

/**
 The `MGLCircleScaleAlignment` enumeration representation of the value.
 */
@property (readonly) MGLCircleScaleAlignment MGLCircleScaleAlignmentValue;

/**
 Creates a new value object containing the given `MGLCircleTranslationAnchor` enumeration.

 @param circleTranslationAnchor The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLCircleTranslationAnchor:(MGLCircleTranslationAnchor)circleTranslationAnchor;

/**
 The `MGLCircleTranslationAnchor` enumeration representation of the value.
 */
@property (readonly) MGLCircleTranslationAnchor MGLCircleTranslationAnchorValue;

@end

NS_ASSUME_NONNULL_END
