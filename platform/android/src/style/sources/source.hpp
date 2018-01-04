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

    static jni::Object<Source> peerForCoreSource(jni::JNIEnv&, mbgl::style::Source&, AndroidRendererFrontend&);

    /*
     * Called when a Java object is created for a core source that belongs to a map.
     */
    Source(jni::JNIEnv&, mbgl::style::Source&, jni::Object<Source>, AndroidRendererFrontend&);

    /*
     * Called when a Java object is created for a new core source that does not belong to a map.
     */
    Source(jni::JNIEnv&, std::unique_ptr<mbgl::style::Source>);

    virtual ~Source();

    void addToMap(JNIEnv&, jni::Object<Source>, mbgl::Map&, AndroidRendererFrontend&);

    void removeFromMap(JNIEnv&, jni::Object<Source>, mbgl::Map&);

    jni::String getId(jni::JNIEnv&);

    jni::String getAttribution(jni::JNIEnv&);

protected:
    // Set on newly created sources until added to the map.
    std::unique_ptr<mbgl::style::Source> ownedSource;

    // Raw pointer that is valid at all times.
    mbgl::style::Source& source;

    // Set when the source is added to a map.
    jni::UniqueObject<Source> javaPeer;

    // RendererFrontend pointer is valid only when added to the map.
    AndroidRendererFrontend* rendererFrontend { nullptr };
};

} // namespace android
} // namespace mbgl
