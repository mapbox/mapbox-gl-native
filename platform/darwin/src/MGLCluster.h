#import <Foundation/Foundation.h>
#import "MGLFoundation.h"

@protocol MGLFeature;

NS_ASSUME_NONNULL_BEGIN

/**
 An `NSUInteger` constant used to indicate an invalid cluster identifier.
 This indicates a missing cluster feature.
 */
FOUNDATION_EXTERN MGL_EXPORT const NSUInteger MGLClusterIdentifierInvalid;

/**
 A protocol that (private) feature subclasses (i.e. those already conforming to
 the `MGLFeature` protocol) conform to if they are represent clusters.
 
 Currently only subclasses of `MGLPointFeature` support `MGLCluster`.
 
 To check if a feature is a cluster, check conformity to `MGLCluster`, for
 example:
 
 ```swift
 if let cluster = feature as? MGLCluster {
    ...
 }
 ```
 */
MGL_EXPORT
@protocol MGLCluster <NSObject>

/** The identifier for the cluster. */
@property (nonatomic, readonly) NSUInteger clusterIdentifier;

/** The number of points within this cluster */
@property (nonatomic, readonly) NSUInteger clusterPointCount;

/**
 An `NSString` abbreviation for the number of points within this cluster. For
 example "1.2k".
 */
@property (nonatomic, readonly) NSString *clusterPointCountAbbreviation;
@end

NS_ASSUME_NONNULL_END
