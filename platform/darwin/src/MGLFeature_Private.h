#import "MGLFeature.h"
#import "MGLShape.h"

#import <mbgl/util/geo.hpp>
#import <mbgl/util/feature.hpp>

NS_ARRAY_OF(MGLShape <MGLFeature> *) *MGLFeaturesFromMBGLFeatures(const std::vector<mbgl::Feature> &features);
