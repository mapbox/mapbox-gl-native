#import "MGLAbstractShapeSource.h"

#import "MGLFoundation.h"
#import "MGLTypes.h"
#import "MGLShape.h"

#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface MGLAbstractShapeSource (Private)

MGL_EXPORT

mbgl::style::GeoJSONOptions MGLGeoJSONOptionsFromDictionary(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *options);

MGL_EXPORT
mbgl::style::CustomGeometrySource::Options MBGLCustomGeometrySourceOptionsFromDictionary(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *options);

@end
NS_ASSUME_NONNULL_END
