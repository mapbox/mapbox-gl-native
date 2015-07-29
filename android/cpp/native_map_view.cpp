#include <cstdlib>
#include <ctime>
#include <cassert>
#include <memory>
#include <list>
#include <tuple>

#include <sys/system_properties.h>

#include <GLES2/gl2.h>

#include <mbgl/android/native_map_view.hpp>
#include <mbgl/android/jni.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/event.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {
namespace android {

// TODO log GL info in Java

void log_gl_string(GLenum name, const char *label) {
    const GLubyte *str = glGetString(name);
    if (str == nullptr) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "glGetString(%d) returned error %d", name,
                         glGetError());
        throw new std::runtime_error("glGetString() failed");
    } else {
        char buf[513];
        for (int len = std::strlen(reinterpret_cast<const char *>(str)), pos = 0; len > 0;
             len -= 512, pos += 512) {
            strncpy(buf, reinterpret_cast<const char *>(str) + pos, 512);
            buf[512] = 0;
            mbgl::Log::Info(mbgl::Event::OpenGL, "GL %s: %s", label, buf);
        }
    }
}

NativeMapView::NativeMapView(JNIEnv *env, jobject obj_, float pixelRatio_, int availableProcessors_, size_t totalMemory_)
    : mbgl::View(*this),
      pixelRatio(pixelRatio_),
      availableProcessors(availableProcessors_),
      totalMemory(totalMemory_),
      fileCache(mbgl::android::cachePath + "/mbgl-cache.db"),
      fileSource(&fileCache),
      map(*this, fileSource, MapMode::Continuous) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::NativeMapView");

    assert(env != nullptr);
    assert(obj_ != nullptr);

    if (env->GetJavaVM(&vm) < 0) {
        env->ExceptionDescribe();
        return;
    }

    obj = env->NewGlobalRef(obj_);
    if (obj == nullptr) {
        env->ExceptionDescribe();
        return;
    }
}

NativeMapView::~NativeMapView() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::~NativeMapView");

    assert(vm != nullptr);
    assert(obj != nullptr);

    jint ret;
    JNIEnv *env = nullptr;
    ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (ret == JNI_OK) {
        env->DeleteGlobalRef(obj);
    } else {
        mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
        throw new std::runtime_error("GetEnv() failed");
    }
    obj = nullptr;
    vm = nullptr;
}

float NativeMapView::getPixelRatio() const {
    return pixelRatio;
}

std::array<uint16_t, 2> NativeMapView::getSize() const {
    return {{ static_cast<uint16_t>(width), static_cast<uint16_t>(height) }};
}

std::array<uint16_t, 2> NativeMapView::getFramebufferSize() const {
    return {{ static_cast<uint16_t>(fbWidth), static_cast<uint16_t>(fbHeight) }};
}

void NativeMapView::activate() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::activate");

    // TODO
}

void NativeMapView::deactivate() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::deactivate");

    // TODO
}

void NativeMapView::invalidate() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::invalidate()");

    clean.clear();

    assert(vm != nullptr);
    assert(obj != nullptr);

    JavaVMAttachArgs args = {JNI_VERSION_1_2, "NativeMapView::invalidate()", NULL};

    jint ret;
    JNIEnv *env = nullptr;
    bool detach = false;
    ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        if (ret != JNI_EDETACHED) {
            mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
            throw new std::runtime_error("GetEnv() failed");
        } else {
            ret = vm->AttachCurrentThread(&env, &args);
            if (ret != JNI_OK) {
                mbgl::Log::Error(mbgl::Event::JNI, "AttachCurrentThread() failed with %i", ret);
                throw new std::runtime_error("AttachCurrentThread() failed");
            }
            detach = true;
        }
    }

    env->CallVoidMethod(obj, onInvalidateId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    if (detach) {
        if ((ret = vm->DetachCurrentThread()) != JNI_OK) {
            mbgl::Log::Error(mbgl::Event::JNI, "DetachCurrentThread() failed with %i", ret);
            throw new std::runtime_error("DetachCurrentThread() failed");
        }
    }
    env = nullptr;
}

void NativeMapView::swap() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::swap()");

    // TODO
}

void NativeMapView::notify() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::notify()");
    // noop
}

mbgl::Map &NativeMapView::getMap() { return map; }

mbgl::DefaultFileSource &NativeMapView::getFileSource() { return fileSource; }

bool NativeMapView::inEmulator() {
    // Detect if we are in emulator
    char prop[PROP_VALUE_MAX];
    __system_property_get("ro.kernel.qemu", prop);
    return strtol(prop, nullptr, 0) == 1;
}

