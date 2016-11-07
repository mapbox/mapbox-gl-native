#import "MGLGeoJSONSource.h"

#include <mbgl/style/sources/geojson_source.hpp>

@interface MGLGeoJSONSourceBase (Private)

@property (nonatomic, readwrite) NSDictionary *options;

- (instancetype)initWithIdentifier:(NSString *)identifier options:(NS_DICTIONARY_OF(NSString *, id) *)options;

- (mbgl::style::GeoJSONOptions)geoJSONOptions;

@end
