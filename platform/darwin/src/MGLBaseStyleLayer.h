#import <Foundation/Foundation.h>

@interface MGLBaseStyleLayer : NSObject

@property (nonatomic, assign, getter=isVisible) BOOL visible;

/**
 The maximum zoom level on which the layer gets parsed and appears on.
 */
@property (nonatomic, assign) float maximumZoomLevel;

/**
 The minimum zoom level on which the layer gets parsed and appears on.
 */
@property (nonatomic, assign) float minimumZoomLevel;

/**
 Updates the layer’s layout and paint properties.
 */
- (void)update;

@end
