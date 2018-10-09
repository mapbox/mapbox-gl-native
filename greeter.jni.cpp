#include "greeter.jni.hpp"

#include <jni/jni.hpp>
#include <mapbox/bindgen/jni.hpp>
#include <mapbox/bindgen/jni/marshal.hpp>

#pragma clang diagnostic ignored "-Wglobal-constructors"

namespace mapbox {
namespace bindgen {

static Registration registration { [] (jni::JNIEnv& env_) {
    jni::RegisterNativePeer<std::shared_ptr<::Greeter>>(env_,
        jni::Class<Greeter>::Singleton(env_),
        "peer",
        [] (jni::JNIEnv& env, jni::Object<Greeter> self, jni::String greeting) {
            return std::make_unique<std::shared_ptr<::Greeter>>(
                Greeter::createCpp(env, self, String::toCpp(env, greeting)));
        },
        "initialize",
        "finalize",
        jni::MakeNativePeerMethod("greeting",
            [] (jni::JNIEnv& env, std::shared_ptr<::Greeter>& object, jni::String name) -> jni::String {
                (void)env;
                return String::toJava(env, object->greeting(String::toCpp(env, name))).release();
            }));
} };

} // namespace bindgen
} // namespace mapbox
