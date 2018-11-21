#import "MGLFoundation.h"
#import "MGLTypes.h"
#import "MGLComputedShapeSource.h"

#include <mbgl/style/sources/custom_geometry_source.hpp>

NS_ASSUME_NONNULL_BEGIN

MGL_EXPORT
mbgl::style::CustomGeometrySource::Options MBGLCustomGeometrySourceOptionsFromDictionary(NSDictionary<MGLShapeSourceOption, id> *options);

NS_ASSUME_NONNULL_END
