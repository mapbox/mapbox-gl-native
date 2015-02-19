#import <UIKit/UIKit.h>

@interface MGLStyleFunctionValue : NSObject

+ (instancetype)linearFunctionWithBaseZoomLevel:(CGFloat)zBase
                                   initialValue:(CGFloat)val
                                          slope:(CGFloat)slope
                                   minimumValue:(CGFloat)min
                                   maximumValue:(CGFloat)max;

+ (instancetype)exponentialFunctionWithBaseZoomLevel:(CGFloat)zBase
                                        initialValue:(CGFloat)val
                                               slope:(CGFloat)slope
                                        minimumValue:(CGFloat)min
                                        maximumValue:(CGFloat)max;

+ (instancetype)minimumZoomLevelFunction:(CGFloat)minimumZoom;

+ (instancetype)maximumZoomLevelFunction:(CGFloat)maximumZoom;

+ (instancetype)stopsFunctionWithZoomLevelsAndValues:(NSNumber *)firstZoom, ... NS_REQUIRES_NIL_TERMINATION;

@end
