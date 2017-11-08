// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLFoundation.h"
#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Direction of light source when map is rotated.

 Values of this type are used in the `MGLHillshadeStyleLayer.hillshadeIlluminationAnchor`
 property.
 */
typedef NS_ENUM(NSUInteger, MGLHillshadeIlluminationAnchor) {
    /**
     The hillshade illumination is relative to the north direction.
     */
    MGLHillshadeIlluminationAnchorMap,
    /**
     The hillshade illumination is relative to the top of the viewport.
     */
    MGLHillshadeIlluminationAnchorViewport,
};

/**
 Client-side hillshading visualization based on DEM data. Currently, the
 implementation only supports Mapbox Terrain RGB tiles

 You can access an existing hillshade style layer using the
 `-[MGLStyle layerWithIdentifier:]` method if you know its identifier;
 otherwise, find it using the `MGLStyle.layers` property. You can also create a
 new hillshade style layer and add it to the style using a method such as
 `-[MGLStyle addLayer:]`.

 ### Example

 ```swift
 ```
 */
MGL_EXPORT
@interface MGLHillshadeStyleLayer : MGLVectorStyleLayer

/**
 Returns a hillshade style layer initialized with an identifier and source.

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

#if TARGET_OS_IPHONE
/**
 The shading color used to accentuate rugged terrain like sharp cliffs and
 gorges.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *hillshadeAccentColor;
#else
/**
 The shading color used to accentuate rugged terrain like sharp cliffs and
 gorges.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *hillshadeAccentColor;
#endif

/**
 The transition affecting any changes to this layer’s `hillshadeAccentColor` property.

 This property corresponds to the `hillshade-accent-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition hillshadeAccentColorTransition;

/**
 Intensity of the hillshade
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0.5`. Set this property to `nil` to
 reset it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *hillshadeExaggeration;

/**
 The transition affecting any changes to this layer’s `hillshadeExaggeration` property.

 This property corresponds to the `hillshade-exaggeration-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition hillshadeExaggerationTransition;

#if TARGET_OS_IPHONE
/**
 The shading color of areas that faces towards the light source.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.whiteColor`. Set this property to `nil` to reset it to the default
 value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *hillshadeHighlightColor;
#else
/**
 The shading color of areas that faces towards the light source.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.whiteColor`. Set this property to `nil` to reset it to the default
 value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *hillshadeHighlightColor;
#endif

/**
 The transition affecting any changes to this layer’s `hillshadeHighlightColor` property.

 This property corresponds to the `hillshade-highlight-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition hillshadeHighlightColorTransition;

/**
 Direction of light source when map is rotated.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLHillshadeIlluminationAnchorViewport`. Set this
 property to `nil` to reset it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *hillshadeIlluminationAnchor;

/**
 The direction of the light source used to generate the hillshading with 0 as
 the top of the viewport if `hillshadeIlluminationAnchor` is set to
 `MGLHillshadeIlluminationAnchorViewport` and due north if
 `hillshadeIlluminationAnchor` is set to `MGLHillshadeIlluminationAnchorMap`.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `335`. Set this property to `nil` to
 reset it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *hillshadeIlluminationDirection;

/**
 The transition affecting any changes to this layer’s `hillshadeIlluminationDirection` property.

 This property corresponds to the `hillshade-illumination-direction-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition hillshadeIlluminationDirectionTransition;

#if TARGET_OS_IPHONE
/**
 The shading color of areas that face away from the light source.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<UIColor *> *hillshadeShadowColor;
#else
/**
 The shading color of areas that face away from the light source.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSColor *> *hillshadeShadowColor;
#endif

/**
 The transition affecting any changes to this layer’s `hillshadeShadowColor` property.

 This property corresponds to the `hillshade-shadow-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition hillshadeShadowColorTransition;

@end

/**
 Methods for wrapping an enumeration value for a style layer attribute in an
 `MGLHillshadeStyleLayer` object and unwrapping its raw value.
 */
@interface NSValue (MGLHillshadeStyleLayerAdditions)

#pragma mark Working with Hillshade Style Layer Attribute Values

/**
 Creates a new value object containing the given `MGLHillshadeIlluminationAnchor` enumeration.

 @param hillshadeIlluminationAnchor The value for the new object.
 @return A new value object that contains the enumeration value.
 */
+ (instancetype)valueWithMGLHillshadeIlluminationAnchor:(MGLHillshadeIlluminationAnchor)hillshadeIlluminationAnchor;

/**
 The `MGLHillshadeIlluminationAnchor` enumeration representation of the value.
 */
@property (readonly) MGLHillshadeIlluminationAnchor MGLHillshadeIlluminationAnchorValue;

@end

NS_ASSUME_NONNULL_END
