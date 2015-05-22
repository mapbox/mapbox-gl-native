
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/log.hpp>

#include <mbgl/map/still_image.hpp>


#include <stdexcept>
#include <sstream>
#include <string>
#include <cstring>
#include <cassert>

#ifdef MBGL_USE_CGL
#include <CoreFoundation/CoreFoundation.h>
#elif MBGL_USE_GLX
#include <GL/glx.h>
#endif

namespace mbgl {

HeadlessView::HeadlessView(uint16_t width, uint16_t height, float pixelRatio)
    : display(std::make_shared<HeadlessDisplay>()) {
    activate();
    resize(width, height, pixelRatio);
    deactivate();
}

HeadlessView::HeadlessView(std::shared_ptr<HeadlessDisplay> display_,
                           uint16_t width,
                           uint16_t height,
                           float pixelRatio)
    : display(display_) {
    activate();
    resize(width, height, pixelRatio);
    deactivate();
}

void HeadlessView::loadExtensions() {
    if (extensionsLoaded) {
        return;
    }

#ifdef MBGL_USE_CGL
    gl::InitializeExtensions([](const char * name) {
        static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
        if (!framework) {
            throw std::runtime_error("Failed to load OpenGL framework.");
        }

        CFStringRef str = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
        void* symbol = CFBundleGetFunctionPointerForName(framework, str);
        CFRelease(str);

        return reinterpret_cast<gl::glProc>(symbol);
    });
#endif

#ifdef MBGL_USE_GLX
    gl::InitializeExtensions([](const char * name) {
        return glXGetProcAddress(reinterpret_cast<const GLubyte *>(name));
    });
#endif

    extensionsLoaded = true;
}

void HeadlessView::createContext() {
    if (!display) {
        throw std::runtime_error("Display is not set");
    }

#if MBGL_USE_CGL
    CGLError error = CGLCreateContext(display->pixelFormat, NULL, &glContext);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Error creating GL context object:") + CGLErrorString(error) + "\n");
    }

    error = CGLEnable(glContext, kCGLCEMPEngine);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Error enabling OpenGL multithreading:") + CGLErrorString(error) + "\n");
    }
#endif

#if MBGL_USE_GLX
    xDisplay = display->xDisplay;
    fbConfigs = display->fbConfigs;

    if (!glContext) {
        // Try to create a legacy context
        glContext = glXCreateNewContext(xDisplay, fbConfigs[0], GLX_RGBA_TYPE, None, True);
        if (glContext) {
            if (!glXIsDirect(xDisplay, glContext)) {
                Log::Error(Event::OpenGL, "failed to create direct OpenGL Legacy context");
                glXDestroyContext(xDisplay, glContext);
                glContext = 0;
            }
        }
    }

    if (glContext == 0) {
        throw std::runtime_error("Error creating GL context object.");
    }

    // Create a dummy pbuffer. We will render to framebuffers anyway, but we need a pbuffer to
    // activate the context.
    int pbufferAttributes[] = {
        GLX_PBUFFER_WIDTH, 8,
        GLX_PBUFFER_HEIGHT, 8,
        None
    };
    glxPbuffer = glXCreatePbuffer(xDisplay, fbConfigs[0], pbufferAttributes);
#endif
}

bool HeadlessView::isActive() {
    return std::this_thread::get_id() == thread;
}

HeadlessView::Dimensions::Dimensions(uint16_t width_, uint16_t height_, float pixelRatio_)
    : width(width_), height(height_), pixelRatio(pixelRatio_) {
}

