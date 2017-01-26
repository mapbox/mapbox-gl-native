#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class ConnectivityListener : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/net/NativeConnectivityListener"; };

    static jni::Class<ConnectivityListener> javaClass;

    static void registerNative(jni::JNIEnv&);

    /*
     * Called when a Java object was created from the jvm side
     */
    ConnectivityListener(jni::JNIEnv&);

    ~ConnectivityListener();

    void onConnectivityStateChanged(jni::JNIEnv&, jni::jboolean);

};

} // namespace android
} // namespace mbgl
