#include <mbgl/style/rapidjson_conversion.hpp>
#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>


namespace mbgl {
namespace style {
namespace conversion {

using JSValuePointer = const JSValue*;

template<> bool ValueTraits<JSValuePointer>::isUndefined(const JSValuePointer& value) {
    return value->IsNull();
}

template<> bool ValueTraits<JSValuePointer>::isArray(const JSValuePointer& value) {
    return value->IsArray();
}

template<> std::size_t ValueTraits<JSValuePointer>::arrayLength(const JSValuePointer& value) {
    return value->Size();
}

template<> JSValuePointer ValueTraits<JSValuePointer>::arrayMember(const JSValuePointer& value, std::size_t i) {
    return &(*value)[rapidjson::SizeType(i)];
}

template<> bool ValueTraits<JSValuePointer>::isObject(const JSValuePointer& value) {
    return value->IsObject();
}

template<> optional<JSValuePointer> ValueTraits<JSValuePointer>::objectMember(const JSValuePointer& value, const char * name) {
    if (!value->HasMember(name)) {
        return optional<JSValuePointer>();
    }
    const JSValuePointer& member = &(*value)[name];
    return {member};
}

template<> optional<Error> ValueTraits<JSValuePointer>::eachMember(const JSValuePointer& value, const std::function<optional<Error> (const std::string&, const JSValuePointer&)>& fn) {
    assert(value->IsObject());
    for (const auto& property : value->GetObject()) {
        optional<Error> result =
            fn({ property.name.GetString(), property.name.GetStringLength() }, &property.value);
        if (result) {
            return result;
        }
    }
    return {};
}

template<> optional<bool> ValueTraits<JSValuePointer>::toBool(const JSValuePointer& value) {
    if (!value->IsBool()) {
        return {};
    }
    return value->GetBool();
}

template<> optional<float> ValueTraits<JSValuePointer>::toNumber(const JSValuePointer& value) {
    if (!value->IsNumber()) {
        return {};
    }
    return value->GetDouble();
}

template<> optional<double> ValueTraits<JSValuePointer>::toDouble(const JSValuePointer& value) {
    if (!value->IsNumber()) {
        return {};
    }
    return value->GetDouble();
}

template<> optional<std::string> ValueTraits<JSValuePointer>::toString(const JSValuePointer& value) {
    if (!value->IsString()) {
        return {};
    }
    return {{ value->GetString(), value->GetStringLength() }};
}

template<> optional<mbgl::Value> ValueTraits<JSValuePointer>::toValue(const JSValuePointer& value) {
    switch (value->GetType()) {
        case rapidjson::kNullType:
        case rapidjson::kFalseType:
            return { false };

        case rapidjson::kTrueType:
            return { true };

        case rapidjson::kStringType:
            return { std::string { value->GetString(), value->GetStringLength() } };

        case rapidjson::kNumberType:
            if (value->IsUint64()) return { value->GetUint64() };
            if (value->IsInt64()) return { value->GetInt64() };
            return { value->GetDouble() };

        default:
            return {};
    }
}

template<> optional<GeoJSON> ValueTraits<JSValuePointer>::toGeoJSON(const JSValuePointer& value, Error& error) {
    try {
        return mapbox::geojson::convert(*value);
    } catch (const std::exception& ex) {
        error = { ex.what() };
        return {};
    }
}


} // namespace conversion
} // namespace style
} // namespace mbgl
