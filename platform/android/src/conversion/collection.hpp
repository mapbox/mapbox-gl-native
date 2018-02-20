#pragma once

#include "conversion.hpp"
#include "constant.hpp"

#include <mbgl/util/optional.hpp>
#include <jni/jni.hpp>

#include <vector>

namespace mbgl {
namespace android {
namespace conversion {

/**
 * Convert jarray -> ArrayList
 */
template <class T>
inline jni::jobject* toArrayList(JNIEnv& env, jni::jarray<T>& array) {
    static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/util/Arrays")).release();
    static jni::jmethodID* asList = &jni::GetStaticMethodID(env, *javaClass, "asList", "([Ljava/lang/Object;)Ljava/util/List;");
    return reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *asList, array));
}

// Java -> C++


inline std::vector<std::string> toVector(JNIEnv& env, jni::jarray<jni::jobject>& array) {
    std::vector<std::string> vector;
    std::size_t len = jni::GetArrayLength(env, array);
    vector.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        jni::jstring* jstr = reinterpret_cast<jni::jstring*>(jni::GetObjectArrayElement(env, array, i));
        vector.push_back(*convert<std::string, jni::String>(env, jni::String(jstr)));
        jni::DeleteLocalRef(env, jstr);
    }

    return vector;
}

inline std::vector<std::string> toVector(JNIEnv& env, jni::Array<jni::String> array) {
    std::size_t len = array.Length(env);
    std::vector<std::string> vector;
    vector.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        jni::String jstr = array.Get(env, i);
        vector.push_back(*convert<std::string, jni::String>(env, jstr));
        jni::DeleteLocalRef(env, jstr);
    }

    return vector;
}

}
}
}
