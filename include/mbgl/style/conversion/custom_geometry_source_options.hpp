#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<CustomGeometrySource::Options> {
    optional<CustomGeometrySource::Options> operator()(const Convertible& value, Error& error) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
