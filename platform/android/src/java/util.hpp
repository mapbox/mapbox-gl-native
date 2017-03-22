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

class Set : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "java/util/Set"; };

    template<class T>
    static jni::Array<jni::Object<T>> toArray(jni::JNIEnv& env, jni::Object<Set> list) {
        static auto toArray = Set::javaClass.GetMethod<jni::Array<jni::Object<>> ()>(env, "toArray");
        return (jni::Array<jni::Object<T>>) list.Call(env, toArray);
    };

    static jni::Class<Set> javaClass;

};

class Map : private mbgl::util::noncopyable {
public:

    class Entry : private mbgl::util::noncopyable {
    public:
        static constexpr auto Name() { return "java/util/Map$Entry"; };

        template <class T>
        static jni::Object<T> getKey(jni::JNIEnv& env, jni::Object<Entry> entry) {
            static auto method = Entry::javaClass.GetMethod<jni::Object<> ()>(env, "getKey");
            return (jni::Object<T>) entry.Call(env, method);
        }

        template <class T>
        static jni::Object<T> getValue(jni::JNIEnv& env, jni::Object<Entry> entry) {
            static auto method = Entry::javaClass.GetMethod<jni::Object<> ()>(env, "getValue");
            return (jni::Object<T>) entry.Call(env, method).Get();
        }

        static jni::Class<Entry> javaClass;
    };

    static constexpr auto Name() { return "java/util/Map"; };

    static jni::Class<Map> javaClass;
};

void registerNative(jni::JNIEnv&);


} // namespace util
} // namespace java
} // namespace android
} // namespace mbgl