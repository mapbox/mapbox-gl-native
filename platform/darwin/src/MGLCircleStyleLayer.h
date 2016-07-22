// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"

typedef NS_ENUM(NSUInteger, MGLCircleStyleLayerCircleTranslateAnchor) {
    MGLCircleStyleLayerCircleTranslateAnchorMap,
    MGLCircleStyleLayerCircleTranslateAnchorViewport,
};
typedef NS_ENUM(NSUInteger, MGLCircleStyleLayerCirclePitchScale) {
    MGLCircleStyleLayerCirclePitchScaleMap,
    MGLCircleStyleLayerCirclePitchScaleViewport,
};

@interface MGLCircleStyleLayer : MGLStyleLayer

#pragma mark - Accessing the Paint Attributes

/**
  Circle radius.
*/
@property (nonatomic) CGFloat circleRadius;

/**
  The color of the circle.
*/
@property (nonatomic) MGLColor *circleColor;

/**
  Amount to blur the circle. 1 blurs the circle such that only the centerpoint is full opacity.
*/
@property (nonatomic) CGFloat circleBlur;

/**
  The opacity at which the circle will be drawn.
*/
@property (nonatomic) CGFloat circleOpacity;

/**
  The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
*/
@property (nonatomic) NSArray *circleTranslate;

/**
  Control whether the translation is relative to the map (north) or viewport (screen)
*/
@property (nonatomic) MGLCircleStyleLayerCircleTranslateAnchor circleTranslateAnchor;

/**
  Controls the scaling behavior of the circle when the map is pitched. The value `map` scales circles according to their apparent distance to the camera. The value `viewport` results in no pitch-related scaling.
*/
@property (nonatomic) MGLCircleStyleLayerCirclePitchScale circlePitchScale;

@end

