#import <Foundation/Foundation.h>

@class MGLMapView;

@protocol MGLStyleLayer <NSObject>

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic, copy, readonly) NSString *layerIdentifier;

@optional

@property (nonatomic, readonly) NSString *sourceIdentifier;

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier;
- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier sourceIdentifier:(NSString *)sourceIdentifier;

@end
