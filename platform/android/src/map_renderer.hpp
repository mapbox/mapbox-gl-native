#pragma once

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/renderer/renderer.hpp>

#include <memory>
#include <utility>

#include <jni/jni.hpp>

#include "jni/generic_global_ref_deleter.hpp"

namespace mbgl {

class Renderer;
class RendererBackend;
class ThreadPool;
class UpdateParameters;

namespace android {

class AndroidRendererBackend;
class FileSource;

/**
 * The MapRenderer is a peer class that encapsulates the actions
 * performed on the GL Thread.
 *
 * The public methods are safe to call from the main thread, others are not.
 */
class MapRenderer : public Scheduler {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/maps/renderer/MapRenderer"; };

    static jni::Class<MapRenderer> javaClass;

    static void registerNative(jni::JNIEnv&);

    static MapRenderer& getNativePeer(JNIEnv&, jni::Object<MapRenderer>);

    MapRenderer(jni::JNIEnv& _env,
                jni::Object<MapRenderer>,
                jni::Object<FileSource>,
                jni::jfloat pixelRatio,
                jni::String programCacheDir);

    ~MapRenderer() override;

    // Resets the renderer to clean up on the calling thread
    void reset();

    // Sets the new update parameters to use on subsequent
    // renders. Be sure to trigger a render with
    // requestRender().
    void update(std::shared_ptr<UpdateParameters>);

    // Gives a handle to the Renderer to enable actions on
    // any thread.
    ActorRef<Renderer> actor() const;

    // From Scheduler. Schedules by using callbacks to the
    // JVM to process the mailbox on the right thread.
    void schedule(std::weak_ptr<Mailbox> scheduled) override;

    void requestRender();

private:
    // Called from the GL Thread //

    // Renders a frame.
    void render(JNIEnv&);

    void onSurfaceCreated(JNIEnv&);

    void onSurfaceChanged(JNIEnv&, jint width, jint height);

private:
    GenericUniqueWeakObject<MapRenderer> javaPeer;
    std::shared_ptr<ThreadPool> threadPool;
    std::unique_ptr<AndroidRendererBackend> backend;
    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<Mailbox> mailbox;
    ActorRef<Renderer> rendererRef;

    std::shared_ptr<UpdateParameters> updateParameters;
    std::mutex updateMutex;

    bool framebufferSizeChanged = false;
};

} // namespace android
} // namespace mbgl
