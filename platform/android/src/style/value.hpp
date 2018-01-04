#pragma once

#include <jni/jni.hpp>

#include <string>

namespace mbgl {
namespace android {

class Value {
public:
    Value(jni::JNIEnv&, jni::jobject*);

    Value(Value&&)                 = default;
    Value& operator=(Value&&)      = default;

    Value(const Value&)            = delete;
    Value& operator=(const Value&) = delete;

    bool isNull() const;
    bool isArray() const;
    bool isObject() const;
    bool isString() const;
    bool isBool() const;
    bool isNumber() const;

    std::string toString() const;
    float toFloat() const;
    double toDouble() const;
    long toLong() const;
    bool toBool() const;
    Value get(const char* key) const;
    int getLength() const;
    Value get(const int index ) const;

    jni::JNIEnv& env;
    std::shared_ptr<jni::jobject> value;
};

}
}
