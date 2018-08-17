#pragma once

#include "conversion.hpp"

#include <jni/jni.hpp>

#include <vector>

namespace mbgl {
namespace android {
namespace conversion {

/**
 * Convert jarray -> ArrayList
 */
template <class T>
jni::jobject* toArrayList(JNIEnv& env, jni::jarray<T>& array) {
    static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/util/Arrays")).release();
    static jni::jmethodID* asList = &jni::GetStaticMethodID(env, *javaClass, "asList", "([Ljava/lang/Object;)Ljava/util/List;");
    return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *asList, array));
}

// Java -> C++

std::vector<std::string> toVector(JNIEnv& env, jni::jarray<jni::jobject>& array);
std::vector<std::string> toVector(JNIEnv& env, jni::Array<jni::String> array);

}
}
}
