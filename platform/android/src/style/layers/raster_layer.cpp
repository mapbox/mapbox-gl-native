// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

#include "raster_layer.hpp"

#include <string>

//XXX
#include <mbgl/platform/log.hpp>

namespace mbgl {
namespace android {

    RasterLayer::RasterLayer(jni::JNIEnv& env, jni::String layerId, jni::String sourceId)
        : Layer(env, std::make_unique<mbgl::style::RasterLayer>(jni::Make<std::string>(env, layerId), jni::Make<std::string>(env, sourceId))) {
        mbgl::Log::Debug(mbgl::Event::JNI, "RasterLayer constructed, owning reference");
    }

    RasterLayer::RasterLayer(mbgl::Map& map, mbgl::style::RasterLayer& coreLayer)
        : Layer(map, coreLayer) {

        mbgl::Log::Debug(mbgl::Event::JNI, "RasterLayer Non-owning reference constructor");
    }

    RasterLayer::~RasterLayer() = default;

    jni::Class<RasterLayer> RasterLayer::javaClass;

    jni::jobject* RasterLayer::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = RasterLayer::javaClass.template GetConstructor<jni::jlong>(env);
        return RasterLayer::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void RasterLayer::registerNative(jni::JNIEnv& env) {
        mbgl::Log::Debug(mbgl::Event::JNI, "Registering native background layer");

        //Lookup the class
        RasterLayer::javaClass = *jni::Class<RasterLayer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<RasterLayer>(
            env, RasterLayer::javaClass, "nativePtr",
            std::make_unique<RasterLayer, JNIEnv&, jni::String, jni::String>,
            "initialize",
            "finalize"
        );

    }

} // namespace android
} // namespace mbgl
