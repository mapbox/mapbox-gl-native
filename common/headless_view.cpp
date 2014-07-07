#include "headless_view.hpp"
#include <llmr/util/timer.hpp>
#include <llmr/platform/platform.hpp>

namespace llmr {

namespace platform {

void notify_map_change(MapChange change) {
    // no-op
}

}

HeadlessView::HeadlessView() {
    // TODO: test if OpenGL 4.1 with GL_ARB_ES2_compatibility is supported
    // If it is, use kCGLOGLPVersion_3_2_Core and enable that extension.
    CGLPixelFormatAttribute attributes[] = {
        kCGLPFAOpenGLProfile,
        (CGLPixelFormatAttribute) kCGLOGLPVersion_Legacy,
        kCGLPFAAccelerated,
        (CGLPixelFormatAttribute) 0
    };

    CGLPixelFormatObj pixelFormat;
    GLint num;
    CGLError error = CGLChoosePixelFormat(attributes, &pixelFormat, &num);
    if (error) {
        fprintf(stderr, "Error pixel format\n");
        return;
    }

    error = CGLCreateContext(pixelFormat, NULL, &gl_context);
    CGLDestroyPixelFormat(pixelFormat);
    if (error) {
        fprintf(stderr, "Error creating GL context object\n");
        return;
    }

    make_active();
}


void HeadlessView::resize(int width, int height) {
    clear_buffers();

    // Create depth/stencil buffer
    glGenRenderbuffersEXT(1, &fbo_depth_stencil);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_depth_stencil);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, width, height);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    glGenRenderbuffersEXT(1, &fbo_color);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_color);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, width, height);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, fbo_color);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER_EXT, fbo_depth_stencil);

    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        fprintf(stderr, "Couldn't create framebuffer: ");
        switch (status) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: fprintf(stderr, "incomplete attachment\n"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: fprintf(stderr, "incomplete missing attachment\n"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: fprintf(stderr, "incomplete draw buffer\n"); break;
            case GL_FRAMEBUFFER_UNSUPPORTED: fprintf(stderr, "unsupported\n"); break;
            default: fprintf(stderr, "other\n"); break;
        }
        return;
    }
}

void HeadlessView::clear_buffers() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    if (fbo) {
        glDeleteFramebuffersEXT(1, &fbo);
        fbo = 0;
    }

    if (fbo_color) {
        glDeleteTextures(1, &fbo_color);
        fbo_color = 0;
    }

    if (fbo_depth_stencil) {
        glDeleteRenderbuffersEXT(1, &fbo_depth_stencil);
        fbo_depth_stencil = 0;
    }
}

HeadlessView::~HeadlessView() {
    clear_buffers();
    CGLDestroyContext(gl_context);
}

void HeadlessView::make_active() {
    CGLError error = CGLSetCurrentContext(gl_context);
    if (error) {
        fprintf(stderr, "Switching OpenGL context failed\n");
    }
}

void HeadlessView::swap() {}

unsigned int HeadlessView::root_fbo() {
    return fbo;
}

}

