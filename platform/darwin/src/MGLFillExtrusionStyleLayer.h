// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLFoundation.h"
#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Controls the translation reference point.

 Values of this type are used in the `MGLFillExtrusionStyleLayer.fillExtrusionTranslateAnchor`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLFillExtrusionTranslateAnchor) {
    /**
     The fill extrusion is translated relative to the map.
     */
    MGLFillExtrusionTranslateAnchorMap,
    /**
     The fill extrusion is translated relative to the viewport.
     */
    MGLFillExtrusionTranslateAnchorViewport,
};

/**
 An extruded (3D) polygon.

 You can access an existing fill-extrusion style layer using the
 `-[MGLStyle layerWithIdentifier:]` method if you know its identifier;
 otherwise, find it using the `MGLStyle.layers` property. You can also create a
 new fill-extrusion style layer and add it to the style using a method such as
 `-[MGLStyle addLayer:]`.

 ### Example

 ```swift
 ```
 */
MGL_EXPORT
@interface MGLFillExtrusionStyleLayer : MGLVectorStyleLayer

#pragma mark - Accessing the Paint Attributes

/**
 The height with which to extrude the base of this layer. Must be less than or
 equal to `fillExtrusionHeight`.
 
 This property is measured in meters.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 This property is only applied to the style if `fillExtrusionHeight` is
 non-`nil`. Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLStyleConstantValue`
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
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *fillExtrusionBase;

#if TARGET_OS_IPHONE
/**
 The base color of the extruded fill. The extrusion's surfaces will be shaded
 differently based on this color in combination with the root `light` settings.
 If this color is specified as `rgba` with an alpha component, the alpha
 component will be ignored; use `fillExtrusionOpacity` to set layer opacity.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `fillExtrusionPattern` is set to
 `nil`. Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLStyleConstantValue`
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
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *fillExtrusionColor;
#else
/**
 The base color of the extruded fill. The extrusion's surfaces will be shaded
 differently based on this color in combination with the root `light` settings.
 If this color is specified as `rgba` with an alpha component, the alpha
 component will be ignored; use `fillExtrusionOpacity` to set layer opacity.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 This property is only applied to the style if `fillExtrusionPattern` is set to
 `nil`. Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLStyleConstantValue`
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
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *fillExtrusionColor;
#endif

/**
 The height with which to extrude this layer.
 
 This property is measured in meters.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLStyleConstantValue`
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
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *fillExtrusionHeight;

/**
 The opacity of the entire fill extrusion layer. This is rendered on a
 per-layer, not per-feature, basis, and data-driven styling is not available.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1`. Set this property to `nil` to reset
 it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLStyleConstantValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *fillExtrusionOpacity;

/**
 Name of image in sprite to use for drawing images on extruded fills. For
 seamless patterns, image width and height must be a factor of two (2, 4, 8,
 ..., 512).
 
 You can set this property to an instance of:
 
 * `MGLStyleConstantValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSString *> *fillExtrusionPattern;

#if TARGET_OS_IPHONE
/**
 The geometry's offset. Values are [x, y] where negatives indicate left and up
 (on the flat plane), respectively.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points downward. Set this property to `nil` to reset it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLStyleConstantValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *fillExtrusionTranslate;
#else
/**
 The geometry's offset. Values are [x, y] where negatives indicate left and up
 (on the flat plane), respectively.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing a `CGVector` struct set to 0 points rightward and 0
 points upward. Set this property to `nil` to reset it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLStyleConstantValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *fillExtrusionTranslate;
#endif

/**
 Controls the translation reference point.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLFillExtrusionTranslateAnchorMap`. Set this
 property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `fillExtrusionTranslate` is
 non-`nil`. Otherwise, it is ignored.
 
 You can set this property to an instance of:
 
 * `MGLStyleConstantValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *fillExtrusionTranslateAnchor;

@end

/**
 Methods for wrapping an enumeration value for a style layer attribute in an
 `MGLFillExtrusionStyleLayer` object and unwrapping its raw value.
 */
@interface NSValue (MGLFillExtrusionStyleLayerAdditions)

#pragma mark Working with FillExtrusion Style Layer Attribute Values

/**
 Creates a new value object containing the given `MGLFillExtrusionTranslateAnchor` enumeration.

 @param fillExtrusionTranslateAnchor The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLFillExtrusionTranslateAnchor:(MGLFillExtrusionTranslateAnchor)fillExtrusionTranslateAnchor;

/**
 The `MGLFillExtrusionTranslateAnchor` enumeration representation of the value.
 */
@property (readonly) MGLFillExtrusionTranslateAnchor MGLFillExtrusionTranslateAnchorValue;

@end

NS_ASSUME_NONNULL_END
