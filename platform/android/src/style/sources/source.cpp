#include "source.hpp"
#include "../android_conversion.hpp"

#include <jni/jni.hpp>

#include <mbgl/platform/log.hpp>

//Java -> C++ conversion
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/source.hpp>

//C++ -> Java conversion
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

    Source::Source(mbgl::Map& coreMap, mbgl::style::Source& coreSource) : source(coreSource) , map(&coreMap) {
    }

    Source::~Source() {
    }

    jni::String Source::getId(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, source.getID());
    }

    std::unique_ptr<mbgl::style::Source> Source::releaseCoreSource() {
        assert(ownedSource != nullptr);
        return std::move(ownedSource);
    }

    jni::Class<Source> Source::javaClass;

    void Source::registerNative(jni::JNIEnv& env) {
        //Lookup the class
        Source::javaClass = *jni::Class<Source>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<Source>(env, Source::javaClass, "nativePtr",
            METHOD(&Source::getId, "nativeGetId")
        );

    }

} //android
} //mbgl