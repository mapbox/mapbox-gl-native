#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/source.hpp>

#include "../value.hpp"
#include "../../android_renderer_frontend.hpp"

#include <mapbox/weak.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class Source : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/Source"; };

    static void registerNative(jni::JNIEnv&);

    static const jni::Object<Source>& peerForCoreSource(jni::JNIEnv&, mbgl::style::Source*, AndroidRendererFrontend&);

    /*
     * Called when a Java object is created for a core source that belongs to a map.
     */
    Source(jni::JNIEnv&, mbgl::style::Source*, const jni::Object<Source>&, AndroidRendererFrontend&);

    /*
     * Called when a Java object is created for a new core source that does not belong to a map.
     */
    Source(jni::JNIEnv&, std::unique_ptr<mbgl::style::Source>);

    virtual ~Source();

    virtual void addToMap(JNIEnv&, const jni::Object<Source>&, mbgl::Map&, AndroidRendererFrontend&);

    virtual bool removeFromMap(JNIEnv&, const jni::Object<Source>&, mbgl::Map&);

    void releaseJavaPeer();

    jni::Local<jni::String> getId(jni::JNIEnv&);

    jni::Local<jni::String> getAttribution(jni::JNIEnv&);

protected:

    inline void source(std::unique_ptr<mbgl::style::Source> ownedSource) noexcept {
        ownedSource_ = std::move(ownedSource);
    }

    inline void source(mbgl::style::Source *coreSource) noexcept {
        nonOwnedSource_ = coreSource->makeWeakPtr();
    }

    inline mbgl::style::Source* source(jni::JNIEnv& env) {
        if (ownedSource_) return ownedSource_.get();
        if (!nonOwnedSource_) {
            jni::ThrowNew(env, jni::FindClass(env, "java/lang/IllegalStateException"),
                          "This source got invalidated after the style change");
        }
        return nonOwnedSource_.get();
    }

    // Set when the source is added to a map.
    jni::Global<jni::Object<Source>> javaPeer;

    // RendererFrontend pointer is valid only when added to the map.
    AndroidRendererFrontend* rendererFrontend { nullptr };

private:
    std::unique_ptr<mbgl::style::Source> ownedSource_ { nullptr };
    mapbox::base::WeakPtr<mbgl::style::Source> nonOwnedSource_;
};

} // namespace android
} // namespace mbgl
