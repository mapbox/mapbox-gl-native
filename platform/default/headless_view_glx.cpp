#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/log.hpp>

#include <cassert>

#include <GL/glx.h>

namespace mbgl {

gl::glProc HeadlessView::initializeExtension(const char* name) {
    return glXGetProcAddress(reinterpret_cast<const GLubyte*>(name));
}

void HeadlessView::createContext() {
    xDisplay = display->xDisplay;
    fbConfigs = display->fbConfigs;

    if (!glContext) {
        // Try to create a legacy context
        glContext = glXCreateNewContext(xDisplay, fbConfigs[0], GLX_RGBA_TYPE, None, True);
        if (glContext) {
            if (!glXIsDirect(xDisplay, glContext)) {
                Log::Error(Event::OpenGL, "failed to create direct OpenGL Legacy context");
                glXDestroyContext(xDisplay, glContext);
                glContext = nullptr;
            }
        }
    }

    if (glContext == nullptr) {
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
}

void HeadlessView::destroyContext() {
    if (glxPbuffer) {
        glXDestroyPbuffer(xDisplay, glxPbuffer);
        glxPbuffer = 0;
    }

    glXDestroyContext(xDisplay, glContext);
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
    if (!glXMakeContextCurrent(xDisplay, glxPbuffer, glxPbuffer, glContext)) {
        throw std::runtime_error("Switching OpenGL context failed.\n");
    }
}

void HeadlessView::deactivateContext() {
    if (!glXMakeContextCurrent(xDisplay, 0, 0, nullptr)) {
        throw std::runtime_error("Removing OpenGL context failed.\n");
    }
}

} // namespace mbgl
