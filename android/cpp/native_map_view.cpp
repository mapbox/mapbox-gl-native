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
#include <mbgl/util/std.hpp>


pthread_once_t loadGLExtensions = PTHREAD_ONCE_INIT;

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

NativeMapView::NativeMapView(JNIEnv *env, jobject obj_)
    : mbgl::View(*this),
      fileCache(mbgl::android::cachePath + "/mbgl-cache.db"),
      fileSource(&fileCache),
      map(*this, fileSource) {
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
    terminateContext();
    destroySurface();
    terminateDisplay();

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

void NativeMapView::activate() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::activate");
    if ((display != EGL_NO_DISPLAY) && (surface != EGL_NO_SURFACE) && (context != EGL_NO_CONTEXT)) {
        if (!eglMakeCurrent(display, surface, surface, context)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent() returned error %d",
                             eglGetError());
            throw new std::runtime_error("eglMakeCurrent() failed");
        }
    } else {
        mbgl::Log::Info(mbgl::Event::Android, "Not activating as we are not ready");
    }
}

void NativeMapView::deactivate() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::deactivate");
    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                         eglGetError());
        throw new std::runtime_error("eglMakeCurrent() failed");
    }
}

void NativeMapView::invalidate(std::function<void()> render) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::invalidate");

    if ((display != EGL_NO_DISPLAY) && (surface != EGL_NO_SURFACE)) {
        render();

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

mbgl::Map &NativeMapView::getMap() { return map; }

mbgl::DefaultFileSource &NativeMapView::getFileSource() { return fileSource; }

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

    const std::unique_ptr<EGLConfig[]> configs = mbgl::util::make_unique<EGLConfig[]>(numConfigs);
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

void loadExtensions();

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

        pthread_once(&loadGLExtensions, loadExtensions);

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d", eglGetError());
            throw new std::runtime_error("eglMakeCurrent() failed");
        }
    }

    resume();
}

void NativeMapView::destroySurface() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::destroySurface");

    pause(true);

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

void NativeMapView::start() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::start");

    if (display == EGL_NO_DISPLAY) {
        initializeDisplay();
    }

    if (context == EGL_NO_CONTEXT) {
        initializeContext();
    }

    assert(display != EGL_NO_DISPLAY);
    assert(context != EGL_NO_CONTEXT);

    map.start(true);
}

