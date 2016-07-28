#import <Foundation/Foundation.h>

@class MGLMapView;

@protocol MGLStyleLayer <NSObject>
@property (nonatomic, weak) MGLMapView *mapView;
@end
