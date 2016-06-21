#pragma once

#include "node_conversion.hpp"

#include <mbgl/style/layer.hpp>
#include <mbgl/style/conversion.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <functional>
#include <string>
#include <unordered_map>

namespace node_mbgl {

using PropertySetter = std::function<bool (mbgl::style::Layer&, const v8::Local<v8::Value>&)>;
using PropertySetters = std::unordered_map<std::string, PropertySetter>;

template <class L, class V>
PropertySetter makePropertySetter(void (L::*setter)(mbgl::style::PropertyValue<V>)) {
    return [setter] (mbgl::style::Layer& layer, const v8::Local<v8::Value>& value) {
        L* typedLayer = layer.as<L>();
        if (!typedLayer) {
            Nan::ThrowTypeError("layer doesn't support this property");
            return false;
        }

        if (value->IsNull() || value->IsUndefined()) {
            (typedLayer->*setter)(mbgl::style::PropertyValue<V>());
            return true;
        }

        mbgl::style::conversion::Result<mbgl::style::PropertyValue<V>> typedValue
            = mbgl::style::conversion::convertPropertyValue<V>(value);
        if (typedValue.template is<mbgl::style::conversion::Error>()) {
            Nan::ThrowTypeError(typedValue.template get<mbgl::style::conversion::Error>().message);
            return false;
        }

        (typedLayer->*setter)(typedValue.template get<mbgl::style::PropertyValue<V>>());
        return true;
    };
}

inline bool setVisibility(mbgl::style::Layer& layer, const v8::Local<v8::Value>& value) {
    if (value->IsNull() || value->IsUndefined()) {
        layer.setVisibility(mbgl::style::VisibilityType::Visible);
        return true;
    }

    if (!value->IsString()) {
        Nan::ThrowTypeError("visibility value must be \"visible\" or \"none\"");
        return false;
    }

    layer.setVisibility(std::string(*Nan::Utf8String(value)) == "none"
        ? mbgl::style::VisibilityType::None
        : mbgl::style::VisibilityType::Visible);

   return true;
}

}
