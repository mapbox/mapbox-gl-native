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

void log_egl_string(EGLDisplay display, EGLint name, const char *label) {
    const char *str = eglQueryString(display, name);
    if (str == nullptr) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglQueryString(%d) returned error %d", name,
                         eglGetError());
        throw new std::runtime_error("eglQueryString() failed");
    } else {
        char buf[513];
        for (int len = std::strlen(str), pos = 0; len > 0; len -= 512, pos += 512) {
            strncpy(buf, str + pos, 512);
            buf[512] = 0;
            mbgl::Log::Info(mbgl::Event::OpenGL, "EGL %s: %s", label, buf);
        }
    }
}

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
      totalMemory(totalMemory_) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::NativeMapView");

    assert(env != nullptr);
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

    fileCache = mbgl::SharedSQLiteCache::get(mbgl::android::cachePath + "/mbgl-cache.db");
    fileSource = std::make_unique<mbgl::DefaultFileSource>(fileCache.get());
    map = std::make_unique<mbgl::Map>(*this, *fileSource, MapMode::Continuous);

    float zoomFactor   = map->getMaxZoom() - map->getMinZoom() + 1;
    float cpuFactor    = availableProcessors;
    float memoryFactor = static_cast<float>(totalMemory) / 1000.0f / 1000.0f / 1000.0f;
    float sizeFactor   = (static_cast<float>(map->getWidth())  / mbgl::util::tileSize) *
                         (static_cast<float>(map->getHeight()) / mbgl::util::tileSize);

    size_t cacheSize = zoomFactor * cpuFactor * memoryFactor * sizeFactor * 0.5f;

    map->setSourceTileCacheSize(cacheSize);

    map->pause();
}

NativeMapView::~NativeMapView() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::~NativeMapView");
    terminateContext();
    destroySurface();
    terminateDisplay();

    assert(vm != nullptr);
    assert(obj != nullptr);

    map.reset();
    fileSource.reset();
    fileCache.reset();

    jint ret;
    JNIEnv *env = nullptr;
    ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (ret == JNI_OK) {
        env->DeleteWeakGlobalRef(obj);
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

    assert(vm != nullptr);

    renderDetach = attach_jni_thread(vm, &renderEnv, "Map Thread");

    if ((display != EGL_NO_DISPLAY) && (surface != EGL_NO_SURFACE) && (context != EGL_NO_CONTEXT)) {
        if (!eglMakeCurrent(display, surface, surface, context)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent() returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglMakeCurrent() failed");
        }

        if (!eglSwapInterval(display, 0)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglSwapInterval() returned error %d", eglGetError());
            throw new std::runtime_error("eglSwapInterval() failed");
        }
    } else {
        mbgl::Log::Info(mbgl::Event::Android, "Not activating as we are not ready");
    }
}

void NativeMapView::deactivate() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::deactivate");

    assert(vm != nullptr);

    if (display != EGL_NO_DISPLAY) {
        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglMakeCurrent() failed");
        }
    } else {
        mbgl::Log::Info(mbgl::Event::Android, "Not deactivating as we are not ready");
    }

    detach_jni_thread(vm, &renderEnv, renderDetach);
}

void NativeMapView::invalidate() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::invalidate()");

    assert(vm != nullptr);
    assert(obj != nullptr);

    JNIEnv *env = nullptr;
    bool detach = attach_jni_thread(vm, &env, "NativeMapView::invalidate()");

    env->CallVoidMethod(obj, onInvalidateId);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    detach_jni_thread(vm, &env, detach);
}

void NativeMapView::beforeRender() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::beforeRender()");
    // no-op
}

void NativeMapView::afterRender() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::afterRender()");

    assert(vm != nullptr);
    assert(obj != nullptr);

    if ((display != EGL_NO_DISPLAY) && (surface != EGL_NO_SURFACE)) {
        if (!eglSwapBuffers(display, surface)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglSwapBuffers() returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglSwapBuffers() failed");
        }

        updateFps();
    } else {
        mbgl::Log::Info(mbgl::Event::Android, "Not swapping as we are not ready");
    }
}

void NativeMapView::notify() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::notify()");
    // noop
}

mbgl::Map &NativeMapView::getMap() { return *map; }

mbgl::DefaultFileSource &NativeMapView::getFileSource() { return *fileSource; }

bool NativeMapView::inEmulator() {
    // Detect if we are in emulator
    char prop[PROP_VALUE_MAX];
    __system_property_get("ro.kernel.qemu", prop);
    return strtol(prop, nullptr, 0) == 1;
}

