#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>

#include <OpenGLES/EAGL.h>

namespace mbgl {

gl::glProc HeadlessView::initializeExtension(const char* name) {
    static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengles"));
    if (!framework) {
        throw std::runtime_error("Failed to load OpenGL framework.");
    }

    CFStringRef str = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
    void* symbol = CFBundleGetFunctionPointerForName(framework, str);
    CFRelease(str);

    return reinterpret_cast<gl::glProc>(symbol);
}

void HeadlessView::createContext() {
    glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (glContext == nil) {
        throw std::runtime_error("Error creating GL context object");
    }
    [reinterpret_cast<EAGLContext*>(glContext) retain];
    reinterpret_cast<EAGLContext*>(glContext).multiThreaded = YES;
}

void HeadlessView::destroyContext() {
    [reinterpret_cast<EAGLContext*>(glContext) release];
    glContext = nil;
}

void HeadlessView::resizeFramebuffer() {
    const unsigned int w = dimensions[0] * pixelRatio;
    const unsigned int h = dimensions[1] * pixelRatio;

    // Create depth/stencil buffer
    MBGL_CHECK_ERROR(glGenRenderbuffers(1, &fboDepthStencil));
    MBGL_CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, fboDepthStencil));
    MBGL_CHECK_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, w, h));
    MBGL_CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    MBGL_CHECK_ERROR(glGenRenderbuffers(1, &fboColor));
    MBGL_CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, fboColor));
    MBGL_CHECK_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, w, h));
    MBGL_CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    MBGL_CHECK_ERROR(glGenFramebuffers(1, &fbo));
    MBGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, fboColor));
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepthStencil));
    MBGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepthStencil));

    GLenum status = MBGL_CHECK_ERROR(glCheckFramebufferStatus(GL_FRAMEBUFFER));

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::string error("Couldn't create framebuffer: ");
        switch (status) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: (error += "incomplete attachment"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: error += "incomplete missing attachment"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: error += "incomplete dimensions"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: error += "unsupported"; break;
            default: error += "other"; break;
        }
        throw std::runtime_error(error);
    }

    MBGL_CHECK_ERROR(glViewport(0, 0, w, h));
}

void HeadlessView::clearBuffers() {
    assert(active);

    MBGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    if (fbo) {
        MBGL_CHECK_ERROR(glDeleteFramebuffers(1, &fbo));
        fbo = 0;
    }

    if (fboColor) {
        MBGL_CHECK_ERROR(glDeleteRenderbuffers(1, &fboColor));
        fboColor = 0;
    }

    if (fboDepthStencil) {
        MBGL_CHECK_ERROR(glDeleteRenderbuffers(1, &fboDepthStencil));
        fboDepthStencil = 0;
    }
}

void HeadlessView::activateContext() {
    [EAGLContext setCurrentContext:reinterpret_cast<EAGLContext*>(glContext)];
}

void HeadlessView::deactivateContext() {
    [EAGLContext setCurrentContext:nil];
}

} // namespace mbgl
