#include "headless_view.hpp"
#include "headless_display.hpp"

#include <stdexcept>

namespace mbgl {

HeadlessView::HeadlessView() : display_ptr_(std::make_shared<HeadlessDisplay>()) {
    createContext();
}

HeadlessView::HeadlessView(std::shared_ptr<HeadlessDisplay> display_ptr)
    : display_ptr_(display_ptr) {
    createContext();
}

void HeadlessView::createContext() {
#if MBGL_USE_CGL
    CGLError error = CGLCreateContext(display_ptr_->pixelFormat, NULL, &gl_context);
    if (error) {
        fprintf(stderr, "Error creating GL context object\n");
        return;
    }

    error = CGLEnable(gl_context, kCGLCEMPEngine);
    if (error != kCGLNoError ) {
        fprintf(stderr, "Error enabling OpenGL multithreading\n");
        return;
    }
#endif

#if MBGL_USE_GLX
    x_display = display_ptr_->x_display;
    x_info = display_ptr_->x_info;

    gl_context = glXCreateContext(x_display, x_info, 0, GL_TRUE);
    if (gl_context == nullptr) {
        throw std::runtime_error("Error creating GL context object");
    }
#endif
}

void HeadlessView::resize(uint16_t width, uint16_t height, float pixelRatio) {
    clear_buffers();

    width *= pixelRatio;
    height *= pixelRatio;

#if MBGL_USE_CGL
    make_active();

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
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: fprintf(stderr, "incomplete dimensions\n"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: fprintf(stderr, "incomplete formats\n"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: fprintf(stderr, "incomplete draw buffer\n"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: fprintf(stderr, "incomplete read buffer\n"); break;
            case GL_FRAMEBUFFER_UNSUPPORTED: fprintf(stderr, "unsupported\n"); break;
            default: fprintf(stderr, "other\n"); break;
        }
        return;
    }

    make_inactive();
#endif

#if MBGL_USE_GLX
    x_pixmap = XCreatePixmap(x_display, DefaultRootWindow(x_display), width, height, 32);
    glx_pixmap = glXCreateGLXPixmap(x_display, x_info, x_pixmap);
#endif
}

void HeadlessView::clear_buffers() {
#if MBGL_USE_CGL
    make_active();

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

    make_inactive();
#endif

#if MBGL_USE_GLX
    if (glx_pixmap) {
        glXDestroyGLXPixmap(x_display, glx_pixmap);
        glx_pixmap = 0;
    }

    if (x_pixmap) {
        XFreePixmap(x_display, x_pixmap);
        x_pixmap = 0;
    }

    make_inactive();
#endif
}

HeadlessView::~HeadlessView() {
    clear_buffers();

#if MBGL_USE_CGL
    CGLDestroyContext(gl_context);
#endif

#if MBGL_USE_GLX
    glXDestroyContext(x_display, gl_context);
#endif
}

void HeadlessView::notify_map_change(mbgl::MapChange /*change*/, mbgl::timestamp /*delay*/) {
    // no-op
}

void HeadlessView::make_active() {
#if MBGL_USE_CGL
    CGLError error = CGLSetCurrentContext(gl_context);
    if (error) {
        fprintf(stderr, "Switching OpenGL context failed\n");
    }
#endif

#if MBGL_USE_GLX
    if (!glXMakeCurrent(x_display, glx_pixmap, gl_context)) {
        fprintf(stderr, "Switching OpenGL context failed\n");
    }
#endif
}

void HeadlessView::make_inactive() {
#if MBGL_USE_CGL
    CGLError error = CGLSetCurrentContext(nullptr);
    if (error) {
        fprintf(stderr, "Removing OpenGL context failed\n");
    }
#endif

#if MBGL_USE_GLX
    if (!glXMakeCurrent(x_display, 0, NULL)) {
        fprintf(stderr, "Removing OpenGL context failed\n");
    }
#endif
}

void HeadlessView::swap() {}

unsigned int HeadlessView::root_fbo() {
#if MBGL_USE_CGL
    return fbo;
#endif

    return 0;
}

}