void loadExtensions() {
    const GLubyte *str = glGetString(GL_EXTENSIONS);
    if (str == nullptr) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "glGetString(GL_EXTENSIONS) returned error %d",
                         glGetError());
        return;
    }

    std::string extensions(reinterpret_cast<const char *>(str));

    if (extensions.find("GL_OES_vertex_array_object") != std::string::npos) {
        mbgl::Log::Info(mbgl::Event::OpenGL, "Using GL_OES_vertex_array_object.");
        gl::BindVertexArray = reinterpret_cast<gl::PFNGLBINDVERTEXARRAYPROC>(
            eglGetProcAddress("glBindVertexArrayOES"));
        gl::DeleteVertexArrays = reinterpret_cast<gl::PFNGLDELETEVERTEXARRAYSPROC>(
            eglGetProcAddress("glDeleteVertexArraysOES"));
        gl::GenVertexArrays = reinterpret_cast<gl::PFNGLGENVERTEXARRAYSPROC>(
            eglGetProcAddress("glGenVertexArraysOES"));
        gl::IsVertexArray =
            reinterpret_cast<gl::PFNGLISVERTEXARRAYPROC>(eglGetProcAddress("glIsVertexArrayOES"));
        assert(gl::BindVertexArray != nullptr);
        assert(gl::DeleteVertexArrays != nullptr);
        assert(gl::GenVertexArrays != nullptr);
        assert(gl::IsVertexArray != nullptr);
    }

    if (extensions.find("GL_OES_packed_depth_stencil") != std::string::npos) {
        mbgl::Log::Info(mbgl::Event::OpenGL, "Using GL_OES_packed_depth_stencil.");
        gl::isPackedDepthStencilSupported = true;
    }

    if (extensions.find("GL_OES_depth24") != std::string::npos) {
        gl::isDepth24Supported = true;
    }

    if (extensions.find("GL_KHR_debug") != std::string::npos) {
        mbgl::Log::Info(mbgl::Event::OpenGL, "Using GL_KHR_debug.");
        gl::DebugMessageControl = reinterpret_cast<gl::PFNGLDEBUGMESSAGECONTROLPROC>(
            eglGetProcAddress("glDebugMessageControl"));
        gl::DebugMessageInsert = reinterpret_cast<gl::PFNGLDEBUGMESSAGEINSERTPROC>(
            eglGetProcAddress("glDebugMessageInsert"));
        gl::DebugMessageCallback = reinterpret_cast<gl::PFNGLDEBUGMESSAGECALLBACKPROC>(
            eglGetProcAddress("glDebugMessageCallback"));
        gl::GetDebugMessageLog = reinterpret_cast<gl::PFNGLGETDEBUGMESSAGELOGPROC>(
            eglGetProcAddress("glGetDebugMessageLog"));
        gl::GetPointerv =
            reinterpret_cast<gl::PFNGLGETPOINTERVPROC>(eglGetProcAddress("glGetPointerv"));
        gl::PushDebugGroup =
            reinterpret_cast<gl::PFNGLPUSHDEBUGGROUPPROC>(eglGetProcAddress("glPushDebugGroup"));
        gl::PopDebugGroup =
            reinterpret_cast<gl::PFNGLPOPDEBUGGROUPPROC>(eglGetProcAddress("glPopDebugGroup"));
        gl::ObjectLabel =
            reinterpret_cast<gl::PFNGLOBJECTLABELPROC>(eglGetProcAddress("glObjectLabel"));
        gl::GetObjectLabel =
            reinterpret_cast<gl::PFNGLGETOBJECTLABELPROC>(eglGetProcAddress("glGetObjectLabel"));
        gl::ObjectPtrLabel =
            reinterpret_cast<gl::PFNGLOBJECTPTRLABELPROC>(eglGetProcAddress("glObjectPtrLabel"));
        gl::GetObjectPtrLabel = reinterpret_cast<gl::PFNGLGETOBJECTPTRLABELPROC>(
            eglGetProcAddress("glGetObjectPtrLabel"));
        assert(gl::DebugMessageControl != nullptr);
        assert(gl::DebugMessageInsert != nullptr);
        assert(gl::DebugMessageCallback != nullptr);
        assert(gl::GetDebugMessageLog != nullptr);
        assert(gl::GetPointerv != nullptr);
        assert(gl::PushDebugGroup != nullptr);
        assert(gl::PopDebugGroup != nullptr);
        assert(gl::ObjectLabel != nullptr);
        assert(gl::GetObjectLabel != nullptr);
        assert(gl::ObjectPtrLabel != nullptr);
        assert(gl::GetObjectPtrLabel != nullptr);
    } else {
        if (extensions.find("GL_EXT_debug_marker") != std::string::npos) {
            mbgl::Log::Info(mbgl::Event::OpenGL, "Using GL_EXT_debug_marker.");
            gl::InsertEventMarkerEXT = reinterpret_cast<gl::PFNGLINSERTEVENTMARKEREXTPROC>(
                eglGetProcAddress("glInsertEventMarkerEXT"));
            gl::PushGroupMarkerEXT = reinterpret_cast<gl::PFNGLPUSHGROUPMARKEREXTPROC>(
                eglGetProcAddress("glPushGroupMarkerEXT"));
            gl::PopGroupMarkerEXT = reinterpret_cast<gl::PFNGLPOPGROUPMARKEREXTPROC>(
                eglGetProcAddress("glPopGroupMarkerEXT"));
            assert(gl::InsertEventMarkerEXT != nullptr);
            assert(gl::PushGroupMarkerEXT != nullptr);
            assert(gl::PopGroupMarkerEXT != nullptr);
        }

        if (extensions.find("GL_EXT_debug_label") != std::string::npos) {
            mbgl::Log::Info(mbgl::Event::OpenGL, "Using GL_EXT_debug_label.");
            gl::LabelObjectEXT = reinterpret_cast<gl::PFNGLLABELOBJECTEXTPROC>(
                eglGetProcAddress("glLabelObjectEXT"));
            gl::GetObjectLabelEXT = reinterpret_cast<gl::PFNGLGETOBJECTLABELEXTPROC>(
                eglGetProcAddress("glGetObjectLabelEXT"));
            assert(gl::LabelObjectEXT != nullptr);
            assert(gl::GetObjectLabelEXT != nullptr);
        }
    }
}

void NativeMapView::stop() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::stop");

    if ((display != EGL_NO_DISPLAY) && (context != EGL_NO_CONTEXT)) {
        map.stop();
    }
}

void NativeMapView::pause(bool waitForPause) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::pause %s",
                     (waitForPause) ? "true" : "false");

    if ((display != EGL_NO_DISPLAY) && (context != EGL_NO_CONTEXT)) {
        map.pause(waitForPause);
    }
}

void NativeMapView::resume() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::resume");

    assert(display != EGL_NO_DISPLAY);
    assert(context != EGL_NO_CONTEXT);

    if (surface != EGL_NO_SURFACE) {
        map.resume();
    } else {
        mbgl::Log::Debug(mbgl::Event::Android, "Not resuming because we are not ready");
    }
}

void NativeMapView::notifyMapChange(mbgl::MapChange, std::chrono::steady_clock::duration) {
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

void NativeMapView::resize(uint16_t width, uint16_t height, float ratio, uint16_t, uint16_t) {
    map.resize(width, height, ratio);
}

}
}
