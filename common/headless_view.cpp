#include "headless_view.hpp"
#include "headless_display.hpp"

#include <stdexcept>
#include <sstream>
#include <string>

#if MBGL_USE_GLX
#ifdef GLX_ARB_create_context
static PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = nullptr;
#endif
#endif

namespace mbgl {

HeadlessView::HeadlessView()
    : display_(std::make_shared<HeadlessDisplay>()) {
    createContext();
}

HeadlessView::HeadlessView(std::shared_ptr<HeadlessDisplay> display)
    : display_(display) {
    createContext();
}


#if MBGL_USE_GLX
#ifdef GLX_ARB_create_context

// These are all of the OpenGL Core profile version that we know about.
struct core_profile_version { int major, minor; };
static const core_profile_version core_profile_versions[] = {
    {4, 5},
    {4, 4},
    {4, 3},
    {4, 2},
    {4, 1},
    {4, 0},
    {3, 3},
    {3, 2},
    {3, 1},
    {3, 0},
    {0, 0},
};

GLXContext createCoreProfile(Display *dpy, GLXFBConfig fbconfig) {
    static bool context_creation_failed = false;
    GLXContext ctx = 0;

    // Set the Error Handler to avoid crashing the program when the context creation fails.
    // It is expected that some context creation attempts fail, e.g. because the OpenGL
    // implementation does not support the version we're requesting.
    int (*previous_error_handler)(Display *, XErrorEvent *) = XSetErrorHandler([](Display *, XErrorEvent *) {
        context_creation_failed = true;
        return 0;
    });

    // Try to create core profiles from the highest known version on down.
    for (int i = 0; !ctx && core_profile_versions[i].major; i++) {
        context_creation_failed = false;
        const int context_flags[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, core_profile_versions[i].major,
            GLX_CONTEXT_MINOR_VERSION_ARB, core_profile_versions[i].minor,
            None
        };
        ctx = glXCreateContextAttribsARB(dpy, fbconfig, 0, True, context_flags);
        if (context_creation_failed) {
            ctx = 0;
        }
    }

    // Restore the old error handler.
    XSetErrorHandler(previous_error_handler);
    return ctx;
}
#endif
#endif

void HeadlessView::createContext() {
#if MBGL_USE_CGL
    CGLError error = CGLCreateContext(display_->pixelFormat, NULL, &gl_context);
    if (error) {
        throw std::runtime_error("Error creating GL context object\n");
    }

    error = CGLEnable(gl_context, kCGLCEMPEngine);
    if (error != kCGLNoError ) {
        throw std::runtime_error("Error enabling OpenGL multithreading\n");
    }
#endif

#if MBGL_USE_GLX
#ifdef GLX_ARB_create_context
    if (glXCreateContextAttribsARB == nullptr) {
        glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
    }
#endif

    x_display = display_->x_display;
    fb_configs = display_->fb_configs;

#ifdef GLX_ARB_create_context
    if (glXCreateContextAttribsARB) {
        // Try to create a core profile context.
        gl_context = createCoreProfile(x_display, fb_configs[0]);
    }
#endif

    if (!gl_context) {
        // Try to create a legacy context
        gl_context = glXCreateNewContext(x_display, fb_configs[0], GLX_RGBA_TYPE, 0, True);
        if (gl_context) {
            if (!glXIsDirect(x_display, gl_context)) {
                glXDestroyContext(x_display, gl_context);
                gl_context = 0;
            }
        }
    }

    if (gl_context == 0) {
        throw std::runtime_error("Error creating GL context object");
    }

    // Create a dummy pbuffer. We will render to framebuffers anyway, but we need a pbuffer to
    // activate the context.
    int pbuffer_attributes[] = {
        GLX_PBUFFER_WIDTH, 8,
        GLX_PBUFFER_HEIGHT, 8,
        None
    };
    glx_pbuffer = glXCreatePbuffer(x_display, fb_configs[0], pbuffer_attributes);
#endif
}

void HeadlessView::resize(uint16_t width, uint16_t height, float pixelRatio) {
    clear_buffers();

    width_ = width;
    height_ = height;
    pixelRatio_ = pixelRatio;

    const unsigned int w = width_ * pixelRatio_;
    const unsigned int h = height_ * pixelRatio_;

    make_active();

    // Create depth/stencil buffer
    glGenRenderbuffersEXT(1, &fbo_depth_stencil);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_depth_stencil);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, w, h);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    glGenRenderbuffersEXT(1, &fbo_color);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_color);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, w, h);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, fbo_color);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER_EXT, fbo_depth_stencil);

    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        std::stringstream error("Couldn't create framebuffer: ");
        switch (status) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: (error << "incomplete attachment\n"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: error << "incomplete missing attachment\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: error << "incomplete dimensions\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: error << "incomplete formats\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: error << "incomplete draw buffer\n"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: error << "incomplete read buffer\n"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: error << "unsupported\n"; break;
            default: error << "other\n"; break;
        }
        throw std::runtime_error(error.str());
    }

    make_inactive();
}

const std::unique_ptr<uint32_t[]> HeadlessView::readPixels() {
    const unsigned int w = width_ * pixelRatio_;
    const unsigned int h = height_ * pixelRatio_;

    std::unique_ptr<uint32_t[]> pixels(new uint32_t[w * h]);

    make_active();
    glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());
    make_inactive();

    return pixels;
}

void HeadlessView::clear_buffers() {
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
}

HeadlessView::~HeadlessView() {
    clear_buffers();

#if MBGL_USE_CGL
    CGLDestroyContext(gl_context);
#endif

#if MBGL_USE_GLX
    if (glx_pbuffer) {
        glXDestroyPbuffer(x_display, glx_pbuffer);
        glx_pbuffer = 0;
    }

    glXDestroyContext(x_display, gl_context);
#endif
}

void HeadlessView::notify() {
    // no-op
}

void HeadlessView::notify_map_change(mbgl::MapChange /*change*/, mbgl::timestamp /*delay*/) {
    // no-op
}

void HeadlessView::make_active() {
#if MBGL_USE_CGL
    CGLError error = CGLSetCurrentContext(gl_context);
    if (error) {
        throw std::runtime_error("Switching OpenGL context failed\n");
    }
#endif

#if MBGL_USE_GLX
    if (!glXMakeContextCurrent(x_display, glx_pbuffer, glx_pbuffer, gl_context)) {
        throw std::runtime_error("Switching OpenGL context failed\n");
    }
#endif
}

void HeadlessView::make_inactive() {
#if MBGL_USE_CGL
    CGLError error = CGLSetCurrentContext(nullptr);
    if (error) {
        throw std::runtime_error("Removing OpenGL context failed\n");
    }
#endif

#if MBGL_USE_GLX
    if (!glXMakeContextCurrent(x_display, 0, 0, nullptr)) {
        throw std::runtime_error("Removing OpenGL context failed\n");
    }
#endif
}

void HeadlessView::swap() {}

unsigned int HeadlessView::root_fbo() {
    return fbo;
}

}

