#import "MGLGeoJSONSource.h"
#import "MGLGeoJSONSource_Private.h"

#include <mbgl/style/sources/geojson_source.hpp>

@interface MGLGeoJSONSource (Private)

- (mbgl::style::GeoJSONOptions)geoJSONOptions;

@end
