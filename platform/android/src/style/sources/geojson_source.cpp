#include "geojson_source.hpp"

#include <mbgl/renderer/query.hpp>

// Java -> C++ conversion
#include "../android_conversion.hpp"
#include "../conversion/filter.hpp"
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/conversion/geojson_options.hpp>
#include <mbgl/style/conversion_impl.hpp>

// C++ -> Java conversion
#include "../../conversion/conversion.hpp"
#include "../../conversion/collection.hpp"
#include "../../geojson/conversion/feature.hpp"
#include "../conversion/url_or_tileset.hpp"

#include <string>
#include <mbgl/util/shared_thread_pool.hpp>

// GeoJSONSource uses a "coalescing" model for high frequency asynchronous data update calls,
// which in practice means, that any update that started processing is going to finish
// and the last scheduled update is going to finish as well. Any updates scheduled during processing can be canceled.
// Conversion from Java features to core ones is done on a worker thread and once finished,
// the ownership of the converted features is returned to the calling thread.
namespace mbgl {
namespace android {

    // This conversion is expected not to fail because it's used only in contexts where
    // the value was originally a GeoJsonOptions object on the Java side. If it fails
    // to convert, it's a bug in our serialization or Java-side static typing.
    static style::GeoJSONOptions convertGeoJSONOptions(jni::JNIEnv& env, jni::Object<> options) {
        using namespace mbgl::style::conversion;
        if (!options) {
            return style::GeoJSONOptions();
        }
        Error error;
        optional<style::GeoJSONOptions> result = convert<style::GeoJSONOptions>(mbgl::android::Value(env, options), error);
        if (!result) {
            throw std::logic_error(error.message);
        }
        return *result;
    }

    GeoJSONSource::GeoJSONSource(jni::JNIEnv& env, jni::String sourceId, jni::Object<> options)
        : Source(env, std::make_unique<mbgl::style::GeoJSONSource>(
                jni::Make<std::string>(env, sourceId),
                convertGeoJSONOptions(env, options)))
        , threadPool(sharedThreadPool())
        , converter(std::make_unique<Actor<FeatureConverter>>(*threadPool)) {
    }

    GeoJSONSource::GeoJSONSource(jni::JNIEnv& env,
                                 mbgl::style::Source& coreSource,
                                 AndroidRendererFrontend& frontend)
        : Source(env, coreSource, createJavaPeer(env), frontend)
        , threadPool(sharedThreadPool())
        , converter(std::make_unique<Actor<FeatureConverter>>(*threadPool)) {
    }

    GeoJSONSource::~GeoJSONSource() = default;

    void GeoJSONSource::setGeoJSONString(jni::JNIEnv& env, jni::String jString) {

        std::shared_ptr<std::string> json = std::make_shared<std::string>(jni::Make<std::string>(env, jString));

        Update::Converter converterFn = [this, json](ActorRef<Callback> _callback) {
            converter->self().invoke(&FeatureConverter::convertJson, json, _callback);
        };

        setAsync(converterFn);
    }

    void GeoJSONSource::setFeatureCollection(jni::JNIEnv& env, jni::Object<geojson::FeatureCollection> jFeatures) {
        setCollectionAsync(env, jFeatures);
    }

    void GeoJSONSource::setFeature(jni::JNIEnv& env, jni::Object<geojson::Feature> jFeature) {
        setCollectionAsync(env, jFeature);
    }

    void GeoJSONSource::setGeometry(jni::JNIEnv& env, jni::Object<geojson::Geometry> jGeometry) {
        setCollectionAsync(env, jGeometry);
    }

    void GeoJSONSource::setURL(jni::JNIEnv& env, jni::String url) {
        // Update the core source
        source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setURL(jni::Make<std::string>(env, url));
    }

    jni::String GeoJSONSource::getURL(jni::JNIEnv& env) {
        optional<std::string> url = source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::getURL();
        return url ? jni::Make<jni::String>(env, *url) : jni::String();
    }

