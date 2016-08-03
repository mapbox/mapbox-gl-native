// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeValue.h"

@interface MGLBackgroundStyleLayer : NSObject <MGLStyleLayer>

#pragma mark - Accessing the Paint Attributes

/**
 The color with which the background will be drawn.
 */
@property (nonatomic) id <MGLStyleAttributeValue> backgroundColor;

/**
 Name of image in sprite to use for drawing an image background. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
 */
@property (nonatomic) id <MGLStyleAttributeValue> backgroundPattern;

/**
 The opacity at which the background will be drawn.
 */
@property (nonatomic) id <MGLStyleAttributeValue> backgroundOpacity;

@end
