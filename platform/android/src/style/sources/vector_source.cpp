#include "vector_source.hpp"

#include "../android_conversion.hpp"
#include "../value.hpp"
#include "../conversion/url_or_tileset.hpp"

#include <mbgl/util/variant.hpp>

#include <string>

namespace mbgl {
namespace android {

    VectorSource::VectorSource(jni::JNIEnv& env, jni::String sourceId, jni::Object<> urlOrTileSet)
        : Source(
            env,
            std::make_unique<mbgl::style::VectorSource>(
                jni::Make<std::string>(env, sourceId),
                *style::conversion::convert<variant<std::string, Tileset>>(Value(env, urlOrTileSet))
            )
        ) {
    }

    VectorSource::VectorSource(mbgl::Map& map, mbgl::style::VectorSource& coreSource)
        : Source(map, coreSource) {
    }

    VectorSource::~VectorSource() = default;

    jni::Class<VectorSource> VectorSource::javaClass;

    jni::jobject* VectorSource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = VectorSource::javaClass.template GetConstructor<jni::jlong>(env);
        return VectorSource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void VectorSource::registerNative(jni::JNIEnv& env) {
        //Lookup the class
        VectorSource::javaClass = *jni::Class<VectorSource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<VectorSource>(
            env, VectorSource::javaClass, "nativePtr",
            std::make_unique<VectorSource, JNIEnv&, jni::String, jni::Object<>>,
            "initialize",
            "finalize"
        );
    }

} // namespace android
} // namespace mbgl
