#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <mbgl/util/optional.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
class ConversionTraits<v8::Local<v8::Value>> {
public:
    static bool isUndefined(const v8::Local<v8::Value>& value) {
        Nan::HandleScope scope;
        return value->IsUndefined() || value->IsNull();
    }

    static bool isArray(const v8::Local<v8::Value>& value) {
        Nan::HandleScope scope;
        return value->IsArray();
    }

    static std::size_t arrayLength(const v8::Local<v8::Value>& value) {
        Nan::HandleScope scope;
        // const_cast because v8::Local<T>::As is not marked const until node v8.0
        v8::Local<v8::Array> array = const_cast<v8::Local<v8::Value>&>(value).As<v8::Array>();
        return array->Length();
    }

    static v8::Local<v8::Value> arrayMember(const v8::Local<v8::Value>& value, std::size_t i) {
        Nan::EscapableHandleScope scope;
        // const_cast because v8::Local<T>::As is not marked const until node v8.0
        v8::Local<v8::Array> array = const_cast<v8::Local<v8::Value>&>(value).As<v8::Array>();
        return scope.Escape(Nan::Get(array, i).ToLocalChecked());
    }

    static bool isObject(const v8::Local<v8::Value>& value) {
        Nan::HandleScope scope;
        return value->IsObject() && !value->IsArray();
    }

    static optional<v8::Local<v8::Value>> objectMember(const v8::Local<v8::Value>& value, const char * name) {
        Nan::EscapableHandleScope scope;
        if (!Nan::Has(Nan::To<v8::Object>(value).ToLocalChecked(), Nan::New(name).ToLocalChecked()).FromJust()) {
            return {};
        }
        Nan::MaybeLocal<v8::Value> result = Nan::Get(Nan::To<v8::Object>(value).ToLocalChecked(), Nan::New(name).ToLocalChecked());
        if (result.IsEmpty()) {
            return {};
        }
        return {scope.Escape(result.ToLocalChecked())};
    }

    template <class Fn>
    static optional<Error> eachMember(const v8::Local<v8::Value>& value, Fn&& fn) {
        Nan::HandleScope scope;
        v8::Local<v8::Array> names = Nan::GetOwnPropertyNames(Nan::To<v8::Object>(value).ToLocalChecked()).ToLocalChecked();
        for (uint32_t i = 0; i < names->Length(); ++i) {
            v8::Local<v8::Value> k = Nan::Get(names, i).ToLocalChecked();
            v8::Local<v8::Value> v = Nan::Get(Nan::To<v8::Object>(value).ToLocalChecked(), k).ToLocalChecked();
            optional<Error> result = fn(*Nan::Utf8String(k), std::move(v));
            if (result) {
                return result;
            }
        }
        return {};
    }

    static optional<bool> toBool(const v8::Local<v8::Value>& value) {
        Nan::HandleScope scope;
        if (!value->IsBoolean()) {
            return {};
        }
        return value->BooleanValue();
    }

    static optional<float> toNumber(const v8::Local<v8::Value>& value) {
        Nan::HandleScope scope;
        if (!value->IsNumber()) {
            return {};
        }
        return value->NumberValue();
    }

    static optional<double> toDouble(const v8::Local<v8::Value>& value) {
        Nan::HandleScope scope;
        if (!value->IsNumber()) {
            return {};
        }
        return value->NumberValue();
    }

    static optional<std::string> toString(const v8::Local<v8::Value>& value) {
        Nan::HandleScope scope;
        if (!value->IsString()) {
            return {};
        }
        return std::string(*Nan::Utf8String(value));
    }

    static optional<Value> toValue(const v8::Local<v8::Value>& value) {
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

    static optional<GeoJSON> toGeoJSON(const v8::Local<v8::Value>& value, Error& error) {
        try {
            Nan::JSON JSON;
            std::string string = *Nan::Utf8String(JSON.Stringify(value->ToObject()).ToLocalChecked());
            return parseGeoJSON(string, error);
        } catch (const std::exception& ex) {
            error = { ex.what() };
            return {};
        }
    }
};

template <class T, class...Args>
optional<T> convert(const v8::Local<v8::Value>& value, Error& error, Args&&...args) {
    return convert<T>(Convertible(value), error, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
