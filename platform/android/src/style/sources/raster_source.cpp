#include "raster_source.hpp"

#include "../android_conversion.hpp"
#include "../value.hpp"
#include "../conversion/url_or_tileset.hpp"

#include <mbgl/util/variant.hpp>

#include <string>

namespace mbgl {
namespace android {

    RasterSource::RasterSource(jni::JNIEnv& env, const jni::String& sourceId, const jni::Object<>& urlOrTileSet, jni::jint tileSize)
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

    jni::Local<jni::String> RasterSource::getURL(jni::JNIEnv& env) {
        optional<std::string> url = source.as<mbgl::style::RasterSource>()->RasterSource::getURL();
        return url ? jni::Make<jni::String>(env, *url) : jni::Local<jni::String>();
    }

    jni::Local<jni::Object<Source>> RasterSource::createJavaPeer(jni::JNIEnv& env) {
        static auto& javaClass = jni::Class<RasterSource>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
        return javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void RasterSource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<RasterSource>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<RasterSource>(
            env, javaClass, "nativePtr",
            jni::MakePeer<RasterSource, const jni::String&, const jni::Object<>&, jni::jint>,
            "initialize",
            "finalize",
            METHOD(&RasterSource::getURL, "nativeGetUrl")
        );
    }

} // namespace android
} // namespace mbgl
