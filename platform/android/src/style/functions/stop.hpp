#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <jni/jni.hpp>

#include "../../java/lang.hpp"

namespace mbgl {
namespace android {

class Stop : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/functions/stops/Stop"; };

    template<class I, class O>
    static jni::Object<Stop> New(jni::JNIEnv& env, jni::Object<I> in, jni::Object<O> out) {
        static auto constructor = Stop::javaClass.GetConstructor<jni::Object<>, jni::Object<>>(env);
        return Stop::javaClass.New(env, constructor, (jni::Object<>) in, (jni::Object<>) out);
    }

    static jni::Class<Stop> javaClass;

    static void registerNative(jni::JNIEnv&);

    class CompositeValue : private mbgl::util::noncopyable {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/functions/stops/Stop$CompositeValue"; };

        static jni::Object<Stop::CompositeValue> New(jni::JNIEnv&, jni::Object<java::lang::Number>, jni::Object<>);

        static jni::Class<Stop::CompositeValue> javaClass;
    };
};

} // namespace android
} // namespace mbgl
