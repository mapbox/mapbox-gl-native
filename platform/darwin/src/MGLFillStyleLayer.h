// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeValue.h"

typedef NS_ENUM(NSUInteger, MGLFillStyleLayerFillTranslateAnchor) {
    MGLFillStyleLayerFillTranslateAnchorMap,
    MGLFillStyleLayerFillTranslateAnchorViewport,
};

@interface MGLFillStyleLayer : NSObject <MGLStyleLayer>

#pragma mark - Accessing the Paint Attributes

/**
  Whether or not the fill should be antialiased.
*/
@property (nonatomic) id <MGLStyleAttributeValue> fillAntialias;

/**
  The opacity of the entire fill layer. In contrast to the fill-color, this value will also affect the 1px stroke around the fill, if the stroke is used.
*/
@property (nonatomic) id <MGLStyleAttributeValue> fillOpacity;

/**
  The color of the filled part of this layer. This color can be specified as rgba with an alpha component and the color's opacity will not affect the opacity of the 1px stroke, if it is used.
*/
@property (nonatomic) id <MGLStyleAttributeValue> fillColor;

/**
  The outline color of the fill. Matches the value of `fill-color` if unspecified.
*/
@property (nonatomic) id <MGLStyleAttributeValue> fillOutlineColor;

/**
  The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.
*/
@property (nonatomic) id <MGLStyleAttributeValue> fillTranslate;

/**
  Control whether the translation is relative to the map (north) or viewport (screen)
*/
@property (nonatomic) id <MGLStyleAttributeValue> fillTranslateAnchor;

/**
  Name of image in sprite to use for drawing image fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
*/
@property (nonatomic) id <MGLStyleAttributeValue> fillPattern;

@end

