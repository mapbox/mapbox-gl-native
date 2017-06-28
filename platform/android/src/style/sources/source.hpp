#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/source.hpp>

#include "../value.hpp"
#include "../../android_renderer_frontend.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class Source : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/Source"; };

    static jni::Class<Source> javaClass;

    static void registerNative(jni::JNIEnv&);

    /*
     * Called when a Java object is created on the c++ side
     */
    Source(mbgl::style::Source&);

    /*
     * Called when a Java object was created from the jvm side
     */
    Source(jni::JNIEnv&, std::unique_ptr<mbgl::style::Source>);

    virtual ~Source();

    /**
     * Set core source (ie return ownership after remove)
     */
    void setSource(std::unique_ptr<style::Source>);

    style::Source& get();

    void addToMap(mbgl::Map&);

    void setRendererFrontend(AndroidRendererFrontend&);

    virtual jni::jobject* createJavaPeer(jni::JNIEnv&) = 0;

    jni::String getId(jni::JNIEnv&);

    jni::String getAttribution(jni::JNIEnv&);

protected:
    // Release the owned view and return it
    std::unique_ptr<mbgl::style::Source> releaseCoreSource();

    // Set on newly created sources until added to the map
    std::unique_ptr<mbgl::style::Source> ownedSource;

    // Raw pointer that is valid until the source is removed from the map
    mbgl::style::Source& source;

    // RendererFrontend pointer is valid only when
    // added to the map
    AndroidRendererFrontend* rendererFrontend;
};

} // namespace android
} // namespace mbgl
