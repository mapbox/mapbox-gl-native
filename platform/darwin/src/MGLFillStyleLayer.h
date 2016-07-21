// This file is generated. Do not edit.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"

typedef NS_ENUM(NSUInteger, MGLFillStyleLayerFillTranslateAnchor) {
    MGLFillStyleLayerFillTranslateAnchorMap,
    MGLFillStyleLayerFillTranslateAnchorViewport,
};

@interface MGLFillStyleLayer : MGLStyleLayer

// Paint properties

@property (nonatomic) BOOL fillAntialias;

@property (nonatomic) CGFloat fillOpacity;

@property (nonatomic) MGLColor *fillColor;

@property (nonatomic) MGLColor *fillOutlineColor;

@property (nonatomic) NSArray *fillTranslate;

@property (nonatomic) MGLFillStyleLayerFillTranslateAnchor fillTranslateAnchor;

@property (nonatomic) NSString *fillPattern;

@end

