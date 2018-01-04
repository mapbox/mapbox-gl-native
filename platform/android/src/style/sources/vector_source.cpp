#include "vector_source.hpp"

#include <mbgl/renderer/query.hpp>

// Java -> C++ conversion
#include "../android_conversion.hpp"
#include "../conversion/filter.hpp"

// C++ -> Java conversion
#include "../../conversion/conversion.hpp"
#include "../../conversion/collection.hpp"
#include "../../geojson/conversion/feature.hpp"
#include "../conversion/url_or_tileset.hpp"

#include <mbgl/util/variant.hpp>

#include <string>
#include <vector>

namespace mbgl {
namespace android {

    VectorSource::VectorSource(jni::JNIEnv& env, jni::String sourceId, jni::Object<> urlOrTileSet)
        : Source(
            env,
            std::make_unique<mbgl::style::VectorSource>(
                jni::Make<std::string>(env, sourceId),
                convertURLOrTileset(Value(env, urlOrTileSet))
            )
        ) {
    }

    VectorSource::VectorSource(jni::JNIEnv& env,
                               mbgl::style::Source& coreSource,
                               AndroidRendererFrontend& frontend)
        : Source(env, coreSource, createJavaPeer(env), frontend) {
    }

    VectorSource::~VectorSource() = default;

    jni::String VectorSource::getURL(jni::JNIEnv& env) {
        optional<std::string> url = source.as<mbgl::style::VectorSource>()->VectorSource::getURL();
        return url ? jni::Make<jni::String>(env, *url) : jni::String();
    }

    jni::Array<jni::Object<geojson::Feature>> VectorSource::querySourceFeatures(jni::JNIEnv& env,
                                                                             jni::Array<jni::String> jSourceLayerIds,
                                                                             jni::Array<jni::Object<>> jfilter) {
        using namespace mbgl::android::conversion;
        using namespace mbgl::android::geojson;

        std::vector<mbgl::Feature> features;
        if (rendererFrontend) {
            features = rendererFrontend->querySourceFeatures(source.getID(), { toVector(env, jSourceLayerIds), toFilter(env, jfilter) });
        }
        return *convert<jni::Array<jni::Object<Feature>>, std::vector<mbgl::Feature>>(env, features);
    }

    jni::Class<VectorSource> VectorSource::javaClass;

    jni::Object<Source> VectorSource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = VectorSource::javaClass.template GetConstructor<jni::jlong>(env);
        return jni::Object<Source>(VectorSource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this)).Get());
    }

    void VectorSource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        VectorSource::javaClass = *jni::Class<VectorSource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<VectorSource>(
            env, VectorSource::javaClass, "nativePtr",
            std::make_unique<VectorSource, JNIEnv&, jni::String, jni::Object<>>,
            "initialize",
            "finalize",
            METHOD(&VectorSource::querySourceFeatures, "querySourceFeatures"),
            METHOD(&VectorSource::getURL, "nativeGetUrl")
        );
    }

} // namespace android
} // namespace mbgl
