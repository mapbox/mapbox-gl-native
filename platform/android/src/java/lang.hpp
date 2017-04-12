#pragma once

#include "../jni/binding.hpp"

namespace mbgl {
namespace android {
namespace java {
namespace lang {

using ObjectTag = jni::ObjectTag; // == "java/lang/Object"
using Object = jni::Object<>; // == jni::Object<jni::ObjectTag>

using StringTag = jni::StringTag; // == "java/lang/String"
using String = jni::String; // == jni::Object<jni::StringTag>

struct ObjectArrayTag { static constexpr auto Name() { return "[Ljava/lang/Object;"; } };
using ObjectArray = jni::Object<ObjectArrayTag>;

struct BooleanTag { static constexpr auto Name() { return "java/lang/Boolean"; } };
using Boolean = jni::Object<BooleanTag>;
using Boolean_constructor = binding::Constructor<BooleanTag, jni::jboolean>;
struct Boolean_booleanValueTag { static constexpr auto Name() { return "booleanValue"; } };
using Boolean_booleanValue = binding::Method<BooleanTag, Boolean_booleanValueTag, jni::jboolean(void)>;
template <typename T>
auto MakeAnything(jni::ThingToMake<Boolean>, jni::JNIEnv& env, T value) {
    return jni::Make<Boolean>(env, Boolean_constructor(), jni::jboolean(value));
}

struct FloatTag { static constexpr auto Name() { return "java/lang/Float"; } };
using Float = jni::Object<FloatTag>;
using Float_constructor = binding::Constructor<FloatTag, jni::jfloat>;
template <typename T>
auto MakeAnything(jni::ThingToMake<Float>, jni::JNIEnv& env, T value) {
    return jni::Make<Float>(env, Float_constructor(), jni::jfloat(value));
}

struct DoubleTag { static constexpr auto Name() { return "java/lang/Double"; } };
using Double = jni::Object<DoubleTag>;
using Double_constructor = binding::Constructor<DoubleTag, jni::jdouble>;
template <typename T>
auto MakeAnything(jni::ThingToMake<Double>, jni::JNIEnv& env, T value) {
    return jni::Make<Double>(env, Double_constructor(), jni::jdouble(value));
}

struct LongTag { static constexpr auto Name() { return "java/lang/Long"; } };
using Long = jni::Object<LongTag>;
using Long_constructor = binding::Constructor<LongTag, jni::jlong>;
template <typename T>
auto MakeAnything(jni::ThingToMake<Long>, jni::JNIEnv& env, T value) {
    return jni::Make<Long>(env, Long_constructor(), jni::jlong(value));
}

struct NumberTag { static constexpr auto Name() { return "java/lang/Number"; } };
using Number = jni::Object<NumberTag>;
struct Number_floatValueTag { static constexpr auto Name() { return "floatValue"; } };
using Number_floatValue = binding::Method<NumberTag, Number_floatValueTag, jni::jfloat(void)>;
struct Number_doubleValueTag { static constexpr auto Name() { return "doubleValue"; } };
using Number_doubleValue = binding::Method<NumberTag, Number_doubleValueTag, jni::jdouble(void)>;
struct Number_longValueTag { static constexpr auto Name() { return "longValue"; } };
using Number_longValue = binding::Method<NumberTag, Number_longValueTag, jni::jlong(void)>;

} // namespace lang
} // namespace java
} // namespace android
} // namespace mbgl
