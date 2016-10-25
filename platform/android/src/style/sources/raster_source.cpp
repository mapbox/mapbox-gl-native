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
                *style::conversion::convert<variant<std::string, Tileset>>(Value(env, urlOrTileSet)),
                tileSize
            )
        ) {
    }

    RasterSource::RasterSource(mbgl::Map& map, mbgl::style::RasterSource& coreSource)
        : Source(map, coreSource) {
    }

    RasterSource::~RasterSource() = default;

    jni::Class<RasterSource> RasterSource::javaClass;

    jni::jobject* RasterSource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = RasterSource::javaClass.template GetConstructor<jni::jlong>(env);
        return RasterSource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void RasterSource::registerNative(jni::JNIEnv& env) {
        //Lookup the class
        RasterSource::javaClass = *jni::Class<RasterSource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<RasterSource>(
            env, RasterSource::javaClass, "nativePtr",
            std::make_unique<RasterSource, JNIEnv&, jni::String, jni::Object<>, jni::jint>,
            "initialize",
            "finalize"
        );
    }

} // namespace android
} // namespace mbgl
