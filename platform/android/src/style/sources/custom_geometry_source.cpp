#include "custom_geometry_source.hpp"
#include "../../attach_env.hpp"

#include <mbgl/renderer/query.hpp>

// Java -> C++ conversion
#include "../android_conversion.hpp"
#include "../conversion/filter.hpp"

// C++ -> Java conversion
#include "../../conversion/conversion.hpp"
#include "../../conversion/collection.hpp"
#include "../../geojson/feature.hpp"
#include <mbgl/style/conversion/custom_geometry_source_options.hpp>

#include <string>

namespace mbgl {
namespace android {

    // This conversion is expected not to fail because it's used only in contexts where
    // the value was originally a CustomGeometrySourceOptions object on the Java side. If it fails
    // to convert, it's a bug in our serialization or Java-side static typing.
    static style::CustomGeometrySource::Options convertCustomGeometrySourceOptions(jni::JNIEnv& env,
                                                                                   jni::Local<jni::Object<>> options,
                                                                                   style::TileFunction fetchFn,
                                                                                   style::TileFunction cancelFn) {
        using namespace mbgl::style::conversion;
        if (!options) {
            return style::CustomGeometrySource::Options();
        }
        Error error;
        optional<style::CustomGeometrySource::Options> result = convert<style::CustomGeometrySource::Options>(Value(env, std::move(options)), error);
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
                         convertCustomGeometrySourceOptions(env, jni::SeizeLocal(env, std::move(options)),
                                 std::bind(&CustomGeometrySource::fetchTile, this, std::placeholders::_1),
                                 std::bind(&CustomGeometrySource::cancelTile, this, std::placeholders::_1)))) {
    }

    CustomGeometrySource::CustomGeometrySource(jni::JNIEnv& env,
                                               mbgl::style::Source& coreSource,
                                               AndroidRendererFrontend& frontend)
        : Source(env, coreSource, createJavaPeer(env), frontend) {
    }

    CustomGeometrySource::~CustomGeometrySource() {
        releaseThreads();
    }

    void CustomGeometrySource::fetchTile (const mbgl::CanonicalTileID& tileID) {
        android::UniqueEnv _env = android::AttachEnv();

        static auto javaClass = jni::Class<CustomGeometrySource>::Singleton(*_env);
        static auto fetchTile = javaClass.GetMethod<void (jni::jint, jni::jint, jni::jint)>(*_env, "fetchTile");

        assert(javaPeer);

        auto peer = jni::Cast(*_env, javaClass, *javaPeer);
        peer.Call(*_env, fetchTile, (int)tileID.z, (int)tileID.x, (int)tileID.y);
    };

    void CustomGeometrySource::cancelTile(const mbgl::CanonicalTileID& tileID) {
        android::UniqueEnv _env = android::AttachEnv();

        static auto javaClass = jni::Class<CustomGeometrySource>::Singleton(*_env);
        static auto cancelTile = javaClass.GetMethod<void (jni::jint, jni::jint, jni::jint)>(*_env, "cancelTile");

        assert(javaPeer);

        auto peer = jni::Cast(*_env, javaClass, *javaPeer);
        peer.Call(*_env, cancelTile, (int)tileID.z, (int)tileID.x, (int)tileID.y);
    };

    void CustomGeometrySource::startThreads() {
        android::UniqueEnv _env = android::AttachEnv();

        static auto javaClass = jni::Class<CustomGeometrySource>::Singleton(*_env);
        static auto startThreads = javaClass.GetMethod<void ()>(*_env, "startThreads");

        assert(javaPeer);

        auto peer = jni::Cast(*_env, javaClass, *javaPeer);
        peer.Call(*_env, startThreads);
    }

    void CustomGeometrySource::releaseThreads() {
        android::UniqueEnv _env = android::AttachEnv();

        static auto javaClass = jni::Class<CustomGeometrySource>::Singleton(*_env);
        static auto releaseThreads = javaClass.GetMethod<void ()>(*_env, "releaseThreads");

        assert(javaPeer);

        auto peer = jni::Cast(*_env, javaClass, *javaPeer);
        peer.Call(*_env, releaseThreads);
    };

    bool CustomGeometrySource::isCancelled(jni::jint z,
                                                jni::jint x,
                                                jni::jint y) {
        android::UniqueEnv _env = android::AttachEnv();

        static auto javaClass = jni::Class<CustomGeometrySource>::Singleton(*_env);
        static auto isCancelled = javaClass.GetMethod<jboolean (jni::jint, jni::jint, jni::jint)>(*_env, "isCancelled");

        assert(javaPeer);

        auto peer = jni::Cast(*_env, javaClass, *javaPeer);
        return peer.Call(*_env, isCancelled, z, x, y);
    };

    void CustomGeometrySource::setTileData(jni::JNIEnv& env,
                                           jni::jint z,
                                           jni::jint x,
                                           jni::jint y,
                                           jni::Object<geojson::FeatureCollection> jFeatures) {
        using namespace mbgl::android::geojson;

        // Convert the jni object
        auto geometry = geojson::FeatureCollection::convert(env, jFeatures);

        // Update the core source if not cancelled
        if (!isCancelled(z, x ,y)) {
            source.as<mbgl::style::CustomGeometrySource>()->CustomGeometrySource::setTileData(CanonicalTileID(z, x, y), GeoJSON(geometry));
        }
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
            features = rendererFrontend->querySourceFeatures(source.getID(),
                { {},  toFilter(env, jni::SeizeLocal(env, std::move(jfilter))) });
        }
        return Feature::convert(env, features);
    }

    jni::Object<Source> CustomGeometrySource::createJavaPeer(jni::JNIEnv& env) {
        static auto javaClass = jni::Class<CustomGeometrySource>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::jlong>(env);
        return jni::Object<Source>(javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this)).Get());
    }

    void CustomGeometrySource::addToMap(JNIEnv& env, jni::Object<Source> obj, mbgl::Map& map, AndroidRendererFrontend& frontend) {
        Source::addToMap(env, obj, map, frontend);
        startThreads();
    }

    bool CustomGeometrySource::removeFromMap(JNIEnv& env, jni::Object<Source> source, mbgl::Map& map) {
        bool successfullyRemoved = Source::removeFromMap(env, source, map);
        if (successfullyRemoved) {
            releaseThreads();
        }
        return successfullyRemoved;
    }

    void CustomGeometrySource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto javaClass = jni::Class<CustomGeometrySource>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<CustomGeometrySource>(
            env, javaClass, "nativePtr",
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
