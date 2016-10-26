#import "MGLShape.h"

#import <mbgl/util/feature.hpp>

@interface MGLShape (Private)

/**
 Returns an `mbgl::Feature` representation of the `MGLShape`.
 */
- (mbgl::Feature)featureObject;

/**
 Returns a dictionary with the GeoJSON geometry member object.
 */
- (NSDictionary *)geoJSONDictionary;

@end
