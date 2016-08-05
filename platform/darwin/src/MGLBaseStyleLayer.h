#import <Foundation/Foundation.h>

@interface MGLBaseStyleLayer : NSObject

@property (nonatomic, assign, getter=isVisible) BOOL visible;
@property (nonatomic, assign) float maxZoom;
@property (nonatomic, assign) float minZoom;

@end
