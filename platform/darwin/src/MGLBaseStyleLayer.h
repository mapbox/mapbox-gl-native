#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The base style layer class from which all other style layer classes 
 inherit. Style layers allow runtime customization of all map styling 
 properties.
 
 You should use the concrete subclasses of `MGLBaseStyleLayer` (which
 conform to `MGLStyleLayer`) to style fill, line, symbol, and other layer 
 types.
 */
@interface MGLBaseStyleLayer : NSObject

/**
 The display of the layer. A value of `NO` hides the layer.
 */
@property (nonatomic, assign, getter=isVisible) BOOL visible;

/**
 The maximum zoom level at which the layer gets parsed and appears.
 */
@property (nonatomic, assign) float maximumZoomLevel;

/**
 The minimum zoom level at which the layer gets parsed and appears.
 */
@property (nonatomic, assign) float minimumZoomLevel;

@end

NS_ASSUME_NONNULL_END
