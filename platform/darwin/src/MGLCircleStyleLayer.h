// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeValue.h"

typedef NS_ENUM(NSUInteger, MGLCircleStyleLayerCircleTranslateAnchor) {
    MGLCircleStyleLayerCircleTranslateAnchorMap,
    MGLCircleStyleLayerCircleTranslateAnchorViewport,
};

typedef NS_ENUM(NSUInteger, MGLCircleStyleLayerCirclePitchScale) {
    MGLCircleStyleLayerCirclePitchScaleMap,
    MGLCircleStyleLayerCirclePitchScaleViewport,
};

@interface MGLCircleStyleLayer : NSObject <MGLStyleLayer>

#pragma mark - Accessing the Paint Attributes

/**
 Circle radius.
 */
@property (nonatomic) id <MGLStyleAttributeValue> circleRadius;

/**
 The color of the circle.
 */
@property (nonatomic) id <MGLStyleAttributeValue> circleColor;

/**
 Amount to blur the circle. 1 blurs the circle such that only the centerpoint is full opacity.
 */
@property (nonatomic) id <MGLStyleAttributeValue> circleBlur;

/**
 The opacity at which the circle will be drawn.
 */
@property (nonatomic) id <MGLStyleAttributeValue> circleOpacity;

/**
 The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
 */
@property (nonatomic) id <MGLStyleAttributeValue> circleTranslate;

/**
 Control whether the translation is relative to the map (north) or viewport (screen)
 */
@property (nonatomic) id <MGLStyleAttributeValue> circleTranslateAnchor;

/**
 Controls the scaling behavior of the circle when the map is pitched. The value `map` scales circles according to their apparent distance to the camera. The value `viewport` results in no pitch-related scaling.
 */
@property (nonatomic) id <MGLStyleAttributeValue> circlePitchScale;

@end
