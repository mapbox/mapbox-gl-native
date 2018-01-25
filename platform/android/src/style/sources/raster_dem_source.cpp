#include "raster_dem_source.hpp"

#include "../android_conversion.hpp"
#include "../value.hpp"
#include "../conversion/url_or_tileset.hpp"
#include "source.hpp"

#include <mbgl/util/variant.hpp>

#include <string>

namespace mbgl {
namespace android {

    RasterDEMSource::RasterDEMSource(jni::JNIEnv& env, jni::String sourceId, jni::Object<> urlOrTileSet, jni::jint tileSize)
        : Source(
            env,
            std::make_unique<mbgl::style::RasterDEMSource>(
                jni::Make<std::string>(env, sourceId),
                convertURLOrTileset(Value(env, urlOrTileSet)),
                tileSize
            )
        ) {
    }

    RasterDEMSource::RasterDEMSource(jni::JNIEnv& env,
                               mbgl::style::Source& coreSource,
                               AndroidRendererFrontend& frontend)
         : Source(env, coreSource, createJavaPeer(env), frontend) {
    }

    RasterDEMSource::~RasterDEMSource() = default;

    jni::String RasterDEMSource::getURL(jni::JNIEnv& env) {
        optional<std::string> url = source.as<mbgl::style::RasterDEMSource>()->RasterDEMSource::getURL();
        return url ? jni::Make<jni::String>(env, *url) : jni::String();
    }

    jni::Class<RasterDEMSource> RasterDEMSource::javaClass;

    jni::Object<Source> RasterDEMSource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = RasterDEMSource::javaClass.template GetConstructor<jni::jlong>(env);
        return jni::Object<Source>(RasterDEMSource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this)).Get());
    }

    void RasterDEMSource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        RasterDEMSource::javaClass = *jni::Class<RasterDEMSource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<RasterDEMSource>(
            env, RasterDEMSource::javaClass, "nativePtr",
            std::make_unique<RasterDEMSource, JNIEnv&, jni::String, jni::Object<>, jni::jint>,
            "initialize",
            "finalize",
            METHOD(&RasterDEMSource::getURL, "nativeGetUrl")
        );
    }

} // namespace android
} // namespace mbgl
