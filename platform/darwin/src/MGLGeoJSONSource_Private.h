#import "MGLGeoJSONSource.h"

#include <mbgl/style/sources/geojson_source.hpp>

@interface MGLGeoJSONSource (Private)

- (instancetype)initWithRawSource:(mbgl::style::GeoJSONSource *)rawSource;

- (mbgl::style::GeoJSONOptions)geoJSONOptions;

@end
