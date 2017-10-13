#include "android_conversion.hpp"
#include <mbgl/style/conversion/geojson.hpp>


namespace mbgl {
namespace style {
namespace conversion {

using AndroidValue = mbgl::android::Value;

template<> bool ValueTraits<AndroidValue>::isUndefined(const AndroidValue& value) {
    return value.isNull();
}

template<> bool ValueTraits<AndroidValue>::isArray(const AndroidValue& value) {
    return value.isArray();
}

template<> bool ValueTraits<AndroidValue>::isObject(const AndroidValue& value) {
    return value.isObject();
}

template<> std::size_t ValueTraits<AndroidValue>::arrayLength(const AndroidValue& value) {
    return value.getLength();;
}

template<> AndroidValue ValueTraits<AndroidValue>::arrayMember(const AndroidValue& value, std::size_t i) {
    return value.get(i);
}

template<> optional<AndroidValue> ValueTraits<AndroidValue>::objectMember(const AndroidValue& value, const char* key) {
    AndroidValue member = value.get(key);

    if (!member.isNull()) {
        return member;
    } else {
        return {};
    }
}

template<> optional<Error> ValueTraits<AndroidValue>::eachMember(const AndroidValue&, const std::function<optional<Error> (const std::string&, const AndroidValue&)>&) {
    // TODO
    mbgl::Log::Warning(mbgl::Event::Android, "eachMember not implemented");
    return {};
}

template<> optional<bool> ValueTraits<AndroidValue>::toBool(const AndroidValue& value) {
    if (value.isBool()) {
        return value.toBool();
    } else {
        return {};
    }
}

template<> optional<float> ValueTraits<AndroidValue>::toNumber(const AndroidValue& value) {
    if (value.isNumber()) {
        auto num = value.toFloat();
        return num;
    } else {
        return {};
    }
}

template<> optional<double> ValueTraits<AndroidValue>::toDouble(const AndroidValue& value) {
    if (value.isNumber()) {
        return value.toDouble();
    } else {
        return {};
    }
}

template<> optional<std::string> ValueTraits<AndroidValue>::toString(const AndroidValue& value) {
    if (value.isString()) {
        return value.toString();
    } else {
        return {};
    }
}

template<> optional<mbgl::Value> ValueTraits<AndroidValue>::toValue(const AndroidValue& value) {
    if (value.isNull()) {
        return {};
    } else if (value.isBool()) {
        return { value.toBool() };
    } else if (value.isString()) {
        return { value.toString() };
    } else if (value.isNumber()) {
        auto doubleVal = value.toDouble();
        return { doubleVal - (int) doubleVal > 0.0 ? doubleVal : value.toLong() };
    } else {
        return {};
    }
}

template<> optional<GeoJSON> ValueTraits<AndroidValue>::toGeoJSON(const AndroidValue &value, Error &error) {
    if(value.isNull() || !value.isString()) {
        error = { "no json data found" };
        return {};
    }

    return parseGeoJSON(value.toString(), error);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
