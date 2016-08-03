#import <Foundation/Foundation.h>

@class MGLMapView;

@protocol MGLStyleLayer <NSObject>

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic, readonly) NSString *layerID;

@optional

@property (nonatomic, readonly) NSString *sourceID;

- (instancetype)initWithLayerID:(NSString *)layerID;
- (instancetype)initWithLayerID:(NSString *)layerID sourceID:(NSString *)sourceID;

@end
