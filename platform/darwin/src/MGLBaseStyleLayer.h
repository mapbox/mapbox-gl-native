#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

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

@end

NS_ASSUME_NONNULL_END
