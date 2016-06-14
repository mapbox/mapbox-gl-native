#include <mbgl/style/property_value.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/enum.hpp>

namespace node_mbgl {

template <class V, class Enable = void>
struct ValueConverter {};

template <>
struct ValueConverter<bool> {
    mbgl::optional<mbgl::style::PropertyValue<bool>> operator()(const v8::Local<v8::Value>& value) const {
        if (!value->IsBoolean()) {
            Nan::ThrowTypeError("boolean required");
            return {};
        }

        return { value->BooleanValue() };
    }
};

template <>
struct ValueConverter<float> {
    mbgl::optional<mbgl::style::PropertyValue<float>> operator()(const v8::Local<v8::Value>& value) const {
        if (!value->IsNumber()) {
            Nan::ThrowTypeError("number required");
            return {};
        }

        return { float(value->NumberValue()) };
    }
};

template <>
struct ValueConverter<std::string> {
    mbgl::optional<mbgl::style::PropertyValue<std::string>> operator()(const v8::Local<v8::Value>& value) const {
        if (!value->IsString()) {
            Nan::ThrowTypeError("string required");
            return {};
        }

        return { std::string(*Nan::Utf8String(value)) };
    }
};

template <typename T>
struct ValueConverter<T, std::enable_if_t<std::is_enum<T>::value>> {
    mbgl::optional<mbgl::style::PropertyValue<T>> operator()(const v8::Local<v8::Value>& value) const {
        if (!value->IsString()) {
            Nan::ThrowTypeError("string required");
            return {};
        }

        mbgl::optional<T> result = mbgl::Enum<T>::toEnum(*Nan::Utf8String(value));
        if (!result) {
            Nan::ThrowTypeError("invalid enumeration value");
            return {};
        }

        return { *result };
    }
};

template <>
struct ValueConverter<mbgl::Color> {
    mbgl::optional<mbgl::style::PropertyValue<mbgl::Color>> operator()(const v8::Local<v8::Value>& value) const {
        (void)value;
        return {};
    }
};

template <>
struct ValueConverter<std::array<float, 2>> {
    mbgl::optional<mbgl::style::PropertyValue<std::array<float, 2>>> operator()(const v8::Local<v8::Value>& value) const {
        (void)value;
        return {};
    }
};

template <>
struct ValueConverter<std::vector<float>> {
    mbgl::optional<mbgl::style::PropertyValue<std::vector<float>>> operator()(const v8::Local<v8::Value>& value) const {
        (void)value;
        return {};
    }
};

template <>
struct ValueConverter<std::vector<std::string>> {
    mbgl::optional<mbgl::style::PropertyValue<std::vector<std::string>>> operator()(const v8::Local<v8::Value>& value) const {
        (void)value;
        return {};
    }
};

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

        mbgl::optional<mbgl::style::PropertyValue<V>> typedValue;

        if (value->IsNull() || value->IsUndefined()) {
            typedValue = mbgl::style::PropertyValue<V>();
        } else {
            typedValue = ValueConverter<V>()(value);
        }

        if (!typedValue) {
            return false;
        }

        (typedLayer->*setter)(*typedValue);
        return true;
    };
}

}
