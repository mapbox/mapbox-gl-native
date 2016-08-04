#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/log.hpp>

#include <cassert>

#include <GL/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <gbm.h>

namespace mbgl {

gl::glProc HeadlessView::initializeExtension(const char* name) {
    return eglGetProcAddress(name);
}

void HeadlessView::createContext() {
    dpy = display->dpy;
    config = display->config;

    assert(dpy != EGL_NO_DISPLAY);
    assert(glContext == EGL_NO_CONTEXT);
    assert(config != nullptr);

    eglBindAPI(EGL_OPENGL_API);

    glContext = eglCreateContext(dpy, config, EGL_NO_CONTEXT, NULL);
    if (glContext == EGL_NO_CONTEXT) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglCreateContext() returned error 0x%04x",
                         eglGetError());
        throw std::runtime_error("Error creating GL context object");
    }

    // TODO(vignatti): think about an optimal size here.
    gs = gbm_surface_create(display->gbm, 8, 8,
                            GBM_BO_FORMAT_XRGB8888,
                            GBM_BO_USE_RENDERING);
    if (gs == NULL) {
        throw std::runtime_error("Unable to create gbm surface.");
    }

    surface = eglCreateWindowSurface(dpy, config, reinterpret_cast<EGLNativeWindowType>(gs), NULL);
    if (surface == EGL_NO_SURFACE) {
        throw std::runtime_error("Failed to create surface.");
    }
}

void HeadlessView::destroyContext() {
    if (glContext) {
        if (!eglDestroyContext(dpy, glContext)) {
            throw std::runtime_error("Failed to destroy context.");
        }

        glContext = nullptr;
    }

    gbm_surface_destroy(gs);
    if (surface) {
        eglDestroySurface(dpy, surface);
        surface = 0;
    }
}

void HeadlessView::resizeFramebuffer() {
    const unsigned int w = dimensions[0] * pixelRatio;
    const unsigned int h = dimensions[1] * pixelRatio;

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
        std::string error("Couldn't create framebuffer: ");
        switch (status) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: (error += "incomplete attachment"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: error += "incomplete missing attachment"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: error += "incomplete dimensions"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: error += "incomplete formats"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: error += "incomplete draw buffer"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: error += "incomplete read buffer"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: error += "unsupported"; break;
            default: error += "other"; break;
        }
        throw std::runtime_error(error);
    }

    MBGL_CHECK_ERROR(glViewport(0, 0, w, h));
}

void HeadlessView::clearBuffers() {
    assert(active);

    MBGL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0));

    if (fbo) {
        MBGL_CHECK_ERROR(glDeleteFramebuffersEXT(1, &fbo));
        fbo = 0;
    }

    if (fboColor) {
        MBGL_CHECK_ERROR(glDeleteRenderbuffersEXT(1, &fboColor));
        fboColor = 0;
    }

    if (fboDepthStencil) {
        MBGL_CHECK_ERROR(glDeleteRenderbuffersEXT(1, &fboDepthStencil));
        fboDepthStencil = 0;
    }
}

void HeadlessView::activateContext() {
    if (!eglMakeCurrent(dpy, surface, surface, glContext)) {
        throw std::runtime_error("Switching OpenGL context failed.\n");
    }
}

void HeadlessView::deactivateContext() {
    if (!eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        throw std::runtime_error("Removing OpenGL context failed.\n");
    }
}

} // namespace mbgl
