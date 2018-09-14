#pragma once

#include "source.hpp"
#include <mbgl/style/sources/geojson_source.hpp>
#include "../../geojson/geometry.hpp"
#include "../../geojson/feature.hpp"
#include "../../geojson/feature_collection.hpp"
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

using Callback = std::function<void (GeoJSON)>;

struct FeatureConverter {
    void convertJson(std::shared_ptr<std::string>, ActorRef<Callback>);

    template <class JNIType>
    void convertObject(std::shared_ptr<jni::Global<jni::Object<JNIType>, jni::EnvAttachingDeleter>>, ActorRef<Callback>);
};

struct Update {
    using Converter = std::function<void (ActorRef<Callback>)>;
    Converter converterFn;

    std::unique_ptr<Actor<Callback>> callback;

    Update(Converter, std::unique_ptr<Actor<Callback>>);
};

class GeoJSONSource : public Source {
public:
    using SuperTag = Source;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/GeoJsonSource"; };

    static void registerNative(jni::JNIEnv&);

    GeoJSONSource(jni::JNIEnv&, const jni::String&, const jni::Object<>&);
    GeoJSONSource(jni::JNIEnv&, mbgl::style::Source&, AndroidRendererFrontend&);
    ~GeoJSONSource();

private:
    void setGeoJSONString(jni::JNIEnv&, const jni::String&);
    void setFeatureCollection(jni::JNIEnv&, const jni::Object<geojson::FeatureCollection>&);
    void setFeature(jni::JNIEnv&, const jni::Object<geojson::Feature>&);
    void setGeometry(jni::JNIEnv&, const jni::Object<geojson::Geometry>&);
    void setURL(jni::JNIEnv&, const jni::String&);

    jni::Local<jni::Array<jni::Object<geojson::Feature>>> querySourceFeatures(jni::JNIEnv&,
                                                                  const jni::Array<jni::Object<>>&);

    jni::Local<jni::String> getURL(jni::JNIEnv&);

    jni::Local<jni::Object<Source>> createJavaPeer(jni::JNIEnv&);
    std::unique_ptr<Update> awaitingUpdate;
    std::unique_ptr<Update> update;
    std::shared_ptr<ThreadPool> threadPool;
    std::unique_ptr<Actor<FeatureConverter>> converter;

    template <class JNIType>
    void setCollectionAsync(jni::JNIEnv&, const jni::Object<JNIType>&);

    void setAsync(Update::Converter);

}; // class GeoJSONSource

} // namespace android
} // namespace mbgl
