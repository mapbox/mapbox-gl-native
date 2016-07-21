// This file is generated. Do not edit.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"

typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineCap) {
    MGLLineStyleLayerLineCapButt,
    MGLLineStyleLayerLineCapRound,
    MGLLineStyleLayerLineCapSquare,
};
typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineJoin) {
    MGLLineStyleLayerLineJoinBevel,
    MGLLineStyleLayerLineJoinRound,
    MGLLineStyleLayerLineJoinMiter,
};
typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineTranslateAnchor) {
    MGLLineStyleLayerLineTranslateAnchorMap,
    MGLLineStyleLayerLineTranslateAnchorViewport,
};

@interface MGLLineStyleLayer : MGLStyleLayer

// Layout properties

@property (nonatomic) MGLLineStyleLayerLineCap lineCap;

@property (nonatomic) MGLLineStyleLayerLineJoin lineJoin;

@property (nonatomic) CGFloat lineMiterLimit;

@property (nonatomic) CGFloat lineRoundLimit;

// Paint properties

@property (nonatomic) CGFloat lineOpacity;

@property (nonatomic) MGLColor *lineColor;

@property (nonatomic) NSArray *lineTranslate;

@property (nonatomic) MGLLineStyleLayerLineTranslateAnchor lineTranslateAnchor;

@property (nonatomic) CGFloat lineWidth;

@property (nonatomic) CGFloat lineGapWidth;

@property (nonatomic) CGFloat lineOffset;

@property (nonatomic) CGFloat lineBlur;

@property (nonatomic) NSArray *lineDasharray;

@property (nonatomic) NSString *linePattern;

@end

