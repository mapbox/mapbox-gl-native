#pragma once

#include <mapbox/geojson.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
optional<GeoJSON> Converter<GeoJSON>::operator()(const mbgl::android::Value& value, Error& error) const {
    if(value.isNull() || !value.isString()) {
        error = { "no json data found" };
        return {};
    }

    return convert<GeoJSON>(value.toString(), error);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