void HeadlessView::resize(const uint16_t width, const uint16_t height, const float pixelRatio) {
    dimensions = { width, height, pixelRatio };

    clearBuffers();

    const unsigned int w = dimensions.width * dimensions.pixelRatio;
    const unsigned int h = dimensions.height * dimensions.pixelRatio;

    // Create depth/stencil buffer
    MBGL_CHECK_ERROR(glGenRenderbuffersEXT(1, &fboDepthStencil));
    MBGL_CHECK_ERROR(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fboDepthStencil));
    MBGL_CHECK_ERROR(glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, w, h));
    MBGL_CHECK_ERROR(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0));

    MBGL_CHECK_ERROR(glGenRenderbuffersEXT(1, &fboColor));
    MBGL_CHECK_ERROR(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fboColor));
    MBGL_CHECK_ERROR(glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, w, h));
    MBGL_CHECK_ERROR(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0));

    MBGL_CHECK_ERROR(glGenFramebuffersEXT(1, &fbo));
    MBGL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo));

    MBGL_CHECK_ERROR(glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, fboColor));
    MBGL_CHECK_ERROR(glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER_EXT, fboDepthStencil));

    GLenum status = MBGL_CHECK_ERROR(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT));

    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        std::stringstream error("Couldn't create framebuffer: ");
        switch (status) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: (error << "incomplete attachment.\n"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: error << "incomplete missing attachment.\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: error << "incomplete dimensions.\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: error << "incomplete formats.\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: error << "incomplete draw buffer.\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: error << "incomplete read buffer.\n"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: error << "unsupported.\n"; break;
            default: error << "other\n"; break;
        }
        throw std::runtime_error(error.str());
    }
}

std::unique_ptr<StillImage> HeadlessView::readStillImage() {
    assert(isActive());

    const unsigned int w = dimensions.pixelWidth();
    const unsigned int h = dimensions.pixelHeight();

    auto image = std::make_unique<StillImage>();
    image->width = w;
    image->height = h;
    image->pixels = std::make_unique<uint32_t[]>(w * h);

    MBGL_CHECK_ERROR(glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels.get()));

    const int stride = w * 4;
    auto tmp = std::make_unique<char[]>(stride);
    char *rgba = reinterpret_cast<char *>(image->pixels.get());
    for (int i = 0, j = h - 1; i < j; i++, j--) {
        std::memcpy(tmp.get(), rgba + i * stride, stride);
        std::memcpy(rgba + i * stride, rgba + j * stride, stride);
        std::memcpy(rgba + j * stride, tmp.get(), stride);
    }

    return image;
}

void HeadlessView::clearBuffers() {
    assert(isActive());

    MBGL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0));

    if (fbo) {
        MBGL_CHECK_ERROR(glDeleteFramebuffersEXT(1, &fbo));
        fbo = 0;
    }

    if (fboColor) {
        MBGL_CHECK_ERROR(glDeleteTextures(1, &fboColor));
        fboColor = 0;
    }

    if (fboDepthStencil) {
        MBGL_CHECK_ERROR(glDeleteRenderbuffersEXT(1, &fboDepthStencil));
        fboDepthStencil = 0;
    }
}

HeadlessView::~HeadlessView() {
    activate();
    clearBuffers();
    deactivate();

#if MBGL_USE_CGL
    CGLDestroyContext(glContext);
#endif

#if MBGL_USE_GLX
    if (glxPbuffer) {
        glXDestroyPbuffer(xDisplay, glxPbuffer);
        glxPbuffer = 0;
    }

    glXDestroyContext(xDisplay, glContext);
#endif
}

void HeadlessView::notify() {
    // no-op
}

void HeadlessView::activate() {
     if (thread != std::thread::id()) {
        throw std::runtime_error("OpenGL context was already current");
    }
    thread = std::this_thread::get_id();

    if (!glContext) {
        createContext();
    }

#if MBGL_USE_CGL
    CGLError error = CGLSetCurrentContext(glContext);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Switching OpenGL context failed:") + CGLErrorString(error) + "\n");
    }
#endif

#if MBGL_USE_GLX
    if (!glXMakeContextCurrent(xDisplay, glxPbuffer, glxPbuffer, glContext)) {
        throw std::runtime_error("Switching OpenGL context failed.\n");
    }
#endif

    loadExtensions();
}

void HeadlessView::deactivate() {
    if (thread == std::thread::id()) {
        throw std::runtime_error("OpenGL context was not current");
    }
    thread = std::thread::id();

#if MBGL_USE_CGL
    CGLError error = CGLSetCurrentContext(nullptr);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Removing OpenGL context failed:") + CGLErrorString(error) + "\n");
    }
#endif

#if MBGL_USE_GLX
    if (!glXMakeContextCurrent(xDisplay, 0, 0, nullptr)) {
        throw std::runtime_error("Removing OpenGL context failed.\n");
    }
#endif
}

void HeadlessView::invalidate() {
    // no-op
}

void HeadlessView::swap() {
    // no-op
}

}