    jni::Array<jni::Object<geojson::Feature>> GeoJSONSource::querySourceFeatures(jni::JNIEnv& env,
                                                                        jni::Array<jni::Object<>> jfilter) {
        using namespace mbgl::android::conversion;
        using namespace mbgl::android::geojson;

        std::vector<mbgl::Feature> features;
        if (rendererFrontend) {
            features = rendererFrontend->querySourceFeatures(source.getID(), { {},  toFilter(env, jfilter) });
        }
        return *convert<jni::Array<jni::Object<Feature>>, std::vector<mbgl::Feature>>(env, features);
    }

    jni::Class<GeoJSONSource> GeoJSONSource::javaClass;

    jni::Object<Source> GeoJSONSource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = GeoJSONSource::javaClass.template GetConstructor<jni::jlong>(env);
        return jni::Object<Source>(GeoJSONSource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this)).Get());
    }

    template <class JNIType>
    void GeoJSONSource::setCollectionAsync(jni::JNIEnv& env, jni::Object<JNIType> jObject) {

        std::shared_ptr<jni::jobject> object = std::shared_ptr<jni::jobject>(jObject.NewGlobalRef(env).release()->Get(), GenericGlobalRefDeleter());

        Update::Converter converterFn = [this, object](ActorRef<Callback> _callback) {
            converter->self().invoke(&FeatureConverter::convertObject<JNIType>, jni::Object<JNIType>(*object), _callback);
        };

        setAsync(converterFn);
    }

    void GeoJSONSource::setAsync(Update::Converter converterFn) {
        awaitingUpdate = std::make_unique<Update>(
                std::move(converterFn),
                std::make_unique<Actor<Callback>>(
                        *Scheduler::GetCurrent(),
                        [this](GeoJSON geoJSON) {
                            // conversion from Java features to core ones finished
                            android::UniqueEnv _env = android::AttachEnv();

                            // Update the core source
                            source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setGeoJSON(geoJSON);

                            // if there is an awaiting update, execute it, otherwise, release resources
                            if (awaitingUpdate) {
                                update = std::move(awaitingUpdate);
                                update->converterFn(update->callback->self());
                            } else {
                                update.reset();
                            }
                        })
        );

        // If another update is running, wait
        if (update) {
            return;
        }

        // no updates are being processed, execute this one
        update = std::move(awaitingUpdate);
        update->converterFn(update->callback->self());
    }

    void GeoJSONSource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        GeoJSONSource::javaClass = *jni::Class<GeoJSONSource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<GeoJSONSource>(
            env, GeoJSONSource::javaClass, "nativePtr",
            std::make_unique<GeoJSONSource, JNIEnv&, jni::String, jni::Object<>>,
            "initialize",
            "finalize",
            METHOD(&GeoJSONSource::setGeoJSONString, "nativeSetGeoJsonString"),
            METHOD(&GeoJSONSource::setFeatureCollection, "nativeSetFeatureCollection"),
            METHOD(&GeoJSONSource::setFeature, "nativeSetFeature"),
            METHOD(&GeoJSONSource::setGeometry, "nativeSetGeometry"),
            METHOD(&GeoJSONSource::setURL, "nativeSetUrl"),
            METHOD(&GeoJSONSource::getURL, "nativeGetUrl"),
            METHOD(&GeoJSONSource::querySourceFeatures, "querySourceFeatures")
        );
    }

    void FeatureConverter::convertJson(std::shared_ptr<std::string> json,
                                       ActorRef<Callback> callback) {
        using namespace mbgl::style::conversion;

        android::UniqueEnv _env = android::AttachEnv();

        // Convert the jni object
        Error error;
        optional<GeoJSON> converted = parseGeoJSON(*json, error);
        if(!converted) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting geo json: " + error.message);
            return;
        }

        callback.invoke(&Callback::operator(), *converted);
    }

    template<class JNIType>
    void FeatureConverter::convertObject(jni::Object<JNIType> jObject, ActorRef<Callback> callback) {
        using namespace mbgl::android::geojson;

        android::UniqueEnv _env = android::AttachEnv();
        // Convert the jni object
        auto geometry = JNIType::convert(*_env, jObject);
        callback.invoke(&Callback::operator(), GeoJSON(geometry));
    }

    Update::Update(Converter _converterFn, std::unique_ptr<Actor<Callback>> _callback)
            : converterFn(std::move(_converterFn))
            , callback(std::move(_callback)) {}

} // namespace android
} // namespace mbgl
