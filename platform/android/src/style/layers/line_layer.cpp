// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

#include "line_layer.hpp"

#include <string>

//XXX
#include <mbgl/platform/log.hpp>

namespace mbgl {
namespace android {

    LineLayer::LineLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::LineLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
        mbgl::Log::Debug(mbgl::Event::JNI, "LineLayer constructed, owning reference");
    }

    LineLayer::LineLayer(mbgl::Map& map, mbgl::style::LineLayer& coreLayer)
        : Layer(map, coreLayer) {

        mbgl::Log::Debug(mbgl::Event::JNI, "LineLayer Non-owning reference constructor");
    }

    LineLayer::~LineLayer() = default;

    jni::Class<LineLayer> LineLayer::javaClass;

    jni::jobject* LineLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = LineLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return LineLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void LineLayer::registerNative(jni::JNIEnv& env) {
        mbgl::Log::Debug(mbgl::Event::JNI, "Registering native background layer");

        //Lookup the class
        LineLayer::javaClass = *jni::Class<LineLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<LineLayer>(
            env, LineLayer::javaClass, "nativePtr",
            std::make_unique<LineLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize"
        );

    }

} // namespace android
} // namespace mbgl
