#pragma once

#include <mbgl/util/rapidjson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

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

} // namespace conversion
} // namespace style
} // namespace mbgl
