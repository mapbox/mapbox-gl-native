#include "custom_geometry_source.hpp"

#include <mbgl/renderer/query.hpp>

// Java -> C++ conversion
#include "../android_conversion.hpp"
#include "../conversion/filter.hpp"

// C++ -> Java conversion
#include "../../conversion/conversion.hpp"
#include "../../conversion/collection.hpp"
#include "../../geojson/conversion/feature.hpp"
#include <mbgl/style/conversion/custom_geometry_source_options.hpp>

#include <string>

namespace mbgl {
namespace android {

    // This conversion is expected not to fail because it's used only in contexts where
    // the value was originally a CustomGeometrySourceOptions object on the Java side. If it fails
    // to convert, it's a bug in our serialization or Java-side static typing.
    static style::CustomGeometrySource::Options convertCustomGeometrySourceOptions(jni::JNIEnv& env,
                                                                                   jni::Object<> options,
                                                                                   style::TileFunction fetchFn,
                                                                                   style::TileFunction cancelFn) {
        using namespace mbgl::style::conversion;
        if (!options) {
            return style::CustomGeometrySource::Options();
        }
        Error error;
        optional<style::CustomGeometrySource::Options> result = convert<style::CustomGeometrySource::Options>(Value(env, options), error);
        if (!result) {
            throw std::logic_error(error.message);
        }
        result->fetchTileFunction = fetchFn;
        result->cancelTileFunction = cancelFn;
        return *result;
    }

    CustomGeometrySource::CustomGeometrySource(jni::JNIEnv& env,
                                               jni::String sourceId,
                                               jni::Object<> options)
        : Source(env, std::make_unique<mbgl::style::CustomGeometrySource>(
                         jni::Make<std::string>(env, sourceId),
                         convertCustomGeometrySourceOptions(env, options,
                                 std::bind(&CustomGeometrySource::fetchTile, this, std::placeholders::_1),
                                 std::bind(&CustomGeometrySource::cancelTile, this, std::placeholders::_1)))) {
    }

    CustomGeometrySource::CustomGeometrySource(jni::JNIEnv& env,
                                               mbgl::style::Source& coreSource,
                                               AndroidRendererFrontend& frontend)
        : Source(env, coreSource, createJavaPeer(env), frontend) {
    }

    CustomGeometrySource::~CustomGeometrySource() = default;

    void CustomGeometrySource::fetchTile (const mbgl::CanonicalTileID& tileID) {
        android::UniqueEnv _env = android::AttachEnv();

        static auto fetchTile = javaClass.GetMethod<void (jni::jint, jni::jint, jni::jint)>(*_env, "fetchTile");

        assert(javaPeer);

        auto peer = jni::Cast(*_env, *javaPeer, javaClass);
        peer.Call(*_env, fetchTile, (int)tileID.z, (int)tileID.x, (int)tileID.y);
    };

    void CustomGeometrySource::cancelTile(const mbgl::CanonicalTileID& tileID) {
        android::UniqueEnv _env = android::AttachEnv();

        static auto cancelTile = javaClass.GetMethod<void (jni::jint, jni::jint, jni::jint)>(*_env, "cancelTile");

        assert(javaPeer);

        auto peer = jni::Cast(*_env, *javaPeer, javaClass);
        peer.Call(*_env, cancelTile, (int)tileID.z, (int)tileID.x, (int)tileID.y);
    };

    void CustomGeometrySource::setTileData(jni::JNIEnv& env,
                                           jni::jint z,
                                           jni::jint x,
                                           jni::jint y,
                                           jni::Object<geojson::FeatureCollection> jFeatures) {
        using namespace mbgl::android::geojson;

        // Convert the jni object
        auto geometry = geojson::FeatureCollection::convert(env, jFeatures);

        // Update the core source
        source.as<mbgl::style::CustomGeometrySource>()->CustomGeometrySource::setTileData(CanonicalTileID(z, x, y), GeoJSON(geometry));
    }

    void CustomGeometrySource::invalidateTile(jni::JNIEnv&, jni::jint z, jni::jint x, jni::jint y) {
        source.as<mbgl::style::CustomGeometrySource>()->CustomGeometrySource::invalidateTile(CanonicalTileID(z, x, y));
    }

    void CustomGeometrySource::invalidateBounds(jni::JNIEnv& env, jni::Object<LatLngBounds> jBounds) {
        auto bounds = LatLngBounds::getLatLngBounds(env, jBounds);
        source.as<mbgl::style::CustomGeometrySource>()->CustomGeometrySource::invalidateRegion(bounds);
    }

    jni::Array<jni::Object<geojson::Feature>> CustomGeometrySource::querySourceFeatures(jni::JNIEnv& env,
                                                                        jni::Array<jni::Object<>> jfilter) {
        using namespace mbgl::android::conversion;
        using namespace mbgl::android::geojson;

        std::vector<mbgl::Feature> features;
        if (rendererFrontend) {
            features = rendererFrontend->querySourceFeatures(source.getID(), { {},  toFilter(env, jfilter) });
        }
        return *convert<jni::Array<jni::Object<Feature>>, std::vector<mbgl::Feature>>(env, features);
    }

    jni::Class<CustomGeometrySource> CustomGeometrySource::javaClass;

    jni::Object<Source> CustomGeometrySource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = CustomGeometrySource::javaClass.template GetConstructor<jni::jlong>(env);
        return jni::Object<Source>(CustomGeometrySource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this)).Get());
    }

    void CustomGeometrySource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        CustomGeometrySource::javaClass = *jni::Class<CustomGeometrySource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<CustomGeometrySource>(
            env, CustomGeometrySource::javaClass, "nativePtr",
            std::make_unique<CustomGeometrySource, JNIEnv&, jni::String, jni::Object<>>,
            "initialize",
            "finalize",
            METHOD(&CustomGeometrySource::querySourceFeatures, "querySourceFeatures"),
            METHOD(&CustomGeometrySource::setTileData, "nativeSetTileData"),
            METHOD(&CustomGeometrySource::invalidateTile, "nativeInvalidateTile"),
            METHOD(&CustomGeometrySource::invalidateBounds, "nativeInvalidateBounds")
        );
    }

} // namespace android
} // namespace mbgl
