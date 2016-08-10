#import <Foundation/Foundation.h>

@interface MGLBaseStyleLayer : NSObject

@property (nonatomic, assign, getter=isVisible) BOOL visible;

/**
 The maximum zoom level on which the layer gets parsed and appears on.
 */
@property (nonatomic, assign) float maxZoom;

/**
 The minimum zoom level on which the layer gets parsed and appears on.
 */
@property (nonatomic, assign) float minZoom;

/**
 Updates the layers layout and paint properties.
 
 @discussion The properties are recalculated upon managing sources and
 layers but in order to update a layout or paint property you have to call
 this method manually.
 */
- (void)update;

@end
