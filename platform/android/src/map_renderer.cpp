#include "map_renderer.hpp"

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/util/shared_thread_pool.hpp>

#include <string>

#include "attach_env.hpp"
#include "android_renderer_backend.hpp"
#include "map_renderer_runnable.hpp"
#include "file_source.hpp"

namespace mbgl {
namespace android {

MapRenderer::MapRenderer(jni::JNIEnv& _env,
                         jni::Object<MapRenderer> obj,
                         jni::Object<FileSource> fileSource,
                         jni::jfloat pixelRatio,
                         jni::String programCacheDir)
    : javaPeer(SeizeGenericWeakRef(_env, jni::Object<MapRenderer>(jni::NewWeakGlobalRef(_env, obj.Get()).release())))
    , threadPool(sharedThreadPool())
    , backend(std::make_unique<AndroidRendererBackend>())
    , renderer(std::make_unique<Renderer>(*backend, pixelRatio, FileSource::getDefaultFileSource(_env, fileSource),
                                          *threadPool, GLContextMode::Unique, jni::Make<std::string>(_env, programCacheDir)))
    , mailbox(std::make_shared<Mailbox>(*this))
    , rendererRef(*renderer, mailbox) {
}

MapRenderer::~MapRenderer() = default;

void MapRenderer::reset()  {
    assert (renderer);
    renderer.reset();
}

ActorRef<Renderer> MapRenderer::actor() const {
    return rendererRef;
}

void MapRenderer::schedule(std::weak_ptr<Mailbox> scheduled) {
    // Create a runnable and schedule it on the gl thread
    android::UniqueEnv _env = android::AttachEnv();
    auto runnable = std::make_unique<MapRendererRunnable>(*_env, std::move(scheduled));

    static auto queueEvent = javaClass.GetMethod<void (jni::Object<MapRendererRunnable>)>(*_env, "queueEvent");
    javaPeer->Call(*_env, queueEvent, runnable->getPeer());

    // Release the object as it will be destroyed on GC of the Java Peer
    runnable.release();
}

void MapRenderer::requestRender() {
    android::UniqueEnv _env = android::AttachEnv();
    static auto onInvalidate = javaClass.GetMethod<void ()>(*_env, "requestRender");
    javaPeer->Call(*_env, onInvalidate);
}

void MapRenderer::update(std::shared_ptr<UpdateParameters> params) {
    // Lock on the parameters
    std::lock_guard<std::mutex> lock(updateMutex);
    updateParameters = std::move(params);
}

// Called on OpenGL thread //

void MapRenderer::render(JNIEnv&) {
    assert (renderer);

    std::shared_ptr<UpdateParameters> params;
    {
        // Lock on the parameters
        std::unique_lock<std::mutex> lock(updateMutex);
        if (!updateParameters) return;

        // Hold on to the update parameters during render
        params = updateParameters;
    }

    // Activate the backend
    BackendScope backendGuard { *backend };

    // Ensure that the "current" scheduler on the render thread is
    // this scheduler.
    Scheduler::SetCurrent(this);

    if (framebufferSizeChanged) {
        backend->updateViewPort();
        framebufferSizeChanged = false;
    }

    renderer->render(*params);
}

void MapRenderer::onSurfaceCreated(JNIEnv&) {
    // TODO (re-)create Context, Backend and Renderer
}

void MapRenderer::onSurfaceChanged(JNIEnv&, jint width, jint height) {
    backend->resizeFramebuffer(width, height);
    framebufferSizeChanged = true;
    requestRender();
}

// Static methods //

jni::Class<MapRenderer> MapRenderer::javaClass;

void MapRenderer::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    MapRenderer::javaClass = *jni::Class<MapRenderer>::Find(env).NewGlobalRef(env).release();

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<MapRenderer>(
            env,
            MapRenderer::javaClass,
            "nativePtr",
            std::make_unique<MapRenderer, JNIEnv&, jni::Object<MapRenderer>, jni::Object<FileSource>, jni::jfloat, jni::String>,
            "nativeInitialize",
            "finalize",
            METHOD(&MapRenderer::render, "nativeRender"),
            METHOD(&MapRenderer::onSurfaceCreated, "nativeOnSurfaceCreated"),
            METHOD(&MapRenderer::onSurfaceChanged, "nativeOnSurfaceChanged")
    );
}

MapRenderer& MapRenderer::getNativePeer(JNIEnv& env, jni::Object<MapRenderer> jObject) {
    static auto field = MapRenderer::javaClass.GetField<jlong>(env, "nativePtr");
    MapRenderer* mapRenderer = reinterpret_cast<MapRenderer *>(jObject.Get(env, field));
    assert(mapRenderer != nullptr);
    return *mapRenderer;
}

} // namespace android
} // namespace mbgl
