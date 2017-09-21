#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class Bitmap;

class Snapshot : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/snapshotter/Snapshot"; };

    static jni::Object<Snapshot> New(jni::JNIEnv&, jni::Object<Bitmap> image);

    static jni::Class<Snapshot> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
