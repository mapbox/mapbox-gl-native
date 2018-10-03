#import <Foundation/Foundation.h>
#import "MGLFoundation.h"
@protocol MGLFeature;

NS_ASSUME_NONNULL_BEGIN

FOUNDATION_EXTERN MGL_EXPORT const NSUInteger MGLClusterIdentifierInvalid;

FOUNDATION_EXPORT BOOL MGLFeatureHasClusterAttribute(id<MGLFeature> feature);

MGL_EXPORT
@protocol MGLCluster <NSObject>
@property (nonatomic, readonly) NSUInteger clusterIdentifier;
@property (nonatomic, readonly) NSUInteger clusterPointCount;
@property (nonatomic, readonly) NSString *clusterPointCountAbbreviation;
@end

NS_ASSUME_NONNULL_END
