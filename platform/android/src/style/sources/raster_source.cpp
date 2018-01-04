#include "raster_source.hpp"

#include "../android_conversion.hpp"
#include "../value.hpp"
#include "../conversion/url_or_tileset.hpp"

#include <mbgl/util/variant.hpp>

#include <string>

namespace mbgl {
namespace android {

    RasterSource::RasterSource(jni::JNIEnv& env, jni::String sourceId, jni::Object<> urlOrTileSet, jni::jint tileSize)
        : Source(
            env,
            std::make_unique<mbgl::style::RasterSource>(
                jni::Make<std::string>(env, sourceId),
                convertURLOrTileset(Value(env, urlOrTileSet)),
                tileSize
            )
        ) {
    }

    RasterSource::RasterSource(jni::JNIEnv& env,
                               mbgl::style::Source& coreSource,
                               AndroidRendererFrontend& frontend)
        : Source(env, coreSource, createJavaPeer(env), frontend) {
    }

    RasterSource::~RasterSource() = default;

    jni::String RasterSource::getURL(jni::JNIEnv& env) {
        optional<std::string> url = source.as<mbgl::style::RasterSource>()->RasterSource::getURL();
        return url ? jni::Make<jni::String>(env, *url) : jni::String();
    }

    jni::Class<RasterSource> RasterSource::javaClass;

    jni::Object<Source> RasterSource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = RasterSource::javaClass.template GetConstructor<jni::jlong>(env);
        return jni::Object<Source>(RasterSource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this)).Get());
    }

    void RasterSource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        RasterSource::javaClass = *jni::Class<RasterSource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<RasterSource>(
            env, RasterSource::javaClass, "nativePtr",
            std::make_unique<RasterSource, JNIEnv&, jni::String, jni::Object<>, jni::jint>,
            "initialize",
            "finalize",
            METHOD(&RasterSource::getURL, "nativeGetUrl")
        );
    }

} // namespace android
} // namespace mbgl
