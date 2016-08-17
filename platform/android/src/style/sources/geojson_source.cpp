#include "geojson_source.hpp"

#include "../android_conversion.hpp"
#include "../conversion/geojson.hpp"
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson_options.hpp>

#include <string>

namespace mbgl {
namespace android {

    GeoJSONSource::GeoJSONSource(jni::JNIEnv& env, jni::String sourceId, jni::Object<> options)
        : Source(env, std::make_unique<mbgl::style::GeoJSONSource>(
                jni::Make<std::string>(env, sourceId),
                options ? *style::conversion::convert<style::GeoJSONOptions>(Value(env, options)) : style::GeoJSONOptions()
                )
            ) {
    }

    GeoJSONSource::GeoJSONSource(mbgl::Map& map, mbgl::style::GeoJSONSource& coreSource)
        : Source(map, coreSource) {
    }

    GeoJSONSource::~GeoJSONSource() = default;

    void GeoJSONSource::setGeoJSON(jni::JNIEnv& env, jni::Object<> json) {
        using namespace mbgl::style::conversion;

        //Convert the jni object
        Result<GeoJSON> converted = convert<GeoJSON>(Value(env, json));
        if(!converted) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting geo json: " + converted.error().message);
            return;
        }

        //Update the core source
        source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setGeoJSON(*converted);

        //Repaint
        updateStyle(false);
    }

    void GeoJSONSource::setURL(jni::JNIEnv& env, jni::String url) {
        //Update the core source
        source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setURL(jni::Make<std::string>(env, url));

        //Repaint
        updateStyle(false);
    }

    jni::Class<GeoJSONSource> GeoJSONSource::javaClass;

    jni::jobject* GeoJSONSource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = GeoJSONSource::javaClass.template GetConstructor<jni::jlong>(env);
        return GeoJSONSource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void GeoJSONSource::registerNative(jni::JNIEnv& env) {
        //Lookup the class
        GeoJSONSource::javaClass = *jni::Class<GeoJSONSource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<GeoJSONSource>(
            env, GeoJSONSource::javaClass, "nativePtr",
            std::make_unique<GeoJSONSource, JNIEnv&, jni::String, jni::Object<>>,
            "initialize",
            "finalize",
            METHOD(&GeoJSONSource::setGeoJSON, "nativeSetGeoJson"),
            METHOD(&GeoJSONSource::setURL, "nativeSetUrl")
        );
    }

} // namespace android
} // namespace mbgl
