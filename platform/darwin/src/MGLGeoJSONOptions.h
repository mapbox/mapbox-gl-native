#import <Foundation/Foundation.h>

@interface MGLGeoJSONOptions : NSObject

@property (nonatomic, assign) NSInteger maximumZoom;
@property (nonatomic, assign) NSInteger buffer;
@property (nonatomic, assign) double tolerance;

@property (nonatomic, assign) BOOL cluster;
@property (nonatomic, assign) NSInteger clusterRadius;
@property (nonatomic, assign) NSInteger clusterMaximumZoom;

@end
