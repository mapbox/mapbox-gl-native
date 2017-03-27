#include "java_types.hpp"

namespace mbgl {
namespace android {
namespace java {

    jni::jclass* ObjectArray::jclass;

    jni::jclass* String::jclass;

    jni::jclass* Boolean::jclass;
    jni::jmethodID* Boolean::booleanValueMethodId;

    jni::jclass* Number::jclass;
    jni::jmethodID* Number::floatValueMethodId;
    jni::jmethodID* Number::doubleValueMethodId;
    jni::jmethodID* Number::longValueMethodId;

    jni::jclass* Map::jclass;
    jni::jmethodID* Map::getMethodId;

    void registerNatives(JNIEnv& env) {
        ObjectArray::jclass = jni::NewGlobalRef(env, &jni::FindClass(env, "[Ljava/lang/Object;")).release();

        String::jclass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/String")).release();

        Boolean::jclass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/Boolean")).release();
        Boolean::booleanValueMethodId = &jni::GetMethodID(env, *Boolean::jclass, "booleanValue", "()Z");

        Number::jclass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/Number")).release();
        Number::floatValueMethodId = &jni::GetMethodID(env, *Number::jclass, "floatValue", "()F");
        Number::doubleValueMethodId = &jni::GetMethodID(env, *Number::jclass, "doubleValue", "()D");
        Number::longValueMethodId = &jni::GetMethodID(env, *Number::jclass, "longValue", "()J");

        Map::jclass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/util/Map")).release();
        Map::getMethodId = &jni::GetMethodID(env, *Map::jclass, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
    }

}
}
}
