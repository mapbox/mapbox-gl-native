#pragma once

#include "value.hpp"

#include <mbgl/util/feature.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/conversion_impl.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
class ConversionTraits<mbgl::android::Value> {
public:
    static bool isUndefined(const mbgl::android::Value& value) {
        return value.isNull();
    }

    static bool isArray(const mbgl::android::Value& value) {
        return value.isArray();
    }

    static bool isObject(const mbgl::android::Value& value) {
        return value.isObject();
    }

    static std::size_t arrayLength(const mbgl::android::Value& value) {
        return value.getLength();;
    }

    static mbgl::android::Value arrayMember(const mbgl::android::Value& value, std::size_t i) {
        return value.get(i);
    }

    static optional<mbgl::android::Value> objectMember(const mbgl::android::Value& value, const char* key) {
        mbgl::android::Value member = value.get(key);
        if (!member.isNull()) {
            return member;
        } else {
            return {};
        }
    }

    template <class Fn>
    static optional<Error> eachMember(const mbgl::android::Value& value, Fn&& fn) {
        assert(value.isObject());
        mbgl::android::Value keys = value.keyArray();
        std::size_t length = arrayLength(keys);
        for(std::size_t i = 0; i < length; ++i){
            const auto k = keys.get(i).toString();
            auto v = value.get(k.c_str());
            optional<Error> result = fn(k, std::move(v));
            if (result) {
                return result;
            }
        }
        return {};
    }

    static optional<bool> toBool(const mbgl::android::Value& value) {
        if (value.isBool()) {
            return value.toBool();
        } else {
            return {};
        }
    }

    static optional<float> toNumber(const mbgl::android::Value& value) {
        if (value.isNumber()) {
            auto num = value.toFloat();
            return num;
        } else {
            return {};
        }
    }

    static optional<double> toDouble(const mbgl::android::Value& value) {
        if (value.isNumber()) {
            return value.toDouble();
        } else {
            return {};
        }
    }

    static optional<std::string> toString(const mbgl::android::Value& value) {
        if (value.isString()) {
            return value.toString();
        } else {
            return {};
        }
    }

    static optional<Value> toValue(const mbgl::android::Value& value) {
        if (value.isNull()) {
            return {};
        } else if (value.isBool()) {
            return { value.toBool() };
        } else if (value.isString()) {
            return { value.toString() };
        } else if (value.isNumber()) {
            return { value.toDouble() };
        } else {
            return {};
        }
    }

    static optional<GeoJSON> toGeoJSON(const mbgl::android::Value &value, Error &error) {
        if (value.isNull() || !value.isString()) {
            error = { "no json data found" };
            return {};
        }
        return parseGeoJSON(value.toString(), error);
    }
};

template <class T, class...Args>
optional<T> convert(mbgl::android::Value&& value, Error& error, Args&&...args) {
    return convert<T>(Convertible(std::move(value)), error, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
