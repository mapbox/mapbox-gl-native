#include "map_renderer.hpp"

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/util/shared_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>

#include <string>

#include "attach_env.hpp"
#include "android_renderer_backend.hpp"
#include "map_renderer_runnable.hpp"
#include "file_source.hpp"

namespace mbgl {
namespace android {

MapRenderer::MapRenderer(jni::JNIEnv& _env, jni::Object<MapRenderer> obj,
                         jni::Object<FileSource> _fileSource, jni::jfloat pixelRatio_,
                         jni::String programCacheDir_,
                         jni::String localIdeographFontFamily_)
        : javaPeer(SeizeGenericWeakRef(_env, jni::Object<MapRenderer>(jni::NewWeakGlobalRef(_env, obj.Get()).release()))), pixelRatio(pixelRatio_)
        , fileSource(FileSource::getDefaultFileSource(_env, _fileSource))
        , programCacheDir(jni::Make<std::string>(_env, programCacheDir_))
        , localIdeographFontFamily(localIdeographFontFamily_ == nullptr ? optional<std::string>{} : jni::Make<std::string>(_env, localIdeographFontFamily_ ))
        , threadPool(sharedThreadPool())
        , mailbox(std::make_shared<Mailbox>(*this)) {
}

MapRenderer::~MapRenderer() = default;

void MapRenderer::reset() {
    // Make sure to destroy the renderer on the GL Thread
    auto self = ActorRef<MapRenderer>(*this, mailbox);
    self.ask(&MapRenderer::resetRenderer).wait();

    // Lock to make sure there is no concurrent initialisation on the gl thread
    std::lock_guard<std::mutex> lock(initialisationMutex);
    rendererObserver.reset();
}

ActorRef<Renderer> MapRenderer::actor() const {
    return *rendererRef;
}

void MapRenderer::schedule(std::weak_ptr<Mailbox> scheduled) {
    // Create a runnable
    android::UniqueEnv _env = android::AttachEnv();
    auto runnable = std::make_unique<MapRendererRunnable>(*_env, std::move(scheduled));

    // Obtain ownership of the peer (gets transferred to the MapRenderer on the JVM for later GC)
    auto peer = runnable->peer();

    // Queue the event on the Java Peer
    static auto queueEvent = javaClass.GetMethod<void(
            jni::Object<MapRendererRunnable>)>(*_env, "queueEvent");
    javaPeer->Call(*_env, queueEvent, *peer);

    // Release the c++ peer as it will be destroyed on GC of the Java Peer
    runnable.release();
}

void MapRenderer::requestRender() {
    android::UniqueEnv _env = android::AttachEnv();
    static auto onInvalidate = javaClass.GetMethod<void()>(*_env, "requestRender");
    javaPeer->Call(*_env, onInvalidate);
}

void MapRenderer::update(std::shared_ptr<UpdateParameters> params) {
    // Lock on the parameters
    std::lock_guard<std::mutex> lock(updateMutex);
    updateParameters = std::move(params);
}

void MapRenderer::setObserver(std::shared_ptr<RendererObserver> _rendererObserver) {
    // Lock as the initialization can come from the main thread or the GL thread first
    std::lock_guard<std::mutex> lock(initialisationMutex);

    rendererObserver = std::move(_rendererObserver);

    // Set the new observer on the Renderer implementation
    if (renderer) {
        renderer->setObserver(rendererObserver.get());
    }
}

void MapRenderer::requestSnapshot(SnapshotCallback callback) {
    auto self = ActorRef<MapRenderer>(*this, mailbox);
    self.invoke(
            &MapRenderer::scheduleSnapshot,
            std::make_unique<SnapshotCallback>([&, callback=std::move(callback), runloop=util::RunLoop::Get()](PremultipliedImage image) {
                runloop->invoke([callback=std::move(callback), image=std::move(image)]() mutable {
                    callback(std::move(image));
                });
                snapshotCallback.reset();
            })
    );
}

// Called on OpenGL thread //

void MapRenderer::resetRenderer() {
    assert (renderer);
    renderer.reset();
}

void MapRenderer::scheduleSnapshot(std::unique_ptr<SnapshotCallback> callback) {
    snapshotCallback = std::move(callback);
    requestRender();
}

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

    // Deliver the snapshot if requested
    if (snapshotCallback) {
        snapshotCallback->operator()(backend->readFramebuffer());
        snapshotCallback.reset();
    }
}

void MapRenderer::onSurfaceCreated(JNIEnv&) {
    // Lock as the initialization can come from the main thread or the GL thread first
    std::lock_guard<std::mutex> lock(initialisationMutex);

    // The android system will have already destroyed the underlying
    // GL resources if this is not the first initialization and an
    // attempt to clean them up will fail
    if (backend) backend->markContextLost();
    if (renderer) renderer->markContextLost();

    // Reset in opposite order
    renderer.reset();
    backend.reset();

    // Create the new backend and renderer
    backend = std::make_unique<AndroidRendererBackend>();
    renderer = std::make_unique<Renderer>(*backend, pixelRatio, fileSource, *threadPool,
                                          GLContextMode::Unique, programCacheDir, localIdeographFontFamily);
    rendererRef = std::make_unique<ActorRef<Renderer>>(*renderer, mailbox);

    // Set the observer on the new Renderer implementation
    if (rendererObserver) {
        renderer->setObserver(rendererObserver.get());
    }
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
    jni::RegisterNativePeer<MapRenderer>(env, MapRenderer::javaClass, "nativePtr",
                                         std::make_unique<MapRenderer, JNIEnv&, jni::Object<MapRenderer>, jni::Object<FileSource>, jni::jfloat, jni::String, jni::String>,
                                         "nativeInitialize", "finalize",
                                         METHOD(&MapRenderer::render, "nativeRender"),
                                         METHOD(&MapRenderer::onSurfaceCreated,
                                                "nativeOnSurfaceCreated"),
                                         METHOD(&MapRenderer::onSurfaceChanged,
                                                "nativeOnSurfaceChanged"));
}

MapRenderer& MapRenderer::getNativePeer(JNIEnv& env, jni::Object<MapRenderer> jObject) {
    static auto field = MapRenderer::javaClass.GetField<jlong>(env, "nativePtr");
    MapRenderer* mapRenderer = reinterpret_cast<MapRenderer*>(jObject.Get(env, field));
    assert(mapRenderer != nullptr);
    return *mapRenderer;
}

} // namespace android
} // namespace mbgl
