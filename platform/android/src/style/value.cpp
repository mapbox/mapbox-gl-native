#include "value.hpp"

#include "../java_types.hpp"

namespace mbgl {
namespace android {

    class ObjectDeleter {
    public:
        ObjectDeleter() = default;
        ObjectDeleter(JNIEnv& e) : env(e) {}

        void operator()(jni::jobject* p) const {
          if (p) {
              jni::DeleteLocalRef(env, p);
          }
        }

    private:
        JNIEnv& env;
    };

    //Instance

    Value::Value(jni::JNIEnv& env, jni::jobject* _value) : jenv(env), value(_value, ObjectDeleter(env)) {}

    Value::~Value() = default;

    bool Value::isNull() const {
        return value == nullptr;
    }

    bool Value::isArray() const {
        return jni::IsInstanceOf(jenv, value.get(), *java::ObjectArray::jclass);
    }

    bool Value::isObject() const {
        return jni::IsInstanceOf(jenv, value.get(), *java::Map::jclass);;
    }

    bool Value::isString() const {
        return jni::IsInstanceOf(jenv, value.get(), *java::String::jclass);
    }

    bool Value::isBool() const {
        return jni::IsInstanceOf(jenv, value.get(), *java::Boolean::jclass);
    }

    bool Value::isNumber() const {
        return jni::IsInstanceOf(jenv, value.get(), *java::Number::jclass);
    }

    std::string Value::toString() const {
        jni::jstring* string = reinterpret_cast<jni::jstring*>(value.get());
        return jni::Make<std::string>(jenv, jni::String(string));
    }

    float Value::toNumber() const {
        return jni::CallMethod<jni::jfloat>(jenv, value.get(), *java::Number::floatValueMethodId);
    }

    bool Value::toBool() const {
        return jni::CallMethod<jni::jboolean>(jenv, value.get(), *java::Boolean::booleanValueMethodId);
    }

    Value Value::get(const char* key) const {
        jni::jobject* member = jni::CallMethod<jni::jobject*>(jenv, value.get(), *java::Map::getMethodId, jni::Make<jni::String>(jenv, std::string(key)).Get());
        return Value(jenv, member);
    }

    int Value::getLength() const {
        auto array = (jni::jarray<jni::jobject>*) value.get();
        return jni::GetArrayLength(jenv, *array);
    }

    Value Value::get(const int index ) const {
        auto array = (jni::jarray<jni::jobject>*) value.get();
        return Value(jenv, jni::GetObjectArrayElement(jenv, *array, index));
    }
}
}

