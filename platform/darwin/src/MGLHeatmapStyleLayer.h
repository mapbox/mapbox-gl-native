// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLFoundation.h"
#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 A heatmap.

 You can access an existing heatmap style layer using the
 `-[MGLStyle layerWithIdentifier:]` method if you know its identifier;
 otherwise, find it using the `MGLStyle.layers` property. You can also create a
 new heatmap style layer and add it to the style using a method such as
 `-[MGLStyle addLayer:]`.

 ### Example

 ```swift
 ```
 */
MGL_EXPORT
@interface MGLHeatmapStyleLayer : MGLVectorStyleLayer

/**
 Returns a heatmap style layer initialized with an identifier and source.

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
 Similar to `heatmapWeight` but controls the intensity of the heatmap globally.
 Primarily used for adjusting the heatmap based on zoom level.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1`. Set this property to `nil` to reset
 it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *heatmapIntensity;

/**
 The transition affecting any changes to this layer’s `heatmapIntensity` property.

 This property corresponds to the `heatmap-intensity-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition heatmapIntensityTransition;

/**
 The global opacity at which the heatmap layer will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1`. Set this property to `nil` to reset
 it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *heatmapOpacity;

/**
 The transition affecting any changes to this layer’s `heatmapOpacity` property.

 This property corresponds to the `heatmap-opacity-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition heatmapOpacityTransition;

/**
 Radius of influence of one heatmap point in points. Increasing the value makes
 the heatmap smoother, but less detailed.
 
 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `30`. Set this property to `nil` to
 reset it to the default value.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *heatmapRadius;

/**
 The transition affecting any changes to this layer’s `heatmapRadius` property.

 This property corresponds to the `heatmap-radius-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition heatmapRadiusTransition;

/**
 A measure of how much an individual point contributes to the heatmap. A value
 of 10 would be equivalent to having 10 points of weight 1 in the same spot.
 Especially useful when combined with clustering.
 
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
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *heatmapWeight;

@end

NS_ASSUME_NONNULL_END
