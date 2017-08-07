#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<GeoJSON> {
public:
    optional<GeoJSON> operator()(const std::string&, Error&) const;

    // This is explicitly specialized in the .cpp file for JSValue. It may also be explicitly
    // specialized for SDK-specific types (e.g. mbgl::android::Value).
    template <class V>
    optional<GeoJSON> operator()(const V&, Error&) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
