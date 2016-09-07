#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>

#include <QApplication>
#include <QGLContext>
#include <QGLWidget>

#if QT_VERSION >= 0x050000
#include <QOpenGLContext>
#endif

namespace mbgl {

gl::glProc HeadlessView::initializeExtension(const char* name) {
#if QT_VERSION >= 0x050000
        QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        return thisContext->getProcAddress(name);
#else
        const QGLContext* thisContext = QGLContext::currentContext();
        return reinterpret_cast<mbgl::gl::glProc>(thisContext->getProcAddress(name));
#endif
}

void HeadlessView::createContext() {
    static const char* argv[] = { "mbgl" };
    static int argc = 1;
    static auto* app = new QApplication(argc, const_cast<char**>(argv));

    Q_UNUSED(app);

    glContext = new QGLWidget;
}

void HeadlessView::destroyContext() {
    delete glContext;
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
    glContext->makeCurrent();
}

void HeadlessView::deactivateContext() {
    glContext->doneCurrent();
}

} // namespace mbgl
