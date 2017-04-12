#include "value.hpp"

#include "../java/lang.hpp"
#include "../java/util.hpp"

namespace mbgl {
namespace android {

class ObjectDeleter {
public:
    ObjectDeleter() = default;
    ObjectDeleter(JNIEnv& e) : env(e) {
    }

    void operator()(jni::jobject* p) const {
        if (p) {
            jni::DeleteLocalRef(env, p);
        }
    }

private:
    JNIEnv& env;
};

// Instance

Value::Value(jni::JNIEnv& env_, jni::Object<> value_) : env(env_), value(value_) {
}

Value::~Value() = default;

bool Value::isNull() const {
    return value;
}

bool Value::isArray() const {
    return value.IsInstanceOf(env, binding::Class<java::lang::ObjectArrayTag>::Get(env));
}

bool Value::isObject() const {
    return value.IsInstanceOf(env, binding::Class<java::util::MapTag>::Get(env));
}

bool Value::isString() const {
    return value.IsInstanceOf(env, binding::Class<java::lang::StringTag>::Get(env));
}

bool Value::isBool() const {
    return value.IsInstanceOf(env, binding::Class<java::lang::BooleanTag>::Get(env));
}

bool Value::isNumber() const {
    return value.IsInstanceOf(env, binding::Class<java::lang::NumberTag>::Get(env));
}

std::string Value::toString() const {
    return jni::Make<std::string>(env, jni::String(reinterpret_cast<jni::jstring*>(value.Get())));
}

float Value::toFloat() const {
    return java::lang::Number_floatValue::Call(env, java::lang::Number(value.Get()));
}

double Value::toDouble() const {
    return java::lang::Number_doubleValue::Call(env, java::lang::Number(value.Get()));
}

long Value::toLong() const {
    return java::lang::Number_longValue::Call(env, java::lang::Number(value.Get()));
}

bool Value::toBool() const {
    return java::lang::Boolean_booleanValue::Call(env, java::lang::Boolean(value.Get()));
}

Value Value::get(const char* key) const {
    return { env, java::util::Map_get::Call(env, java::util::Map(value.Get()),
                                            java::lang::Object(jni::Make<jni::String>(env, key))) };
}

int Value::getLength() const {
    return jni::Array<jni::Object<>>(reinterpret_cast<jni::jarray<jni::jobject>*>(value.Get()))
        .Length(env);
}

Value Value::get(const int index) const {
    return { env,
             jni::Array<jni::Object<>>(reinterpret_cast<jni::jarray<jni::jobject>*>(value.Get()))
                 .Get(env, index) };
}

} // namespace android
} // namespace mbgl
