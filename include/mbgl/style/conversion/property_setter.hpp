#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/data_driven_property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>

#include <string>

namespace mbgl {
namespace style {
namespace conversion {

template <class V>
using LayoutPropertySetter = optional<Error> (*) (Layer&, const V&);

template <class V>
using PaintPropertySetter = optional<Error> (*) (Layer&, const V&, const optional<std::string>&);

template <class V, class L, class PropertyValue, void (L::*setter)(PropertyValue)>
optional<Error> setLayoutProperty(Layer& layer, const V& value) {
    L* typedLayer = layer.as<L>();
    if (!typedLayer) {
        return Error { "layer doesn't support this property" };
    }

    Result<PropertyValue> typedValue = convert<PropertyValue>(value);
    if (!typedValue) {
        return typedValue.error();
    }

    (typedLayer->*setter)(*typedValue);
    return {};
}

template <class V, class L, class PropertyValue, void (L::*setter)(PropertyValue, const optional<std::string>&)>
optional<Error> setPaintProperty(Layer& layer, const V& value, const optional<std::string>& klass) {
    L* typedLayer = layer.as<L>();
    if (!typedLayer) {
        return Error { "layer doesn't support this property" };
    }

    Result<PropertyValue> typedValue = convert<PropertyValue>(value);
    if (!typedValue) {
        return typedValue.error();
    }

    (typedLayer->*setter)(*typedValue, klass);
    return {};
}

template <class V, class L, void (L::*setter)(const TransitionOptions&, const optional<std::string>&)>
optional<Error> setTransition(Layer& layer, const V& value, const optional<std::string>& klass) {
    L* typedLayer = layer.as<L>();
    if (!typedLayer) {
        return Error { "layer doesn't support this property" };
    }

    Result<TransitionOptions> transition = convert<TransitionOptions>(value);
    if (!transition) {
        return transition.error();
    }

    (typedLayer->*setter)(*transition, klass);
    return {};
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
