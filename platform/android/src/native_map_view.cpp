#include "native_map_view.hpp"
#include "jni.hpp"
#include <jni/jni.hpp>

#include <cstdlib>
#include <ctime>
#include <cassert>
#include <memory>
#include <list>
#include <tuple>

#include <sys/system_properties.h>

#include <mbgl/util/platform.hpp>
#include <mbgl/util/event.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/image.hpp>

#include "bitmap.hpp"
#include "run_loop_impl.hpp"

namespace mbgl {
namespace android {

NativeMapView::NativeMapView(JNIEnv *env_, jobject obj_, float _pixelRatio, int availableProcessors_, size_t totalMemory_)
    : env(env_),
      pixelRatio(_pixelRatio),
      availableProcessors(availableProcessors_),
      totalMemory(totalMemory_),
      runLoop(std::make_unique<mbgl::util::RunLoop>(mbgl::util::RunLoop::Type::New)),
      threadPool(4) {
    mbgl::Log::Info(mbgl::Event::Android, "NativeMapView::NativeMapView");

    //Add a wake function to wake the render thread when needed
    mbgl::util::RunLoop::Impl* loop = reinterpret_cast<mbgl::util::RunLoop::Impl*>(mbgl::util::RunLoop::getLoopHandle());
    loop->setWakeFunction(std::bind(&NativeMapView::wake, this));

    assert(env_ != nullptr);
    assert(obj_ != nullptr);

    if (env->GetJavaVM(&vm) < 0) {
        env->ExceptionDescribe();
        return;
    }

    obj = env->NewWeakGlobalRef(obj_);
    if (obj == nullptr) {
        env->ExceptionDescribe();
        return;
    }

    fileSource = std::make_unique<mbgl::DefaultFileSource>(
        mbgl::android::cachePath + "/mbgl-offline.db",
        mbgl::android::apkPath);

    map = std::make_unique<mbgl::Map>(
        *this, mbgl::Size{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
        pixelRatio, *fileSource, threadPool, MapMode::Continuous);

    float zoomFactor   = map->getMaxZoom() - map->getMinZoom() + 1;
    float cpuFactor    = availableProcessors;
    float memoryFactor = static_cast<float>(totalMemory) / 1000.0f / 1000.0f / 1000.0f;
    float sizeFactor   = (static_cast<float>(map->getSize().width)  / mbgl::util::tileSize) *
                         (static_cast<float>(map->getSize().height) / mbgl::util::tileSize);

    size_t cacheSize = zoomFactor * cpuFactor * memoryFactor * sizeFactor * 0.5f;

    map->setSourceTileCacheSize(cacheSize);
}

NativeMapView::~NativeMapView() {
    mbgl::Log::Info(mbgl::Event::Android, "NativeMapView::~NativeMapView");

    assert(vm != nullptr);
    assert(obj != nullptr);

    map.reset();
    fileSource.reset();

    env->DeleteWeakGlobalRef(obj);

    obj = nullptr;
    env = nullptr;
    vm = nullptr;
}

mbgl::Size NativeMapView::getFramebufferSize() const {
    mbgl::Log::Info(mbgl::Event::Android, "FB size %dx%d", fbWidth, fbHeight);
    return { static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight) };
}

void NativeMapView::wake() {
    mbgl::Log::Info(mbgl::Event::JNI, "Wake callback");

    JNIEnv *env2;
    jboolean detach = attach_jni_thread(theJVM, &env2, "GL Callback Thread");
    if (!detach) {
        env2->CallVoidMethod(obj, wakeCallbackId);
        if (env2->ExceptionCheck()) {
            env2->ExceptionDescribe();
        }
    }
    detach_jni_thread(theJVM, &env2, detach);
}

void NativeMapView::updateViewBinding() {
    getContext().bindFramebuffer.setCurrentValue(0);
    assert(mbgl::gl::value::BindFramebuffer::Get() == getContext().bindFramebuffer.getCurrentValue());
    getContext().viewport.setCurrentValue({ 0, 0, getFramebufferSize() });
    assert(mbgl::gl::value::Viewport::Get() == getContext().viewport.getCurrentValue());
}

void NativeMapView::bind() {
    getContext().bindFramebuffer = 0;
    getContext().viewport = { 0, 0, getFramebufferSize() };
}

void NativeMapView::invalidate() {
    Log::Info(mbgl::Event::Android, "NativeMapView::invalidate");
    assert(vm != nullptr);
    assert(obj != nullptr);
    JNIEnv *env2;
    jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Callback Thread");
    if (!renderDetach) {
        env2->CallVoidMethod(obj, onInvalidateId);
        if(env2->ExceptionCheck()) {
            env2->ExceptionDescribe();
        }
    }
}

void NativeMapView::render() {
    mbgl::Log::Info(mbgl::Event::Android, "NativeMapView::render");

    if (framebufferSizeChanged) {
        getContext().viewport = { 0, 0, getFramebufferSize() };
        framebufferSizeChanged = false;
    }

    updateViewBinding();
    map->render(*this);

    if (snapshot){
        snapshot = false;

        // take snapshot
        auto image = getContext().readFramebuffer<mbgl::PremultipliedImage>(getFramebufferSize());
        auto bitmap = Bitmap::CreateBitmap(*env, std::move(image));

        JNIEnv *env2;
        jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Callback Thread");
        if (!renderDetach) {
            env2->CallVoidMethod(obj, onSnapshotReadyId, jni::Unwrap(*bitmap));
            if (env2->ExceptionCheck()) {
                env2->ExceptionDescribe();
            }
        }
    }

    updateFps();
}

mbgl::Map &NativeMapView::getMap() { return *map; }

mbgl::DefaultFileSource &NativeMapView::getFileSource() { return *fileSource; }

void NativeMapView::scheduleTakeSnapshot() {
    snapshot = true;
}


void NativeMapView::notifyMapChange(mbgl::MapChange change) {
    mbgl::Log::Info(mbgl::Event::Android, "notifyMapChange");
    assert(vm != nullptr);
    assert(obj != nullptr);

    JNIEnv *env2;
    jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Callback Thread");
    if (!renderDetach) {
        env2->CallVoidMethod(obj, onMapChangedId, change);
        if(env2->ExceptionCheck()) {
            env2->ExceptionDescribe();
        }
    }
}

void NativeMapView::enableFps(bool enable) {
    fpsEnabled = enable;
}

void NativeMapView::updateFps() {
    if (!fpsEnabled) {
        return;
    }

    static int frames = 0;
    static int64_t timeElapsed = 0LL;

    frames++;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    int64_t currentTime = now.tv_sec * 1000000000LL + now.tv_nsec;

    if (currentTime - timeElapsed >= 1) {
        fps = frames / ((currentTime - timeElapsed) / 1E9);
        mbgl::Log::Info(mbgl::Event::Render, "FPS: %4.2f", fps);
        timeElapsed = currentTime;
        frames = 0;
    }

    assert(vm != nullptr);
    assert(obj != nullptr);

    JNIEnv *env2;
    jboolean renderDetach = attach_jni_thread(theJVM, &env2, "Callback Thread");
    if (!renderDetach) {
        env2->CallVoidMethod(obj, onFpsChangedId, fps);
        if(env2->ExceptionCheck()) {
            env2->ExceptionDescribe();
        }
    }
}

void NativeMapView::onViewportChanged(int w, int h) {
    resizeView((int) w / pixelRatio, (int) h / pixelRatio);
    resizeFramebuffer(w, h);
}

void NativeMapView::resizeView(int w, int h) {
    mbgl::Log::Info(mbgl::Event::Android, "resizeView %ix%i", w, h);
    width = w;
    height = h;
    map->setSize({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
}

void NativeMapView::resizeFramebuffer(int w, int h) {
    mbgl::Log::Info(mbgl::Event::Android, "resizeFramebuffer %ix%i", w, h);
    fbWidth = w;
    fbHeight = h;
    framebufferSizeChanged = true;
    invalidate();
}

void NativeMapView::setInsets(mbgl::EdgeInsets insets_) {
    insets = insets_;
}

}
}
