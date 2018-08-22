#include "connectivity_listener.hpp"

#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/logging.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

    /**
     * Invoked when the construction is initiated from the jvm
     */
    ConnectivityListener::ConnectivityListener(jni::JNIEnv&) {}

    ConnectivityListener::~ConnectivityListener() {}

    void ConnectivityListener::onConnectivityStateChanged(jni::JNIEnv&, jni::jboolean connected) {
        NetworkStatus::Set(connected ? NetworkStatus::Status::Online : NetworkStatus::Status::Offline);
    }

    void ConnectivityListener::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<ConnectivityListener>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<ConnectivityListener>(
            env,
            javaClass,
            "nativePtr",
            std::make_unique<ConnectivityListener, JNIEnv&>,
            "initialize",
            "finalize",
            METHOD(&ConnectivityListener::onConnectivityStateChanged, "nativeOnConnectivityStateChanged")
        );
    }

} // namespace android
} // namespace mbgl
