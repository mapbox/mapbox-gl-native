#include "source.hpp"
#include "../android_conversion.hpp"

#include <jni/jni.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/util/logging.hpp>

// Java -> C++ conversion
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/source.hpp>

// C++ -> Java conversion
#include "../conversion/property_value.hpp"

#include <string>

namespace mbgl {
namespace android {

    /**
     * Invoked when the construction is initiated from the jvm through a subclass
     */
    Source::Source(jni::JNIEnv&, std::unique_ptr<mbgl::style::Source> coreSource)
        : ownedSource(std::move(coreSource))
        , source(*ownedSource) {
    }

    Source::Source(mbgl::style::Source& coreSource)
            : source(coreSource) {
    }

    Source::~Source() = default;

    style::Source& Source::get() {
        return source;
    }

    void Source::setSource(std::unique_ptr<style::Source> coreSource) {
        this->ownedSource = std::move(coreSource);
    }

    jni::String Source::getId(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, source.getID());
    }

    jni::String Source::getAttribution(jni::JNIEnv& env) {
        auto attribution = source.getAttribution();
        return attribution ? jni::Make<jni::String>(env, attribution.value()) : jni::Make<jni::String>(env,"");
    }

    void Source::addToMap(mbgl::Map& _map) {
        // Check to see if we own the source first
        if (!ownedSource) {
            throw std::runtime_error("Cannot add source twice");
        }

        // Add source to map
        _map.getStyle().addSource(releaseCoreSource());
    }

    void Source::setRendererFrontend(AndroidRendererFrontend& frontend_) {
        rendererFrontend = &frontend_;
    }

    std::unique_ptr<mbgl::style::Source> Source::releaseCoreSource() {
        assert(ownedSource != nullptr);
        return std::move(ownedSource);
    }

    jni::Class<Source> Source::javaClass;

    void Source::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        Source::javaClass = *jni::Class<Source>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<Source>(env, Source::javaClass, "nativePtr",
            METHOD(&Source::getId, "nativeGetId"),
            METHOD(&Source::getAttribution, "nativeGetAttribution")
        );

    }

} // namespace android
} // namespace mbgl
