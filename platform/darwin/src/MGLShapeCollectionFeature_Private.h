#import "MGLFeature.h"

#import <mbgl/util/feature.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface MGLShapeCollectionFeature ()

- (mbgl::FeatureCollection)mbglFeatureCollection;

@end

NS_ASSUME_NONNULL_END
