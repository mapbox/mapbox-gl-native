#pragma once

#include <jni/jni.hpp>

#include <memory>

namespace mbgl {
namespace android {

template <typename T>
class Peer {
public:
    static constexpr auto Name();
    static void RegisterNative(jni::JNIEnv&);

    static T& Get(jni::JNIEnv& env, const jni::Object<Peer<T>>& obj) {
        jni::Field<Peer, jlong> field{ env, javaClass, peer };
        return *reinterpret_cast<Peer*>(obj.Get(env, field))->native;
    }

protected:
    template <typename... Args>
    Peer(Args&&... args) : native(std::make_unique<T>(std::forward<Args>(args)...)) {
    }

    const std::unique_ptr<T> native;

private:
    static const char* peer;
    static jni::Class<Peer<T>> javaClass;
};

template <typename T>
jni::Class<Peer<T>> Peer<T>::javaClass;

template <typename T>
const char* Peer<T>::peer = "peer";

} // namespace android
} // namespace mbgl
