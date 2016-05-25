#import "MGLFeature.h"
#import "MGLShape.h"

#import <mbgl/util/geo.hpp>
#import <mbgl/util/feature.hpp>

/**
 Returns an array of `MGLFeature` objects converted from the given vector of
 vector tile features.
 */
NS_ARRAY_OF(MGLShape <MGLFeature> *) *MGLFeaturesFromMBGLFeatures(const std::vector<mbgl::Feature> &features);
