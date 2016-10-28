#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/data_driven_property_value.hpp>

#include <functional>
#include <string>

namespace mbgl {
namespace style {
namespace conversion {

template <class V>
using LayoutPropertySetter = std::function<optional<Error> (Layer&, const V&)>;

template <class V>
using PaintPropertySetter = std::function<optional<Error> (Layer&, const V&, const optional<std::string>&)>;

template <class V, class L, class PropertyValue, class...Args>
auto makePropertySetter(void (L::*setter)(PropertyValue, const Args&...args)) {
    return [setter] (Layer& layer, const V& value, const Args&...args) -> optional<Error> {
        L* typedLayer = layer.as<L>();
        if (!typedLayer) {
            return Error { "layer doesn't support this property" };
        }

        Result<PropertyValue> typedValue = convert<PropertyValue>(value);
        if (!typedValue) {
            return typedValue.error();
        }

        (typedLayer->*setter)(*typedValue, args...);
        return {};
    };
}

template <class V>
optional<Error> setVisibility(Layer& layer, const V& value) {
    if (isUndefined(value)) {
        layer.setVisibility(VisibilityType::Visible);
        return {};
    }

    Result<VisibilityType> visibility = convert<VisibilityType>(value);
    if (!visibility) {
        return visibility.error();
    }

    layer.setVisibility(*visibility);
    return {};
}

} // namespace conversion
} // namespace style
} // namespace mbgl
