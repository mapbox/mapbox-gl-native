#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace math {

class Math : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "java/lang/Math"; };

    static jni::jdouble rint(jni::JNIEnv& env, const jni::jdouble value) {
        static auto& javaClass = jni::Class<Math>::Singleton(env);
        static auto method = javaClass.GetStaticMethod<jni::jdouble (jni::jdouble)>(env, "rint");
        return javaClass.Call(env, method, value);
    };
};

void registerNative(jni::JNIEnv&);

} // namespace math
} // namespace android
} // namespace mbgl