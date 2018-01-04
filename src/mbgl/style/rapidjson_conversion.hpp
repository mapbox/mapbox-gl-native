#pragma once

#include <mbgl/util/rapidjson.hpp>
#include <mbgl/style/conversion.hpp>

#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
class ConversionTraits<const JSValue*> {
public:
    static bool isUndefined(const JSValue* value) {
        return value->IsNull();
    }

    static bool isArray(const JSValue* value) {
        return value->IsArray();
    }

    static std::size_t arrayLength(const JSValue* value) {
        return value->Size();
    }

    static const JSValue* arrayMember(const JSValue* value, std::size_t i) {
        return &(*value)[rapidjson::SizeType(i)];
    }

    static bool isObject(const JSValue* value) {
        return value->IsObject();
    }

    static optional<const JSValue*> objectMember(const JSValue* value, const char * name) {
        if (!value->HasMember(name)) {
            return optional<const JSValue*>();
        }
        const JSValue* const& member = &(*value)[name];
        return {member};
    }

    template <class Fn>
    static optional<Error> eachMember(const JSValue* value, Fn&& fn) {
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

    static optional<bool> toBool(const JSValue* value) {
        if (!value->IsBool()) {
            return {};
        }
        return value->GetBool();
    }

    static optional<float> toNumber(const JSValue* value) {
        if (!value->IsNumber()) {
            return {};
        }
        return value->GetDouble();
    }

    static optional<double> toDouble(const JSValue* value) {
        if (!value->IsNumber()) {
            return {};
        }
        return value->GetDouble();
    }

    static optional<std::string> toString(const JSValue* value) {
        if (!value->IsString()) {
            return {};
        }
        return {{ value->GetString(), value->GetStringLength() }};
    }

    static optional<Value> toValue(const JSValue* value) {
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

    static optional<GeoJSON> toGeoJSON(const JSValue* value, Error& error) {
        try {
            return mapbox::geojson::convert(*value);
        } catch (const std::exception& ex) {
            error = { ex.what() };
            return {};
        }
    }
};

template <class T, class...Args>
optional<T> convert(const JSValue& value, Error& error, Args&&...args) {
    return convert<T>(Convertible(&value), error, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace style
} // namespace mbgl

