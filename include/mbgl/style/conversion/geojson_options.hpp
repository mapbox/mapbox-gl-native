#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<GeoJSONOptions> {
    optional<GeoJSONOptions> operator()(const Convertible& value, Error& error) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
