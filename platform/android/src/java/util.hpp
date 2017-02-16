#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace java {
namespace util {

class List : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "java/util/List"; };

    template<class T>
    static jni::Array<jni::Object<T>> toArray(jni::JNIEnv& env, jni::Object<List> list) {
        static auto toArray = List::javaClass.GetMethod<jni::Array<jni::Object<>> ()>(env, "toArray");
        return (jni::Array<jni::Object<T>>) list.Call(env, toArray);
    };

    static jni::Class<List> javaClass;

};

void registerNative(jni::JNIEnv&);


} // namespace util
} // namespace java
} // namespace android
} // namespace mbgl