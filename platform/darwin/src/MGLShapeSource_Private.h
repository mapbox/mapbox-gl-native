#import "MGLShapeSource.h"
#import "MGLShapeSource_Private.h"

#include <mbgl/style/sources/geojson_source.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface MGLShapeSource (Private)

- (instancetype)initWithRawSource:(mbgl::style::GeoJSONSource *)rawSource;

- (mbgl::style::GeoJSONOptions)geoJSONOptions;

@end

NS_ASSUME_NONNULL_END
