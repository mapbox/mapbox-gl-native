// This file is generated. Do not edit.

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

// Paint properties

@property (nonatomic) CGFloat circleRadius;

@property (nonatomic) MGLColor *circleColor;

@property (nonatomic) CGFloat circleBlur;

@property (nonatomic) CGFloat circleOpacity;

@property (nonatomic) NSArray *circleTranslate;

@property (nonatomic) MGLCircleStyleLayerCircleTranslateAnchor circleTranslateAnchor;

@property (nonatomic) MGLCircleStyleLayerCirclePitchScale circlePitchScale;

@end

