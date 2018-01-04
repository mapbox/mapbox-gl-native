#pragma once

#include <mapbox/geojson.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
optional<std::array<LatLng, 4>> Converter<std::array<LatLng, 4>>::operator()(const mbgl::android::Value& value, Error& error) const {
    if (value.isNull() || !value.isArray()) {
        error = { "value cannot be converted to LatLng array" };
        return {};
    }

    return convert<GeoJSON>(value.toString(), error);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
