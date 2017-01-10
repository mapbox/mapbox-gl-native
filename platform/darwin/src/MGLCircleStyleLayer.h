// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

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
 layer.circleRadius = MGLStyleValue(interpolationBase: 1.75, stops: [
     12: MGLStyleValue(rawValue: 2),
     22: MGLStyleValue(rawValue: 180)
 ])
 layer.circleOpacity = MGLStyleValue(rawValue: 0.7)
 layer.predicate = NSPredicate(format: "%K == %@", "marital-status", "married")
 mapView.style?.addLayer(layer)
 ```
 */
@interface MGLCircleStyleLayer : MGLVectorStyleLayer

#pragma mark - Accessing the Paint Attributes

/**
 Amount to blur the circle. 1 blurs the circle such that only the centerpoint is
 full opacity.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleBlur;

#if TARGET_OS_IPHONE
/**
 The fill color of the circle.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *circleColor;
#else
/**
 The fill color of the circle.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *circleColor;
#endif

/**
 The opacity at which the circle will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1`. Set this property to `nil` to reset
 it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleOpacity;

/**
 Circle radius.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `5`. Set this property to `nil` to reset
 it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleRadius;

/**
 Controls the scaling behavior of the circle when the map is pitched.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLCircleScaleAlignmentMap`. Set this property to
 `nil` to reset it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-circle-pitch-scale"><code>circle-pitch-scale</code></a>
 layout property in the Mapbox Style Specification.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circleScaleAlignment;

@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circlePitchScale __attribute__((unavailable("Use circleScaleAlignment instead.")));

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
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circleTranslation;
#endif

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
