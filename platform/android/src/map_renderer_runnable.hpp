#pragma once

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/scheduler.hpp>

#include <memory>
#include <utility>

#include <jni/jni.hpp>

#include "jni/generic_global_ref_deleter.hpp"

namespace mbgl {
namespace android {

/**
 * The MapRendererRunnable is a peer class that encapsulates
 * a scheduled mailbox in a Java Runnable so it can be
 * scheduled on the map renderer thread.
 *
 */
class MapRendererRunnable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/maps/renderer/MapRendererRunnable"; };

    static jni::Class<MapRendererRunnable> javaClass;

    static void registerNative(jni::JNIEnv&);

    MapRendererRunnable(jni::JNIEnv&, std::weak_ptr<Mailbox>);

    // Only for jni registration, unused
    MapRendererRunnable(jni::JNIEnv&) {
        assert(false);
    }

    ~MapRendererRunnable();

    void run(jni::JNIEnv&);

    jni::Object<MapRendererRunnable> getPeer();

private:
    GenericUniqueWeakObject<MapRendererRunnable> javaPeer;
    std::weak_ptr<Mailbox> mailbox;
};

} // namespace android
} // namespace mbgl