void NativeMapView::initializeDisplay() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::initializeDisplay");

    assert(display == EGL_NO_DISPLAY);
    assert(config == nullptr);
    assert(format < 0);

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetDisplay() returned error %d", eglGetError());
        throw new std::runtime_error("eglGetDisplay() failed");
    }

    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglInitialize() returned error %d", eglGetError());
        throw new std::runtime_error("eglInitialize() failed");
    }
    if ((major <= 1) && (minor < 3)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "EGL version is too low, need 1.3, got %d.%d", major,
                         minor);
        throw new std::runtime_error("EGL version is too low");
    }

    log_egl_string(display, EGL_VENDOR, "Vendor");
    log_egl_string(display, EGL_VERSION, "Version");
    log_egl_string(display, EGL_CLIENT_APIS, "Client APIs");
    log_egl_string(display, EGL_EXTENSIONS, "Client Extensions");

    // Detect if we are in emulator
    if (inEmulator()) {
        mbgl::Log::Warning(mbgl::Event::Android, "In emulator! Enabling hacks :-(");
    }

    // Get all configs at least RGB 565 with 16 depth and 8 stencil
    EGLint configAttribs[] = {
        EGL_CONFIG_CAVEAT,                               EGL_NONE,           EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES2_BIT,                              EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,
        EGL_BUFFER_SIZE,                                 16,                 EGL_RED_SIZE,
        5,                                               EGL_GREEN_SIZE,     6,
        EGL_BLUE_SIZE,                                   5,                  EGL_DEPTH_SIZE,
        16,                                              EGL_STENCIL_SIZE,   8,
        (inEmulator() ? EGL_NONE : EGL_CONFORMANT),        EGL_OPENGL_ES2_BIT, // Ugly hack
        (inEmulator() ? EGL_NONE : EGL_COLOR_BUFFER_TYPE), EGL_RGB_BUFFER,     // Ugly hack
        EGL_NONE};
    EGLint numConfigs;
    if (!eglChooseConfig(display, configAttribs, nullptr, 0, &numConfigs)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglChooseConfig(NULL) returned error %d",
                         eglGetError());
        throw new std::runtime_error("eglChooseConfig() failed");
    }
    if (numConfigs < 1) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglChooseConfig() returned no configs.");
        throw new std::runtime_error("eglChooseConfig() failed");
    }

    const auto configs = std::make_unique<EGLConfig[]>(numConfigs);
    if (!eglChooseConfig(display, configAttribs, configs.get(), numConfigs, &numConfigs)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglChooseConfig() returned error %d", eglGetError());
        throw new std::runtime_error("eglChooseConfig() failed");
    }

    config = chooseConfig(configs.get(), numConfigs);
    if (config == nullptr) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "No config chosen");
        throw new std::runtime_error("No config chosen");
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib() returned error %d",
                         eglGetError());
        throw new std::runtime_error("eglGetConfigAttrib() failed");
    }
    mbgl::Log::Info(mbgl::Event::OpenGL, "Chosen window format is %d", format);
}

void NativeMapView::terminateDisplay() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::terminateDisplay");

    if (display != EGL_NO_DISPLAY) {
        // Destroy the surface first, if it still exists. This call needs a valid surface.
        if (surface != EGL_NO_SURFACE) {
            if (!eglDestroySurface(display, surface)) {
                mbgl::Log::Error(mbgl::Event::OpenGL, "eglDestroySurface() returned error %d",
                                 eglGetError());
                throw new std::runtime_error("eglDestroySurface() failed");
            }
            surface = EGL_NO_SURFACE;
        }

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d", eglGetError());
            throw new std::runtime_error("eglMakeCurrent() failed");
        }

        if (!eglTerminate(display)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglTerminate() returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglTerminate() failed");
        }
    }

    display = EGL_NO_DISPLAY;
    config = nullptr;
    format = -1;
}

void NativeMapView::initializeContext() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::initializeContext");

    assert(display != EGL_NO_DISPLAY);
    assert(context == EGL_NO_CONTEXT);
    assert(config != nullptr);

    const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglCreateContext() returned error %d",
                         eglGetError());
        throw new std::runtime_error("eglCreateContext() failed");
    }
}

void NativeMapView::terminateContext() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::terminateContext");
    if (display != EGL_NO_DISPLAY) {

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d", eglGetError());
            throw new std::runtime_error("eglMakeCurrent() failed");
        }

        if (context != EGL_NO_CONTEXT) {
            if (!eglDestroyContext(display, context)) {
                mbgl::Log::Error(mbgl::Event::OpenGL, "eglDestroyContext() returned error %d",
                                 eglGetError());
                throw new std::runtime_error("eglDestroyContext() failed");
            }
        }
    }

    context = EGL_NO_CONTEXT;
}

