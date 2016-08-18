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

 The default value of this property is `5`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleRadius;

/**
 The color of the circle.

 The default value of this property is an `NSColor` or `UIColor`object whose RGB value is 0, 0, 0 and whose alpha value is 1. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleColor;

/**
 Amount to blur the circle. 1 blurs the circle such that only the centerpoint is full opacity.

 The default value of this property is `0`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleBlur;

/**
 The opacity at which the circle will be drawn.

 The default value of this property is `1`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleOpacity;

/**
 The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.

 The default value of this property is 0 from the left and 0 from the top. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen)

 The default value of this property is `MGLCircleStyleLayerCircleTranslateAnchorMap`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleTranslateAnchor;

/**
 Controls the scaling behavior of the circle when the map is pitched. The value `MGLCircleStyleLayerCirclePitchScaleMap` scales circles according to their apparent distance to the camera. The value `MGLCircleStyleLayerCirclePitchScaleViewport` results in no pitch-related scaling.

 The default value of this property is `MGLCircleStyleLayerCirclePitchScaleMap`. Set this property to `nil` to reset it to the default.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circlePitchScale;

@end

NS_ASSUME_NONNULL_END
