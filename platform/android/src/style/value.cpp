#include "value.hpp"

#include "../java_types.hpp"

namespace mbgl {
namespace android {
    // Instance

    Value::Value(jni::JNIEnv& _env, jni::Local<jni::Object<>> _value)
        : env(_env),
          value(std::move(_value)) {}

    bool Value::isNull() const {
        return !value;
    }

    bool Value::isArray() const {
        return jni::IsInstanceOf(env, value->Get(), *java::ObjectArray::jclass);
    }

    bool Value::isObject() const {
        return jni::IsInstanceOf(env, value->Get(), *java::Map::jclass);
    }

    bool Value::isString() const {
        return jni::IsInstanceOf(env, value->Get(), *java::String::jclass);
    }

    bool Value::isBool() const {
        return jni::IsInstanceOf(env, value->Get(), *java::Boolean::jclass);
    }

    bool Value::isNumber() const {
        return jni::IsInstanceOf(env, value->Get(), *java::Number::jclass);
    }

    std::string Value::toString() const {
        jni::jstring* string = reinterpret_cast<jni::jstring*>(value->Get());
        return jni::Make<std::string>(env, jni::String(string));
    }

    float Value::toFloat() const {
        return jni::CallMethod<jni::jfloat>(env, value->Get(), *java::Number::floatValueMethodId);
    }

    double Value::toDouble() const {
        return jni::CallMethod<jni::jdouble>(env, value->Get(), *java::Number::doubleValueMethodId);
    }

    long Value::toLong() const {
        return jni::CallMethod<jni::jlong>(env, value->Get(), *java::Number::longValueMethodId);
    }

    bool Value::toBool() const {
        return jni::CallMethod<jni::jboolean>(env, value->Get(), *java::Boolean::booleanValueMethodId);
    }

    Value Value::get(const char* key) const {
        jni::jobject* member = jni::CallMethod<jni::jobject*>(env, value->Get(), *java::Map::getMethodId, jni::Make<jni::String>(env, std::string(key)).Get());
        return Value(env, jni::SeizeLocal(env, jni::Object<>(member)));
    }

    int Value::getLength() const {
        auto array = (jni::jarray<jni::jobject>*) value->Get();
        return jni::GetArrayLength(env, *array);
    }

    Value Value::get(const int index) const {
        auto array = (jni::jarray<jni::jobject>*) value->Get();
        return Value(env, jni::SeizeLocal(env, jni::Object<>(jni::GetObjectArrayElement(env, *array, index))));
    }
}
}