void NativeMapView::createSurface(ANativeWindow *window_) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::createSurface");

    assert(window == nullptr);
    assert(window_ != nullptr);
    window = window_;

    assert(display != EGL_NO_DISPLAY);
    assert(surface == EGL_NO_SURFACE);
    assert(config != nullptr);
    assert(format >= 0);

    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    const EGLint surfaceAttribs[] = {EGL_NONE};
    surface = eglCreateWindowSurface(display, config, window, surfaceAttribs);
    if (surface == EGL_NO_SURFACE) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglCreateWindowSurface() returned error %d",
                         eglGetError());
        throw new std::runtime_error("eglCreateWindowSurface() failed");
    }

    if (!firstTime) {
        firstTime = true;

        EGLDisplay oldDisplay = eglGetCurrentDisplay();
        EGLSurface oldReadSurface = eglGetCurrentSurface(EGL_READ);
        EGLSurface oldDrawSurface = eglGetCurrentSurface(EGL_DRAW);
        EGLContext oldContext = eglGetCurrentContext();

        if (!eglMakeCurrent(display, surface, surface, context)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent() returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglMakeCurrent() failed");
        }

        log_gl_string(GL_VENDOR, "Vendor");
        log_gl_string(GL_RENDERER, "Renderer");
        log_gl_string(GL_VERSION, "Version");
        if (!inEmulator()) {
            log_gl_string(GL_SHADING_LANGUAGE_VERSION,
                        "SL Version"); // In the emulator this returns NULL with error code 0?
                                        // https://code.google.com/p/android/issues/detail?id=78977
        }

        log_gl_string(GL_EXTENSIONS, "Extensions");
        mbgl::gl::InitializeExtensions([] (const char * name) {
             return reinterpret_cast<mbgl::gl::glProc>(eglGetProcAddress(name));
        });

        if (!eglMakeCurrent(oldDisplay, oldDrawSurface, oldReadSurface, oldContext)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d", eglGetError());
            throw new std::runtime_error("eglMakeCurrent() failed");
        }
    }

    resume();
}

void NativeMapView::destroySurface() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::destroySurface");

    pause();

    if (surface != EGL_NO_SURFACE) {
        if (!eglDestroySurface(display, surface)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglDestroySurface() returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglDestroySurface() failed");
        }
    }

    surface = EGL_NO_SURFACE;

    if (window != nullptr) {
        ANativeWindow_release(window);
        window = nullptr;
    }
}

// Speed
/*
typedef enum {
    Format16Bit = 0,
    Format32BitNoAlpha = 1,
    Format32BitAlpha = 2,
    Format24Bit = 3,
    Unknown = 4
} BufferFormat;

typedef enum {
    Format16Depth8Stencil = 0,
    Format24Depth8Stencil = 1,
} DepthStencilFormat;
*/

// Quality
typedef enum {
    Format16Bit = 3,
    Format32BitNoAlpha = 1,
    Format32BitAlpha = 2,
    Format24Bit = 0,
    Unknown = 4
} BufferFormat;

typedef enum {
    Format16Depth8Stencil = 1,
    Format24Depth8Stencil = 0,
} DepthStencilFormat;

// Tuple is <buffer_format, depth_stencil_format, is_not_conformant, is_caveat, config_num,
// config_id>
typedef std::tuple<BufferFormat, DepthStencilFormat, bool, bool, int, EGLConfig> ConfigProperties;

