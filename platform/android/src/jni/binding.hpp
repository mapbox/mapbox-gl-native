#pragma once

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace binding {

#ifndef NDEBUG

void RegisterNative(void (*)(jni::JNIEnv&));
void RegisterNative(jni::JNIEnv&);

// This class acts as a catch-all class for static initialization of JNI classes/methods.
// The Class, Constructor, StaticMethod, and Method classes below have a reference to the
// static member in this class. Since this class is templated, referencing it will trigger a
// concrete instantiation of the static member named "Invoke". The template definition below
// initializes an object of this class, and passes a function pointer (in the form of a lambda) to
// the constructor, which adds this function pointer to an internal vector of function pointers.
// Static initialization means that all Classes/Constructors/StaticMethods/Methods that are used
// *somewhere* in this code (and thus trigger the template instantiation) will get initialized
// during program startup (before main), albeit in an unknown order.
// We collect these so that a Debug build can forcibly trigger a call to every referenced entity.
// This is useful for verifying that all referenced Classes/Constructors/StaticMethods/Methods can
// be found through JNI lookups at program startup.
// Release builds do not build such a cache (see the #ifdef). All JNI lookups happen lazily in that
// case.
template <class Type>
struct Register {
    Register(void (*registerNative)(jni::JNIEnv&)) {
        RegisterNative(registerNative);
    }
    static Register Invoke;
};

template <class Type>
Register<Type> Register<Type>::Invoke{ [](jni::JNIEnv& env) { Type::Get(env); } };

#endif

// This class acts as a static cache for obtaining the Java Class object. The only template
// parameter is a Tag that must have a `static constexpr auto Name()` function that returns a
// `const char*` of the class name (e.g. "java/lang/Boolean").
template <class ClassTag>
struct Class {
    static auto Get(jni::JNIEnv& env) {
#ifndef NDEBUG
        (void)Register<Class>::Invoke; // Leave this here. See documentation above.
#endif
        static auto javaClass = jni::Class<ClassTag>::Find(env).NewGlobalRef(env);
        return *javaClass;
    }
};

// This class acts as a static cache for obtaining a constructor methods of a Java class. After
// the ClassTag (see above), it expects the argument list of the constructor, or nothing if the
// class' constructor doesn't take an argument.
template <class ClassTag, class... Args>
struct Constructor {
    static auto Get(jni::JNIEnv& env) {
#ifndef NDEBUG
        (void)Register<Constructor>::Invoke; // Leave this here. See documentation above.
#endif
        static const auto javaConstructor =
            Class<ClassTag>::Get(env).template GetConstructor<Args...>(env);
        return javaConstructor;
    }
};

// This class acts as a static cache for obtaining a static method of a Java class. After
// the ClassTag (see above), it expects a StaticMethodTag which returns the name of the static
// method as a string, identical to how the ClassTag works. Then, it expects the function signature
// R(Args...) of the static method.
// It also provides a static Call() function that allows invoking the static method.
template <class ClassTag, class StaticMethodTag, class>
struct StaticMethod;

template <class ClassTag, class StaticMethodTag, class R, class... Args>
struct StaticMethod<ClassTag, StaticMethodTag, R(Args...)> {
    static auto Get(jni::JNIEnv& env) {
#ifndef NDEBUG
        (void)Register<StaticMethod>::Invoke; // Leave this here. See documentation above.
#endif
        static const auto javaStaticMethod =
            Class<ClassTag>::Get(env).template GetStaticMethod<R(Args...)>(env,
                                                                           StaticMethodTag::Name());
        return javaStaticMethod;
    }

    template <class... Params>
    static auto Call(jni::JNIEnv& env, Params&&... params) {
        Class<ClassTag>::Get(env).Call(env, Get(env), std::forward<Params>(params)...);
    }
};

// This class acts as a static cache for obtaining a method of a Java class. After the ClassTag (see
// above), it expects a MethodTag which returns the name of the method as a string, identical to how
// the ClassTag works. Then, it expects the function signature R(Args...) of the method.
// It also provides a static Call() function that allows invoking the method when providing an
// instance of the class.
template <class ClassTag, class MethodTag, class>
struct Method;

template <class ClassTag, class MethodTag, class R, class... Args>
struct Method<ClassTag, MethodTag, R(Args...)> {
    static auto Get(jni::JNIEnv& env) {
#ifndef NDEBUG
        (void)Register<Method>::Invoke; // Leave this here. See documentation above.
#endif
        static const auto javaMethod =
            Class<ClassTag>::Get(env).template GetMethod<R(Args...)>(env, MethodTag::Name());
        return javaMethod;
    }

    template <class... Params>
    static auto Call(jni::JNIEnv& env, jni::Object<ClassTag> obj, Params&&... params) {
        return obj.Call(env, Get(env), std::forward<Params>(params)...);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace binding
} // namespace android
} // namespace mbgl

template <class ClassTag, class... Args>
auto MakeAnything(jni::ThingToMake<jni::Object<ClassTag>>,
                  jni::JNIEnv& env,
                  mbgl::android::binding::Constructor<ClassTag, Args...>,
                  Args&&... args) {
    return mbgl::android::binding::Class<ClassTag>::Get(env).New(
        env, mbgl::android::binding::Constructor<ClassTag, Args...>::Get(env),
        std::forward<Args>(args)...);
}

template <class ClassTag>
auto MakeAnything(jni::ThingToMake<jni::Array<jni::Object<ClassTag>>>,
                  jni::JNIEnv& env,
                  jni::jsize length,
                  const jni::Object<ClassTag>& initialElement = jni::Object<ClassTag>()) {
    return jni::Array<jni::Object<ClassTag>>::New(
        env, length, mbgl::android::binding::Class<ClassTag>::Get(env), initialElement);
}
