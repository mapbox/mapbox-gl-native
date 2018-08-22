#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>
#include <mbgl/style/image.hpp>

namespace mbgl {
namespace android {

class Image : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/maps/Image"; };

    static mbgl::style::Image getImage(jni::JNIEnv&, const jni::Object<Image>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl