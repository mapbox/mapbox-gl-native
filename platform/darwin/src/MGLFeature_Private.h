#import "MGLFeature.h"
#import "MGLShape.h"

#import <mbgl/util/geo.hpp>
#import <mbgl/util/feature.hpp>

NS_ASSUME_NONNULL_BEGIN

/**
 Returns an array of `MGLFeature` objects converted from the given vector of
 vector tile features.
 */
NS_ARRAY_OF(MGLShape <MGLFeature> *) *MGLFeaturesFromMBGLFeatures(const std::vector<mbgl::Feature> &features);

/**
 Takes an `mbgl::Feature` object, an identifer, and attributes dictionary and
 returns the feature object with converted `mbgl::FeatureIdentifier` and 
 `mbgl::PropertyMap` properties.
 */
mbgl::Feature mbglFeature(mbgl::Feature feature, id identifier, NSDictionary *attributes);

/**
 Returns an `NSDictionary` representation of an `MGLFeature`.
 */
NS_DICTIONARY_OF(NSString *, id) *NSDictionaryFeatureForGeometry(NSDictionary *geometry, NSDictionary *attributes, id identifier);

@protocol MGLFeaturePrivate <MGLFeature>

- (mbgl::Feature)mbglFeature;

@end

NS_ASSUME_NONNULL_END
