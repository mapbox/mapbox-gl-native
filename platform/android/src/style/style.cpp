#include "style.hpp"

namespace mbgl {
namespace android {

Style::Style(jni::JNIEnv &) {

}

Style::~Style() {
}

jni::Class<Style> Style::javaClass;

void Style::registerNative(jni::JNIEnv& env) {
    //Register classes
    Style::javaClass = *jni::Class<Style>::Find(env).NewGlobalRef(env).release();

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<Style>(
        env, Style::javaClass, "nativePtr",
        std::make_unique<Style, JNIEnv&>,
        "initialize",
        "finalize"
);
}


} // namespace mbgl
} // namespace android