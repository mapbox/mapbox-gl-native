#include "value.hpp"

#include "../java_types.hpp"

namespace mbgl {
namespace android {

    //Instance

    Value::Value(jni::JNIEnv& env, jni::jobject* _value) : jenv(env), value(_value) {}

    Value::~Value() {}

    bool Value::isNull() const {
        return value == nullptr;
    }

    bool Value::isArray() const {
        return jni::IsInstanceOf(jenv, value, *java::ObjectArray::jclass);
    }

    bool Value::isObject() const {
        return jni::IsInstanceOf(jenv, value, *java::Map::jclass);;
    }

    bool Value::isString() const {
        return jni::IsInstanceOf(jenv, value, *java::String::jclass);
    }

    bool Value::isBool() const {
        return jni::IsInstanceOf(jenv, value, *java::Boolean::jclass);
    }

    bool Value::isNumber() const {
        return jni::IsInstanceOf(jenv, value, *java::Number::jclass);
    }

    std::string Value::toString() const {
        jni::jstring* string = reinterpret_cast<jni::jstring*>(value);
        return jni::Make<std::string>(jenv, jni::String(string));
    }

    float Value::toNumber() const {
        return jni::CallMethod<jni::jfloat>(jenv, value, *java::Number::floatValueMethodId);
    }

    bool Value::toBool() const {
        return jni::CallMethod<jni::jboolean>(jenv, value, *java::Boolean::booleanValueMethodId);
    }

    Value Value::get(const char* key) const {
        jni::jobject* member = jni::CallMethod<jni::jobject*>(jenv, value, *java::Map::getMethodId, jni::Make<jni::String>(jenv, std::string(key)).Get());
        return Value(jenv, member);
    }

    int Value::getLength() const {
        auto array = (jni::jarray<jni::jobject>*) value;
        return jni::GetArrayLength(jenv, *array);
    }

    Value Value::get(const int index ) const {
        auto array = (jni::jarray<jni::jobject>*) value;
        return Value(jenv, jni::GetObjectArrayElement(jenv, *array, index));
    }
}
}