EGLConfig NativeMapView::chooseConfig(const EGLConfig configs[], EGLint numConfigs) {
    mbgl::Log::Info(mbgl::Event::OpenGL, "Found %d configs", numConfigs);

    // Create a list of configs that pass our filters
    std::list<ConfigProperties> configList;
    for (int i = 0; i < numConfigs; i++) {
        mbgl::Log::Info(mbgl::Event::OpenGL, "Config %d:", i);

        EGLint caveat, conformant, bits, red, green, blue, alpha, alphaMask, depth, stencil,
            sampleBuffers, samples;

        if (!eglGetConfigAttrib(display, configs[i], EGL_CONFIG_CAVEAT, &caveat)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_CONFIG_CAVEAT) returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_CONFORMANT, &conformant)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_CONFORMANT) returned error %d", eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_BUFFER_SIZE, &bits)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_BUFFER_SIZE) returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &red)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_RED_SIZE) returned error %d", eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_GREEN_SIZE, &green)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_GREEN_SIZE) returned error %d", eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_BLUE_SIZE, &blue)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_BLUE_SIZE) returned error %d", eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &alpha)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_ALPHA_SIZE) returned error %d", eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_ALPHA_MASK_SIZE, &alphaMask)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_ALPHA_MASK_SIZE) returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &depth)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_DEPTH_SIZE) returned error %d", eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_STENCIL_SIZE, &stencil)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_STENCIL_SIZE) returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_SAMPLE_BUFFERS, &sampleBuffers)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_SAMPLE_BUFFERS) returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &samples)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglGetConfigAttrib(EGL_SAMPLES) returned error %d", eglGetError());
            throw new std::runtime_error("eglGetConfigAttrib() failed");
        }

        mbgl::Log::Info(mbgl::Event::OpenGL, "...Caveat: %d", caveat);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Conformant: %d", conformant);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Color: %d", bits);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Red: %d", red);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Green: %d", green);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Blue: %d", blue);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Alpha: %d", alpha);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Alpha mask: %d", alphaMask);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Depth: %d", depth);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Stencil: %d", stencil);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Sample buffers: %d", sampleBuffers);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Samples: %d", samples);

        bool configOk = true;
        configOk &= (depth == 24) || (depth == 16);
        configOk &= stencil == 8;
        configOk &= sampleBuffers == 0;
        configOk &= samples == 0;

        // Filter our configs first for depth, stencil and anti-aliasing
        if (configOk) {
            // Work out the config's buffer format
            BufferFormat bufferFormat;
            if ((bits == 16) && (red == 5) && (green == 6) && (blue == 5) && (alpha == 0)) {
                bufferFormat = Format16Bit;
            } else if ((bits == 32) && (red == 8) && (green == 8) && (blue == 8) && (alpha == 0)) {
                bufferFormat = Format32BitNoAlpha;
            } else if ((bits == 32) && (red == 8) && (green == 8) && (blue == 8) && (alpha == 8)) {
                bufferFormat = Format32BitAlpha;
            } else if ((bits == 24) && (red == 8) && (green == 8) && (blue == 8) && (alpha == 0)) {
                bufferFormat = Format24Bit;
            } else {
                bufferFormat = Unknown;
            }

            // Work out the config's depth stencil format
            DepthStencilFormat depthStencilFormat;
            if ((depth == 16) && (stencil == 8)) {
                depthStencilFormat = Format16Depth8Stencil;
            } else {
                depthStencilFormat = Format24Depth8Stencil;
            }

            bool isNotConformant = (conformant & EGL_OPENGL_ES2_BIT) != EGL_OPENGL_ES2_BIT;
            bool isCaveat = caveat != EGL_NONE;
            EGLConfig configId = configs[i];

            // Ignore formats we don't recognise
            if (bufferFormat != Unknown) {
                configList.push_back(std::make_tuple(bufferFormat, depthStencilFormat,
                                                     isNotConformant, isCaveat, i, configId));
            }
        }
    }

    if (configList.empty()) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "Config list was empty.");
    }

    // Sort the configs to find the best one
    configList.sort();
    bool isConformant = !std::get<2>(configList.front());
    bool isCaveat = std::get<3>(configList.front());
    int configNum = std::get<4>(configList.front());
    EGLConfig configId = std::get<5>(configList.front());

    mbgl::Log::Info(mbgl::Event::OpenGL, "Chosen config is %d", configNum);

    if (isCaveat) {
        mbgl::Log::Warning(mbgl::Event::OpenGL, "Chosen config has a caveat.");
    }
    if (!isConformant) {
        mbgl::Log::Warning(mbgl::Event::OpenGL, "Chosen config is not conformant.");
    }

    return configId;
}

void NativeMapView::pause() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::pause");

    if ((display != EGL_NO_DISPLAY) && (context != EGL_NO_CONTEXT)) {
        map->pause();
    }
}

void NativeMapView::resume() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::resume");

    assert(display != EGL_NO_DISPLAY);
    assert(context != EGL_NO_CONTEXT);

    if (surface != EGL_NO_SURFACE) {
        map->resume();
    } else {
        mbgl::Log::Debug(mbgl::Event::Android, "Not resuming because we are not ready");
    }
}

void NativeMapView::notifyMapChange(mbgl::MapChange change) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::notifyMapChange()");

    assert(vm != nullptr);
    assert(obj != nullptr);

    JNIEnv *env = nullptr;
    bool detach = attach_jni_thread(vm, &env, "NativeMapView::notifyMapChange()");

    env->CallVoidMethod(obj, onMapChangedId, change);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    detach_jni_thread(vm, &env, detach);
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

    JNIEnv *env = nullptr;
    bool detach = attach_jni_thread(vm, &env, "NativeMapView::updateFps()");

    env->CallVoidMethod(obj, onFpsChangedId, fps);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    detach_jni_thread(vm, &env, detach);
}

void NativeMapView::resizeView(int w, int h) {
    width = w;
    height = h;
    map->update(mbgl::Update::Dimensions);
}

void NativeMapView::resizeFramebuffer(int w, int h) {
    fbWidth = w;
    fbHeight = h;
    map->update(mbgl::Update::Repaint);
}

}
}
