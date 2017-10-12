#include "node_conversion.hpp"
#include <mbgl/style/conversion/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

static const v8::Local<v8::Value>& cast(const Storage& storage) {
    return *static_cast<const v8::Local<v8::Value>*>(static_cast<const void*>(&storage));
}

static v8::Local<v8::Value>& cast(Storage& storage) {
    return *static_cast<v8::Local<v8::Value>*>(static_cast<void*>(&storage));
}

static void destroy(Storage& storage) {
    const v8::Local<v8::Value> value(std::move(cast(storage)));
    (void)value; // appease linter
}

static void move(Storage&& src, Storage& dest) {
    new (static_cast<void*>(&dest)) const v8::Local<v8::Value> (std::move(cast(src)));
    destroy(src);
}

inline bool isUndefined(const Storage& storage) {
    Nan::HandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    return value->IsUndefined() || value->IsNull();
}

inline bool isArray(const Storage& storage) {
    Nan::HandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    return value->IsArray();
}

inline std::size_t arrayLength(const Storage& storage) {
    Nan::HandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    return value.As<v8::Array>()->Length();
}

inline Value arrayMember(const Storage& storage, std::size_t i) {
    Nan::EscapableHandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    return makeValue(scope.Escape(Nan::Get(value.As<v8::Array>(), i).ToLocalChecked()));
}

inline bool isObject(const Storage& storage) {
    Nan::HandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    return value->IsObject() && !value->IsArray();
}

inline optional<Value> objectMember(const Storage& storage, const char * name) {
    Nan::EscapableHandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    if (!Nan::Has(Nan::To<v8::Object>(value).ToLocalChecked(), Nan::New(name).ToLocalChecked()).FromJust()) {
        return {};
    }
    Nan::MaybeLocal<v8::Value> result = Nan::Get(Nan::To<v8::Object>(value).ToLocalChecked(), Nan::New(name).ToLocalChecked());
    if (result.IsEmpty()) {
        return {};
    }
    return makeValue(scope.Escape(result.ToLocalChecked()));
}

optional<Error> eachMember(const Storage& storage, const std::function<optional<Error> (const std::string&, const Value&)>& fn) {
    Nan::HandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    v8::Local<v8::Array> names = Nan::GetOwnPropertyNames(Nan::To<v8::Object>(value).ToLocalChecked()).ToLocalChecked();
    for (uint32_t i = 0; i < names->Length(); ++i) {
        v8::Local<v8::Value> k = Nan::Get(names, i).ToLocalChecked();
        v8::Local<v8::Value> v = Nan::Get(Nan::To<v8::Object>(value).ToLocalChecked(), k).ToLocalChecked();
        optional<Error> result = fn(*Nan::Utf8String(k), makeValue(v));
        if (result) {
            return result;
        }
    }
    return {};
}

inline optional<bool> toBool(const Storage& storage) {
    Nan::HandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    if (!value->IsBoolean()) {
        return {};
    }
    return value->BooleanValue();
}

inline optional<float> toNumber(const Storage& storage) {
    Nan::HandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    if (!value->IsNumber()) {
        return {};
    }
    return value->NumberValue();
}

inline optional<double> toDouble(const Storage& storage) {
    Nan::HandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    if (!value->IsNumber()) {
        return {};
    }
    return value->NumberValue();
}

inline optional<std::string> toString(const Storage& storage) {
    Nan::HandleScope scope;
    const v8::Local<v8::Value>& value = cast(storage);
    if (!value->IsString()) {
        return {};
    }
    return std::string(*Nan::Utf8String(value));
}

inline optional<mbgl::Value> toValue(const Storage& storage) {
    const v8::Local<v8::Value>& value = cast(storage);
    if (value->IsFalse()) {
        return { false };
    } else if (value->IsTrue()) {
        return { true };
    } else if (value->IsString()) {
        return { std::string(*Nan::Utf8String(value)) };
    } else if (value->IsUint32()) {
        return { std::uint64_t(value->Uint32Value()) };
    } else if (value->IsInt32()) {
        return { std::int64_t(value->Int32Value()) };
    } else if (value->IsNumber()) {
        return { value->NumberValue() };
    } else {
        return {};
    }
}

static optional<GeoJSON> toGeoJSON(const Storage& storage, Error& error) {
    try {
        Nan::JSON JSON;
        const v8::Local<v8::Value>& value = cast(storage);
        std::string string = *Nan::Utf8String(JSON.Stringify(value->ToObject()).ToLocalChecked());
        return parseGeoJSON(string, error);
    } catch (const std::exception& ex) {
        error = { ex.what() };
        return {};
    }
}

Value makeValue(const v8::Local<v8::Value> value) {
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
    new (static_cast<void*>(&storage)) const v8::Local<v8::Value> (value);
    return Value(&vtable, std::move(storage));
}

} // namespace conversion
} // namespace style
} // namespace mbgl
