#pragma once

#include <jni/jni.hpp>
#include <mbgl/util/noncopyable.hpp>

namespace mbgl {
namespace android {
namespace java {
namespace lang {

class Float : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "java/lang/Float"; };

    static jni::Object<Float> valueOf(JNIEnv&, jfloat);

    static jni::Class<Float> javaClass;

    static void registerNative(jni::JNIEnv&);
};

class Double : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "java/lang/Double"; };

    static jni::Object<Double> valueOf(JNIEnv&, jdouble);

    static jni::Class<Double> javaClass;

    static void registerNative(jni::JNIEnv&);
};

class Long : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "java/lang/Long"; };

    static jni::Object<Long> valueOf(JNIEnv&, jlong);

    static jni::Class<Long> javaClass;

    static void registerNative(jni::JNIEnv&);
};

class Boolean : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "java/lang/Boolean"; };

    static jni::Object<Boolean> valueOf(JNIEnv&, jboolean);

    static jni::Class<Boolean> javaClass;

    static void registerNative(jni::JNIEnv&);
};

class Number : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "java/lang/Number"; };

    static jni::Class<Number> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace lang
} // namespace java
} // namespace android
} // namespace mbgl
