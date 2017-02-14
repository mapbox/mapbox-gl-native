#pragma once

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace java {
namespace lang {

class Throwable {
public:
    static constexpr auto Name() {
        return "java/lang/Throwable";
    }

    static jni::String GetMessage(jni::JNIEnv&, jni::Object<Throwable>);

private:
    static jni::Class<Throwable> Class(jni::JNIEnv&);
};

} // namespace lang
} // namespace java
} // namespace android
} // namespace mbgl
