// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

#include "circle_layer.hpp"

#include <string>

//XXX
#include <mbgl/platform/log.hpp>

namespace mbgl {
namespace android {

    CircleLayer::CircleLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::CircleLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
        mbgl::Log::Debug(mbgl::Event::JNI, "CircleLayer constructed, owning reference");
    }

    CircleLayer::CircleLayer(mbgl::Map& map, mbgl::style::CircleLayer& coreLayer)
        : Layer(map, coreLayer) {

        mbgl::Log::Debug(mbgl::Event::JNI, "CircleLayer Non-owning reference constructor");
    }

    CircleLayer::~CircleLayer() = default;

    jni::Class<CircleLayer> CircleLayer::javaClass;

    jni::jobject* CircleLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = CircleLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return CircleLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void CircleLayer::registerNative(jni::JNIEnv& env) {
        mbgl::Log::Debug(mbgl::Event::JNI, "Registering native background layer");

        //Lookup the class
        CircleLayer::javaClass = *jni::Class<CircleLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<CircleLayer>(
            env, CircleLayer::javaClass, "nativePtr",
            std::make_unique<CircleLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize"
        );

    }

} // namespace android
} // namespace mbgl
