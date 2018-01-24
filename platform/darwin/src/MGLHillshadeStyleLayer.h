// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLFoundation.h"
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

/**
 The shading color used to accentuate rugged terrain like sharp cliffs and
 gorges.
 
 The default value of this property is an expression that evaluates to
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `UIColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *hillshadeAccentColor;

/**
 The transition affecting any changes to this layer’s `hillshadeAccentColor` property.

 This property corresponds to the `hillshade-accent-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition hillshadeAccentColorTransition;

/**
 Intensity of the hillshade
 
 The default value of this property is an expression that evaluates to the float
 `0.5`. Set this property to `nil` to reset it to the default value.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *hillshadeExaggeration;

/**
 The transition affecting any changes to this layer’s `hillshadeExaggeration` property.

 This property corresponds to the `hillshade-exaggeration-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition hillshadeExaggerationTransition;

/**
 The shading color of areas that faces towards the light source.
 
 The default value of this property is an expression that evaluates to
 `UIColor.whiteColor`. Set this property to `nil` to reset it to the default
 value.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `UIColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *hillshadeHighlightColor;

/**
 The transition affecting any changes to this layer’s `hillshadeHighlightColor` property.

 This property corresponds to the `hillshade-highlight-color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition hillshadeHighlightColorTransition;

/**
 Direction of light source when map is rotated.
 
 The default value of this property is an expression that evaluates to
 `viewport`. Set this property to `nil` to reset it to the default value.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `MGLHillshadeIlluminationAnchor` values
 * Any of the following constant string values:
   * `map`: The hillshade illumination is relative to the north direction.
   * `viewport`: The hillshade illumination is relative to the top of the
 viewport.
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation functions to the
 `$zoomLevel` variable or applying interpolation or step functions to feature
 attributes.
 */
@property (nonatomic, null_resettable) NSExpression *hillshadeIlluminationAnchor;

/**
 The direction of the light source used to generate the hillshading with 0 as
 the top of the viewport if `hillshadeIlluminationAnchor` is set to
 `MGLHillshadeIlluminationAnchorViewport` and due north if
 `hillshadeIlluminationAnchor` is set to `MGLHillshadeIlluminationAnchorMap`.
 
 The default value of this property is an expression that evaluates to the float
 `335`. Set this property to `nil` to reset it to the default value.
 
 You can set this property to an expression containing any of the following:
 
 * Constant numeric values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *hillshadeIlluminationDirection;

/**
 The transition affecting any changes to this layer’s `hillshadeIlluminationDirection` property.

 This property corresponds to the `hillshade-illumination-direction-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition hillshadeIlluminationDirectionTransition;

/**
 The shading color of areas that face away from the light source.
 
 The default value of this property is an expression that evaluates to
 `UIColor.blackColor`. Set this property to `nil` to reset it to the default
 value.
 
 You can set this property to an expression containing any of the following:
 
 * Constant `UIColor` values
 * Predefined functions, including mathematical and string operators
 * Conditional expressions
 * Variable assignments and references to assigned variables
 * Interpolation and step functions applied to the `$zoomLevel` variable
 
 This property does not support applying interpolation or step functions to
 feature attributes.
 */
@property (nonatomic, null_resettable) NSExpression *hillshadeShadowColor;

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
