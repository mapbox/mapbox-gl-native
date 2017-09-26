#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<GeoJSON> {
public:
    optional<GeoJSON> operator()(const Value&, Error&) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
