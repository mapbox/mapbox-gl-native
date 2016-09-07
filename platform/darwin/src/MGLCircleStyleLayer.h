// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, MGLCircleStyleLayerCircleTranslateAnchor) {
    MGLCircleStyleLayerCircleTranslateAnchorMap,
    MGLCircleStyleLayerCircleTranslateAnchorViewport,
};

typedef NS_ENUM(NSUInteger, MGLCircleStyleLayerCirclePitchScale) {
    MGLCircleStyleLayerCirclePitchScaleMap,
    MGLCircleStyleLayerCirclePitchScaleViewport,
};

@interface MGLCircleStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

#pragma mark - Accessing the Paint Attributes

/**
 Circle radius.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of `5`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> circleRadius;

/**
 The color of the circle.
 
 If this property is set to `nil`, the layer uses an implicit default value of `blackColor`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> circleColor;

/**
 Amount to blur the circle. 1 blurs the circle such that only the centerpoint is full opacity.
 
 If this property is set to `nil`, the layer uses an implicit default value of `0`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> circleBlur;

/**
 The opacity at which the circle will be drawn.
 
 If this property is set to `nil`, the layer uses an implicit default value of `1`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> circleOpacity;

/**
 The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.

 This property is measured in points.
 
 If this property is set to `nil`, the layer uses an implicit default value of 0 points from the left and 0 points from the top.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> circleTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen)
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLCircleStyleLayerCircleTranslateAnchorMap`.

 This property is only applied to the style if `circleTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> circleTranslateAnchor;

/**
 Controls the scaling behavior of the circle when the map is pitched. The value `MGLCircleStyleLayerCirclePitchScaleMap` scales circles according to their apparent distance to the camera. The value `MGLCircleStyleLayerCirclePitchScaleViewport` results in no pitch-related scaling.
 
 If this property is set to `nil`, the layer uses an implicit default value of `MGLCircleStyleLayerCirclePitchScaleMap`.
 */
@property (nonatomic, nullable) id <MGLStyleAttributeValue> circlePitchScale;

@end

NS_ASSUME_NONNULL_END
