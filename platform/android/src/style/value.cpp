#include "value.hpp"

#include "../java_types.hpp"

namespace mbgl {
namespace android {
    // Instance

    Value::Value(jni::JNIEnv& _env, jni::Local<jni::Object<>> _value)
        : env(_env),
          value(std::move(_value)) {}

    Value::Value(jni::JNIEnv& _env, const jni::Object<>& _value)
        : env(_env),
          value(jni::NewLocal(_env, _value)) {}

    bool Value::isNull() const {
        return !value;
    }

    bool Value::isArray() const {
        return jni::IsInstanceOf(env, value.get(), *java::ObjectArray::jclass);
    }

    bool Value::isObject() const {
        return jni::IsInstanceOf(env, value.get(), *java::Map::jclass);
    }

    bool Value::isString() const {
        return jni::IsInstanceOf(env, value.get(), *java::String::jclass);
    }

    bool Value::isBool() const {
        return jni::IsInstanceOf(env, value.get(), *java::Boolean::jclass);
    }

    bool Value::isNumber() const {
        return jni::IsInstanceOf(env, value.get(), *java::Number::jclass);
    }

    std::string Value::toString() const {
        return jni::Make<std::string>(env, jni::Cast(env, jni::Class<jni::StringTag>::Singleton(env), value));
    }

    float Value::toFloat() const {
        return jni::CallMethod<jni::jfloat>(env, value.get(), *java::Number::floatValueMethodId);
    }

    double Value::toDouble() const {
        return jni::CallMethod<jni::jdouble>(env, value.get(), *java::Number::doubleValueMethodId);
    }

    long Value::toLong() const {
        return jni::CallMethod<jni::jlong>(env, value.get(), *java::Number::longValueMethodId);
    }

    bool Value::toBool() const {
        return jni::CallMethod<jni::jboolean>(env, value.get(), *java::Boolean::booleanValueMethodId);
    }

    Value Value::get(const char* key) const {
        jni::jobject* member = jni::CallMethod<jni::jobject*>(env, value.get(), *java::Map::getMethodId, jni::Make<jni::String>(env, std::string(key)).get());
        return Value(env, jni::Local<jni::Object<>>(env, member));
    }

    Value Value::keyArray() const{
        jni::jobject* set = jni::CallMethod<jni::jobject*>(env, value.get(), *java::Map::keySetMethodId);
        jni::jobject* array = jni::CallMethod<jni::jobject*>(env, set, *java::Set::toArrayMethodId);
        return Value(env, jni::Local<jni::Object<>>(env, array));
    }

    int Value::getLength() const {
        auto array = (jni::jarray<jni::jobject>*) value.get();
        return jni::GetArrayLength(env, *array);
    }

    Value Value::get(const int index) const {
        auto array = (jni::jarray<jni::jobject>*) value.get();
        return Value(env, jni::Local<jni::Object<>>(env, jni::GetObjectArrayElement(env, *array, index)));
    }
}
}

