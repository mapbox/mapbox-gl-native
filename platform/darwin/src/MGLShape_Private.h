#import "MGLShape.h"

#import <mbgl/util/geojson.hpp>
#import <mbgl/util/geometry.hpp>
#import <mbgl/util/geo.hpp>

bool operator==(const CLLocationCoordinate2D lhs, const CLLocationCoordinate2D rhs);

@interface MGLShape (Private)

/**
 Returns an `mbgl::GeoJSON` representation of the `MGLShape`.
 */
- (mbgl::GeoJSON)geoJSONObject;

/**
 Returns an `mbgl::Geometry<double>` representation of the `MGLShape`.
 */
- (mbgl::Geometry<double>)geometryObject;

/**
 Returns a dictionary with the GeoJSON geometry member object.
 */
- (NSDictionary *)geoJSONDictionary;

@end
