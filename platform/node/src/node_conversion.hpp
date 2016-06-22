#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <mbgl/util/optional.hpp>
#include <mbgl/util/feature.hpp>

namespace mbgl {
namespace style {
namespace conversion {

inline bool isArray(v8::Local<v8::Value> value) {
    Nan::HandleScope scope;
    return value->IsArray();
}

inline std::size_t arrayLength(v8::Local<v8::Value> value) {
    Nan::HandleScope scope;
    return value.As<v8::Array>()->Length();
}

inline v8::Local<v8::Value> arrayMember(v8::Local<v8::Value> value, std::size_t i) {
    Nan::EscapableHandleScope scope;
    return scope.Escape(Nan::Get(value.As<v8::Array>(), i).ToLocalChecked());
}

inline bool isObject(v8::Local<v8::Value> value) {
    Nan::HandleScope scope;
    return value->IsObject();
}

inline optional<v8::Local<v8::Value>> objectMember(v8::Local<v8::Value> value, const char * name) {
    Nan::EscapableHandleScope scope;
    if (!Nan::Has(value.As<v8::Array>(), Nan::New(name).ToLocalChecked()).FromJust()) {
        return {};
    }
    Nan::MaybeLocal<v8::Value> result = Nan::Get(value.As<v8::Array>(), Nan::New(name).ToLocalChecked());
    if (result.IsEmpty()) {
        return {};
    }
    return scope.Escape(result.ToLocalChecked());
}

inline optional<bool> toBool(v8::Local<v8::Value> value) {
    Nan::HandleScope scope;
    if (!value->IsBoolean()) {
        return {};
    }
    return value->BooleanValue();
}

inline optional<float> toNumber(v8::Local<v8::Value> value) {
    Nan::HandleScope scope;
    if (!value->IsNumber()) {
        return {};
    }
    return value->NumberValue();
}

inline optional<std::string> toString(v8::Local<v8::Value> value) {
    Nan::HandleScope scope;
    if (!value->IsString()) {
        return {};
    }
    return std::string(*Nan::Utf8String(value));
}

inline optional<Value> toValue(v8::Local<v8::Value> value) {
    if (value->IsFalse()) {
        return { false };
    } else if (value->IsTrue()) {
        return { true };
    } else if (value->IsString()) {
        return { std::string(*Nan::Utf8String(value)) };
    } else if (value->IsUint32()) {
        return { value->Uint32Value() };
    } else if (value->IsInt32()) {
        return { value->Int32Value() };
    } else if (value->IsNumber()) {
        return { value->NumberValue() };
    } else {
        return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
