#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>

#include <string>
#include <unordered_map>

namespace mbgl {
namespace style {
namespace conversion {

class Value;
using ValueMap = std::unordered_map<std::string, Value>;
using ValueVector = std::vector<Value>;
class Value : public mbgl::variant<std::string,
                                   float,
                                   bool,
                                   mapbox::util::recursive_wrapper<ValueMap>,
                                   mapbox::util::recursive_wrapper<ValueVector>> {
    using variant::variant;
};

inline bool isUndefined(const Value&) {
    // Variant is always intialized
    return false;
}

inline bool isArray(const Value& value) {
    return value.is<mapbox::util::recursive_wrapper<ValueVector>>();
}

inline std::size_t arrayLength(const Value& value) {
    return value.get<mapbox::util::recursive_wrapper<ValueVector>>().get().size();
}

inline Value arrayMember(const Value& value, std::size_t i) {
    return value.get<mapbox::util::recursive_wrapper<ValueVector>>().get()[i];
}

inline bool isObject(const Value& value) {
    return value.is<mapbox::util::recursive_wrapper<ValueMap>>();
}

inline optional<Value> objectMember(const Value& value, const char* key) {
    auto map = value.get<ValueMap>();
    auto iter = map.find(key);

    if (iter != map.end()) {
        return iter->second;
    } else {
        return {};
    }
}

using EachMemberFn = std::function<optional<Error>(const std::string&, const Value&)>;

optional<Error> eachMember(const Value& value, EachMemberFn&& fn) {
    auto map = value.get<ValueMap>();
    auto iter = map.begin();

    while (iter != map.end()) {
        optional<Error> result = fn(iter->first, iter->second);
        if (result) {
            return result;
        }

        ++iter;
    }

    return {};
}

inline optional<bool> toBool(const Value& value) {
    if (value.is<bool>()) {
        return value.get<bool>();
    } else {
        return {};
    }
}

inline optional<float> toNumber(const Value& value) {
    if (value.is<float>()) {
        return value.get<float>();
    } else {
        return {};
    }
    return {};
}

inline optional<std::string> toString(const Value& value) {
    if (value.is<std::string>()) {
        return value.get<std::string>();
    } else {
        return {};
    }
}

inline optional<mbgl::Value> toValue(const Value& value) {
    if (value.is<bool>()) {
        return { value.get<bool>() };
    } else if (value.is<std::string>()) {
        return { value.get<std::string>() };
    } else if (value.is<float>()) {
        return { double(value.get<float>()) };
    } else {
        return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
