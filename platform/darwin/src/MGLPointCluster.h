#import <Foundation/Foundation.h>
#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

MGL_EXPORT
// TODO: Should this be a MGLPointClusterFeature? Subclass of MGLPointFeature?
@protocol MGLPointCluster <NSObject>
@property (nonatomic, readonly, getter=isCluster) BOOL cluster;     // Keep so we match cluster def? Always YES.
@property (nonatomic, readonly) uint32_t clusterId;
@property (nonatomic, readonly) NSUInteger pointCount;
@property (nonatomic, readonly) NSString *clusterAbbreviated;
@end

NS_ASSUME_NONNULL_END
