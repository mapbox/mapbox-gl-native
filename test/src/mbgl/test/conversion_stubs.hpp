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

class Holder;
using HolderMap = std::unordered_map<std::string, Holder>;
using HolderVector = std::vector<Holder>;
class Holder : public mbgl::variant<std::string, float, bool, HolderMap, HolderVector> {
     using variant::variant;
};

inline bool isUndefined(const Holder&) {
    //Variant is always intialized
    return false;
}

inline bool isArray(const Holder& value) {
    return value.is<HolderVector>();
}

inline std::size_t arrayLength(const Holder& value) {
    return value.get<HolderVector>().size();
}

inline Holder arrayMember(const Holder& value, std::size_t i) {
    return value.get<HolderVector>()[i];
}

inline bool isObject(const Holder& value) {
    return value.is<HolderMap>();
}

inline optional<Holder> objectMember(const Holder& value, const char* key) {
    auto map = value.get<HolderMap>();
    auto iter = map.find(key);

    if (iter != map.end()) {
        return iter->second;
    } else {
        return {};
    }
}

using EachMemberFn = std::function<optional<Error>(const std::string&, const Holder&)>;

optional<Error> eachMember(const Holder& value, EachMemberFn&& fn) {
    auto map = value.get<HolderMap>();
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

inline optional<bool> toBool(const Holder& value) {
    if (value.is<bool>()) {
        return value.get<bool>();
    } else {
        return {};
    }
}

inline optional<float> toNumber(const Holder& value) {
    if (value.is<float>()) {
        return value.get<float>();
    } else {
        return {};
    }
    return {};
}

inline optional<std::string> toString(const Holder& value) {
    if (value.is<std::string>()) {
        return value.get<std::string>();
    } else {
        return {};
    }
}

inline optional<Value> toValue(const Holder& value) {
    if (value.is<bool>()) {
        return { value.get<bool>() };
    } else if (value.is<std::string>()) {
        return { value.get<std::string>() };
    } else if (value.is<float>()) {
        return { value.get<float>() };
    } else {
        return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
