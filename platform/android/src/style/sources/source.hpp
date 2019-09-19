#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/style/source.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/logging.hpp>

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

    inline void source(std::unique_ptr<mbgl::style::Source> source) noexcept {
        ownedSource_ = std::move(source);
    }

    inline void source(mbgl::style::Source *source) noexcept {
        nonOwnedSource_ = source->makeWeakPtr();
    }

    inline mbgl::style::Source* source(optional<jni::JNIEnv&> env = nullopt) {
        if (ownedSource_) return ownedSource_.get();

        const auto msg = "The source got invalidated after the style change";
        if (!nonOwnedSource_) {
            if (env) {
                jni::ThrowNew(*env, jni::FindClass(*env, "java/lang/IllegalStateException"), msg);
            } else {
                mbgl::Log::Error(mbgl::Event::JNI, msg);
            }
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
    mapbox::base::WeakPtrFactory<Source> weakFactory_ {this};
};

} // namespace android
} // namespace mbgl
