// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

#include "symbol_layer.hpp"

#include <string>

//XXX
#include <mbgl/platform/log.hpp>

namespace mbgl {
namespace android {

    SymbolLayer::SymbolLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::SymbolLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
        mbgl::Log::Debug(mbgl::Event::JNI, "SymbolLayer constructed, owning reference");
    }

    SymbolLayer::SymbolLayer(mbgl::Map& map, mbgl::style::SymbolLayer& coreLayer)
        : Layer(map, coreLayer) {

        mbgl::Log::Debug(mbgl::Event::JNI, "SymbolLayer Non-owning reference constructor");
    }

    SymbolLayer::~SymbolLayer() = default;

    jni::Class<SymbolLayer> SymbolLayer::javaClass;

    jni::jobject* SymbolLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = SymbolLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return SymbolLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void SymbolLayer::registerNative(jni::JNIEnv& env) {
        mbgl::Log::Debug(mbgl::Event::JNI, "Registering native background layer");

        //Lookup the class
        SymbolLayer::javaClass = *jni::Class<SymbolLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<SymbolLayer>(
            env, SymbolLayer::javaClass, "nativePtr",
            std::make_unique<SymbolLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize"
        );

    }

} // namespace android
} // namespace mbgl
