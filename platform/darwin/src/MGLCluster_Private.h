#import <Foundation/Foundation.h>
#import "MGLFoundation.h"
#import "MGLCluster.h"

@protocol MGLFeature;

FOUNDATION_EXPORT NSString * const MGLClusterBoolKey;
FOUNDATION_EXPORT NSString * const MGLClusterIdentifierKey;
FOUNDATION_EXPORT NSString * const MGLClusterCountKey;
FOUNDATION_EXPORT NSString * const MGLClusterCountAbbreviationKey;

FOUNDATION_EXPORT BOOL MGLFeatureHasClusterAttribute(id<MGLFeature> feature);

FOUNDATION_EXPORT MGL_EXPORT
NSString *MGLClusterSubclassNameForFeature(id<MGLFeature> feature);

FOUNDATION_EXPORT
id<MGLFeature, MGLCluster> MGLConvertFeatureToClusterSubclass(id<MGLFeature> feature);

