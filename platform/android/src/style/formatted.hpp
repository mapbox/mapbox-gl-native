#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>
#include <mbgl/style/expression/formatted.hpp>

namespace mbgl {
    namespace android {

        using SuperTag = jni::ObjectTag;
        class Formatted : private mbgl::util::noncopyable {
        public:
            static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/types/Formatted"; };

            static jni::Local<jni::Object<Formatted>>
            New(jni::JNIEnv&, const style::expression::Formatted &value);

            static void registerNative(jni::JNIEnv &);
        };

    } // namespace android
} // namespace mbgl