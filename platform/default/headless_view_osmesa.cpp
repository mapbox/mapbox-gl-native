#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/log.hpp>

#include <cassert>

#include <GL/osmesa.h>

namespace mbgl {

gl::glProc HeadlessView::initializeExtension(const char* name) {
    return OSMesaGetProcAddress(name);
}

void HeadlessView::createContext() {
    if (!glContext) {
        /* Create an RGBA-mode context */
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
        /* specify Z, stencil, accum sizes */
        glContext = OSMesaCreateContextExt(OSMESA_RGBA, 16, 0, 0, NULL);
#else
        glContext = OSMesaCreateContext(OSMESA_RGBA, NULL);
#endif

        if (!glContext) {
            Log::Error(Event::OpenGL, "failed to create OSMesa context");
            OSMesaDestroyContext(glContext);
            glContext = 0;
        }
    }

    if (glContext == 0) {
        throw std::runtime_error("Error creating GL context object.");
    }

    // Create a dummy pbuffer. We will render to framebuffers anyway, but we need a pbuffer to
    // activate the context.
    pBuffer = malloc(4 * sizeof(GLubyte));
    if (!pBuffer) {
       throw std::runtime_error("Alloc pixel buffer failed!");
    }
}

void HeadlessView::destroyContext() {
    if (pBuffer) {
        free(pBuffer);
        pBuffer = nullptr;
    }

    OSMesaDestroyContext(glContext);
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
    assert(isActive());

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
    if (!OSMesaMakeCurrent(glContext, pBuffer, GL_UNSIGNED_BYTE, 1, 1)) {
        throw std::runtime_error("Switching OpenGL context failed.\n");
    }
}

void HeadlessView::deactivateContext() {
    // no-op
    // https://bugs.chromium.org/p/chromium/issues/detail?id=482787
}

} // namespace mbgl
