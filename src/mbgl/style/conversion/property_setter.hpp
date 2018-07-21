#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/color_ramp_property_value.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/data_driven_property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>

#include <string>

namespace mbgl {
namespace style {
namespace conversion {

using PropertySetter = optional<Error> (*) (Layer&, const Convertible&);

template <class L, class PropertyValue, void (L::*setter)(PropertyValue), bool convertTokens = false>
optional<Error> setProperty(Layer& layer, const Convertible& value) {
    auto* typedLayer = layer.as<L>();
    if (!typedLayer) {
        return Error { "layer doesn't support this property" };
    }

    Error error;
    optional<PropertyValue> typedValue = convert<PropertyValue>(value, error, convertTokens);
    if (!typedValue) {
        return error;
    }

    (typedLayer->*setter)(*typedValue);
    return {};
}

template <class L, void (L::*setter)(const TransitionOptions&)>
optional<Error> setTransition(Layer& layer, const Convertible& value) {
    auto* typedLayer = layer.as<L>();
    if (!typedLayer) {
        return Error { "layer doesn't support this property" };
    }

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }

    (typedLayer->*setter)(*transition);
    return {};
}

inline optional<Error> setVisibility(Layer& layer, const Convertible& value) {
    if (isUndefined(value)) {
        layer.setVisibility(VisibilityType::Visible);
        return {};
    }

    Error error;
    optional<VisibilityType> visibility = convert<VisibilityType>(value, error);
    if (!visibility) {
        return error;
    }

    layer.setVisibility(*visibility);
    return {};
}

} // namespace conversion
} // namespace style
} // namespace mbgl
