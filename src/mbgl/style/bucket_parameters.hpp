#pragma once

#include <mbgl/util/variant.hpp>
#include <mapbox/recursive_wrapper.hpp>

namespace mbgl {
namespace style {

class GeometryBucketParameters;
class RasterBucketParameters;

using BucketParameters = variant<mapbox::util::recursive_wrapper<GeometryBucketParameters>,
                                 mapbox::util::recursive_wrapper<RasterBucketParameters>>;

} // namespace style
} // namespace mbgl
