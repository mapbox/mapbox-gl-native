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
 
 The default value of this property is an `NSNumber` object containing the float `5`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleRadius;

#if TARGET_OS_IPHONE
/**
 The color of the circle.
 
 The default value of this property is `UIColor.blackColor`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleColor;
#else
/**
 The color of the circle.
 
 The default value of this property is `NSColor.blackColor`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleColor;
#endif

/**
 Amount to blur the circle. 1 blurs the circle such that only the centerpoint is full opacity.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleBlur;

/**
 The opacity at which the circle will be drawn.
 
 The default value of this property is an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleOpacity;

/**
 The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.

 This property is measured in points.
 
 The default value of this property is an `NSValue` object containing a `CGVector` struct set to 0 points from the left and 0 points from the top. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen)
 
 The default value of this property is an `NSValue` object containing `MGLCircleStyleLayerCircleTranslateAnchorMap`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `circleTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circleTranslateAnchor;

/**
 Controls the scaling behavior of the circle when the map is pitched. The value `MGLCircleStyleLayerCirclePitchScaleMap` scales circles according to their apparent distance to the camera. The value `MGLCircleStyleLayerCirclePitchScaleViewport` results in no pitch-related scaling.
 
 The default value of this property is an `NSValue` object containing `MGLCircleStyleLayerCirclePitchScaleMap`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> circlePitchScale;

@end

NS_ASSUME_NONNULL_END
