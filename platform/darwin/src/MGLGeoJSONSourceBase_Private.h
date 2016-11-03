#import "MGLGeoJSONSource.h"

#include <mbgl/style/sources/geojson_source.hpp>

@interface MGLGeoJSONSourceBase (Private)

@property (nonatomic, readwrite) NSDictionary *options;
- (mbgl::style::GeoJSONOptions)geoJSONOptions;

@end
