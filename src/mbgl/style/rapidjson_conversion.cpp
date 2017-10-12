#include <mbgl/style/rapidjson_conversion.hpp>

#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

static const JSValue& cast(const Storage& storage) {
    return **static_cast<const JSValue* const*>(static_cast<const void*>(&storage));
}

static void destroy(Storage&) {
    return;
}

static void move(Storage&& src, Storage& dest) {
    new (static_cast<void*>(&dest)) JSValue* (std::move(*static_cast<JSValue**>(static_cast<void*>(&src))));
    destroy(src);
}

static bool isUndefined(const Storage& storage) {
    return cast(storage).IsNull();
}

static bool isArray(const Storage& storage) {
    return cast(storage).IsArray();
}

static std::size_t arrayLength(const Storage& storage) {
    return cast(storage).Size();
}

static Value arrayMember(const Storage& storage, std::size_t i) {
    return makeValue(&cast(storage)[rapidjson::SizeType(i)]);
}

static bool isObject(const Storage& storage) {
    return cast(storage).IsObject();
}

static optional<Value> objectMember(const Storage& storage, const char * name) {
    const JSValue& value = cast(storage);
    if (!value.HasMember(name)) {
        return optional<Value>();
    }
    return makeValue(&value[name]);
}

static optional<Error> eachMember(const Storage& storage, const std::function<optional<Error> (const std::string&, const Value&)>& fn) {
    const JSValue& value = cast(storage);
    assert(value.IsObject());
    for (const auto& property : value.GetObject()) {
        optional<Error> result =
            fn({ property.name.GetString(), property.name.GetStringLength() }, makeValue(&property.value));
        if (result) {
            return result;
        }
    }
    return {};
}

static optional<bool> toBool(const Storage& storage) {
    const JSValue& value = cast(storage);
    if (!value.IsBool()) {
        return {};
    }
    return value.GetBool();
}

static optional<float> toNumber(const Storage& storage) {
    const JSValue& value = cast(storage);
    if (!value.IsNumber()) {
        return {};
    }
    return value.GetDouble();
}

static optional<double> toDouble(const Storage& storage) {
    const JSValue& value = cast(storage);
    if (!value.IsNumber()) {
        return {};
    }
    return value.GetDouble();
}

static optional<std::string> toString(const Storage& storage) {
    const JSValue& value = cast(storage);
    if (!value.IsString()) {
        return {};
    }
    return {{ value.GetString(), value.GetStringLength() }};
}

static optional<mbgl::Value> toValue(const Storage& storage) {
    const JSValue& value = cast(storage);
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

static optional<GeoJSON> toGeoJSON(const Storage& storage, Error& error) {
    try {
        return mapbox::geojson::convert(cast(storage));
    } catch (const std::exception& ex) {
        error = { ex.what() };
        return {};
    }
}

Value makeValue(const JSValue* value) {
    static Value::VTable vtable = {
        move,
        destroy,
        isUndefined,
        isArray,
        arrayLength,
        arrayMember,
        isObject,
        objectMember,
        eachMember,
        toBool,
        toNumber,
        toDouble,
        toString,
        toValue,
        toGeoJSON
    };

    Storage storage;
    new (static_cast<void*>(&storage)) const JSValue* (value);
    return Value(&vtable, std::move(storage));
}

} // namespace conversion
} // namespace style
} // namespace mbgl
