#import <Foundation/Foundation.h>

@class MGLMapView;

@protocol MGLStyleLayer <NSObject>

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic, copy, readonly) NSString *layerIdentifier;
@property (nonatomic, readonly) NSString *sourceIdentifier;
@property (nonatomic, readonly) NSString *sourceLayerIdentifier;

@end
