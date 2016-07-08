// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

#include "fill_layer.hpp"

#include <string>

//XXX
#include <mbgl/platform/log.hpp>

namespace mbgl {
namespace android {

    FillLayer::FillLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::FillLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
        mbgl::Log::Debug(mbgl::Event::JNI, "FillLayer constructed, owning reference");
    }

    FillLayer::FillLayer(mbgl::Map& map, mbgl::style::FillLayer& coreLayer)
        : Layer(map, coreLayer) {

        mbgl::Log::Debug(mbgl::Event::JNI, "FillLayer Non-owning reference constructor");
    }

    FillLayer::~FillLayer() = default;

    jni::Class<FillLayer> FillLayer::javaClass;

    jni::jobject* FillLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = FillLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return FillLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void FillLayer::registerNative(jni::JNIEnv& env) {
        mbgl::Log::Debug(mbgl::Event::JNI, "Registering native background layer");

        //Lookup the class
        FillLayer::javaClass = *jni::Class<FillLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<FillLayer>(
            env, FillLayer::javaClass, "nativePtr",
            std::make_unique<FillLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize"
        );

    }

} // namespace android
} // namespace mbgl
