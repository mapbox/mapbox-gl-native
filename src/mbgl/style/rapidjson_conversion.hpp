#pragma once

#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

inline bool isUndefined(const JSValue& value) {
    return value.IsNull();
}

inline bool isArray(const JSValue& value) {
    return value.IsArray();
}

inline std::size_t arrayLength(const JSValue& value) {
    return value.Size();
}

inline const JSValue& arrayMember(const JSValue& value, std::size_t i) {
    return value[rapidjson::SizeType(i)];
}

inline bool isObject(const JSValue& value) {
    return value.IsObject();
}

inline const JSValue* objectMember(const JSValue& value, const char * name) {
    if (!value.HasMember(name)) {
        return nullptr;
    }
    return &value[name];
}

template <class Fn>
optional<Error> eachMember(const JSValue& value, Fn&& fn) {
    assert(value.IsObject());
    for (const auto& property : value.GetObject()) {
        optional<Error> result =
            fn({ property.name.GetString(), property.name.GetStringLength() }, property.value);
        if (result) {
            return result;
        }
    }
    return {};
}

inline optional<bool> toBool(const JSValue& value) {
    if (!value.IsBool()) {
        return {};
    }
    return value.GetBool();
}

inline optional<float> toNumber(const JSValue& value) {
    if (!value.IsNumber()) {
        return {};
    }
    return value.GetDouble();
}

inline optional<std::string> toString(const JSValue& value) {
    if (!value.IsString()) {
        return {};
    }
    return {{ value.GetString(), value.GetStringLength() }};
}

inline optional<Value> toValue(const JSValue& value) {
    switch (value.GetType()) {
        case rapidjson::kNullType:
        case rapidjson::kFalseType:
            return { false };

        case rapidjson::kTrueType:
            return { true };

        case rapidjson::kStringType:
            return { std::string { value.GetString(), value.GetStringLength() } };

        case rapidjson::kNumberType:
            if (value.IsUint64()) return { value.GetUint64() };
            if (value.IsInt64()) return { value.GetInt64() };
            return { value.GetDouble() };

        default:
            return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