void NativeMapView::surfaceCreated() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::surfaceCreated");

    if (!firstTime) {
        firstTime = true;

        // TODO move to Java?
        log_gl_string(GL_VENDOR, "Vendor");
        log_gl_string(GL_RENDERER, "Renderer");
        log_gl_string(GL_VERSION, "Version");
        if (!inEmulator()) {
            log_gl_string(GL_SHADING_LANGUAGE_VERSION,
                        "SL Version"); // In the emulator this returns NULL with error code 0?
                                        // https://code.google.com/p/android/issues/detail?id=78977
        }

        log_gl_string(GL_EXTENSIONS, "Extensions");
        // TODO may requie call to Java?
        // Tricky?
        /*mbgl::gl::InitializeExtensions([] (const char * name) {
             return reinterpret_cast<mbgl::gl::glProc>(eglGetProcAddress(name));
        });*/
    }
}

void NativeMapView::notifyMapChange(mbgl::MapChange) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::notifyMapChange()");

    assert(vm != nullptr);
    assert(obj != nullptr);

    JavaVMAttachArgs args = {JNI_VERSION_1_2, "NativeMapView::notifyMapChange()", NULL};

    jint ret;
    JNIEnv *env = nullptr;
    bool detach = false;
    ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        if (ret != JNI_EDETACHED) {
            mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
            throw new std::runtime_error("GetEnv() failed");
        } else {
            ret = vm->AttachCurrentThread(&env, &args);
            if (ret != JNI_OK) {
                mbgl::Log::Error(mbgl::Event::JNI, "AttachCurrentThread() failed with %i", ret);
                throw new std::runtime_error("AttachCurrentThread() failed");
            }
            detach = true;
        }
    }

    env->CallVoidMethod(obj, onMapChangedId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    if (detach) {
        if ((ret = vm->DetachCurrentThread()) != JNI_OK) {
            mbgl::Log::Error(mbgl::Event::JNI, "DetachCurrentThread() failed with %i", ret);
            throw new std::runtime_error("DetachCurrentThread() failed");
        }
    }
    env = nullptr;
}

void NativeMapView::enableFps(bool enable) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::enableFps()");

    fpsEnabled = enable;
}

void NativeMapView::updateFps() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::updateFps()");

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
        mbgl::Log::Debug(mbgl::Event::Render, "FPS: %4.2f", fps);
        timeElapsed = currentTime;
        frames = 0;
    }

    assert(vm != nullptr);
    assert(obj != nullptr);

    JavaVMAttachArgs args = {JNI_VERSION_1_2, "NativeMapView::updateFps()", NULL};

    jint ret;
    JNIEnv *env = nullptr;
    bool detach = false;
    ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        if (ret != JNI_EDETACHED) {
            mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
            throw new std::runtime_error("GetEnv() failed");
        } else {
            ret = vm->AttachCurrentThread(&env, &args);
            if (ret != JNI_OK) {
                mbgl::Log::Error(mbgl::Event::JNI, "AttachCurrentThread() failed with %i", ret);
                throw new std::runtime_error("AttachCurrentThread() failed");
            }
            detach = true;
        }
    }

    env->CallVoidMethod(obj, onFpsChangedId, fps);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    if (detach) {
        if ((ret = vm->DetachCurrentThread()) != JNI_OK) {
            mbgl::Log::Error(mbgl::Event::JNI, "DetachCurrentThread() failed with %i", ret);
            throw new std::runtime_error("DetachCurrentThread() failed");
        }
    }
    env = nullptr;
}

void NativeMapView::drawFrame(bool inProgress) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::drawFrame()");

    const bool dirty = !clean.test_and_set();
    if (dirty) {
        float zoomFactor   = map.getMaxZoom() - map.getMinZoom() + 1;
        float cpuFactor    = availableProcessors;
        float memoryFactor = static_cast<float>(totalMemory) / 1000.0f / 1000.0f / 1000.0f;
        float sizeFactor   = (static_cast<float>(map.getWidth())  / mbgl::util::tileSize) *
                             (static_cast<float>(map.getHeight()) / mbgl::util::tileSize);

        size_t cacheSize = zoomFactor * cpuFactor * memoryFactor * sizeFactor * 0.5f;

        map.setSourceTileCacheSize(cacheSize);

        map.renderSync();
        map.nudgeTransitions();
    }
}

void NativeMapView::surfaceChanged(int w, int h) {
    width = w / pixelRatio;
    height = h / pixelRatio;
    fbWidth = w;
    fbHeight = h;
    map.update(mbgl::Update::Repaint);
}

}
}
