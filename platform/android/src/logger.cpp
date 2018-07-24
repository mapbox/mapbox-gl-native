#include "logger.hpp"
#include "java_types.hpp"

namespace mbgl {
namespace android {

void Logger::registerNative(jni::JNIEnv& env) {
    _class = *jni::Class<Logger>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Logger> Logger::_class;

void Logger::log(jni::JNIEnv& env, EventSeverity severity, const std::string &msg) {
    auto tag = jni::Make<jni::String>(env, "MBGL");
    auto message = jni::Make<jni::String>(env, msg);
    using Signature = void(jni::String, jni::String);

    if(severity == EventSeverity::Debug){
        auto method = _class.GetStaticMethod<Signature>(env, "d");
        _class.Call(env, method, tag, message);
    }else if(severity == EventSeverity::Info){
        auto method = _class.GetStaticMethod<Signature>(env, "i");
        _class.Call(env, method, tag, message);
    }else if(severity == EventSeverity::Warning){
        auto method = _class.GetStaticMethod<Signature>(env, "w");
        _class.Call(env, method, tag, message);
    }else{
        auto method = _class.GetStaticMethod<Signature>(env, "e");
        _class.Call(env, method, tag, message);
    }
}

} // namespace android
} // namespace mbgl