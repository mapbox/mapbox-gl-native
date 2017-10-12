#include "node_conversion.hpp"
#include <mbgl/style/conversion/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

using V8Value = v8::Local<v8::Value>;

template<> bool ValueTraits<V8Value>::isUndefined(const V8Value& value) {
    Nan::HandleScope scope;
    
    return value->IsUndefined() || value->IsNull();
}

template<> bool ValueTraits<V8Value>::isArray(const V8Value& value) {
    Nan::HandleScope scope;
    
    return value->IsArray();
}

template<> std::size_t ValueTraits<V8Value>::arrayLength(const V8Value& value) {
    Nan::HandleScope scope;
    
    return value.As<v8::Array>()->Length();
}

template<> V8Value ValueTraits<V8Value>::arrayMember(const V8Value& value, std::size_t i) {
    Nan::EscapableHandleScope scope;
    
    return scope.Escape(Nan::Get(value.As<v8::Array>(), i).ToLocalChecked());
}

template<> bool ValueTraits<V8Value>::isObject(const V8Value& value) {
    Nan::HandleScope scope;
    
    return value->IsObject() && !value->IsArray();
}

template<> optional<V8Value> ValueTraits<V8Value>::objectMember(const V8Value& value, const char * name) {
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

template<> optional<Error> ValueTraits<V8Value>::eachMember(const V8Value& value, const std::function<optional<Error> (const std::string&, const V8Value&)>& fn) {
    Nan::HandleScope scope;
    
    v8::Local<v8::Array> names = Nan::GetOwnPropertyNames(Nan::To<v8::Object>(value).ToLocalChecked()).ToLocalChecked();
    for (uint32_t i = 0; i < names->Length(); ++i) {
        v8::Local<v8::Value> k = Nan::Get(names, i).ToLocalChecked();
        v8::Local<v8::Value> v = Nan::Get(Nan::To<v8::Object>(value).ToLocalChecked(), k).ToLocalChecked();
        optional<Error> result = fn(*Nan::Utf8String(k), v);
        if (result) {
            return result;
        }
    }
    return {};
}

template<> optional<bool> ValueTraits<V8Value>::toBool(const V8Value& value) {
    Nan::HandleScope scope;
    
    if (!value->IsBoolean()) {
        return {};
    }
    return value->BooleanValue();
}

template<> optional<float> ValueTraits<V8Value>::toNumber(const V8Value& value) {
    Nan::HandleScope scope;
    
    if (!value->IsNumber()) {
        return {};
    }
    return value->NumberValue();
}

template<> optional<double> ValueTraits<V8Value>::toDouble(const V8Value& value) {
    Nan::HandleScope scope;
    
    if (!value->IsNumber()) {
        return {};
    }
    return value->NumberValue();
}

template<> optional<std::string> ValueTraits<V8Value>::toString(const V8Value& value) {
    Nan::HandleScope scope;
    
    if (!value->IsString()) {
        return {};
    }
    return std::string(*Nan::Utf8String(value));
}

template<> optional<mbgl::Value> ValueTraits<V8Value>::toValue(const V8Value& value) {
    
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

template<> optional<GeoJSON> ValueTraits<V8Value>::toGeoJSON(const V8Value& value, Error& error) {
    try {
        Nan::JSON JSON;
        
        std::string string = *Nan::Utf8String(JSON.Stringify(value->ToObject()).ToLocalChecked());
        return parseGeoJSON(string, error);
    } catch (const std::exception& ex) {
        error = { ex.what() };
        return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
