#include <iostream>
#include "gtest/gtest.h"




#include <llmr/llmr.hpp>
#include <llmr/util/image.hpp>
#include <llmr/util/io.hpp>
#include <llmr/util/timer.hpp>

#include <uv.h>

class View : public llmr::View {
public:
    void make_active() {
        CGLError error = CGLSetCurrentContext(gl_context);
        if (error) {
            fprintf(stderr, "Switching OpenGL context failed\n");
        }
    }
    void swap() {}

CGLContextObj gl_context;
};

TEST(Headless, initialize) {
    llmr::util::timer timer;

    // Setup OpenGL
    View view;

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

    error = CGLCreateContext(pixelFormat, NULL, &view.gl_context);
    CGLDestroyPixelFormat(pixelFormat);
    if (error) {
        fprintf(stderr, "Error creating GL context object\n");
        return;
    }

    view.make_active();

    timer.report("gl setup");


    int width = 1024;
    int height = 768;

    // Create depth/stencil buffer
    GLuint fbo_depth_stencil = 0;
    glGenRenderbuffersEXT(1, &fbo_depth_stencil);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_depth_stencil);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, width, height);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    GLuint fbo_color = 0;
    glGenRenderbuffersEXT(1, &fbo_color);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_color);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, width, height);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    GLuint fbo = 0;
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

    timer.report("gl framebuffer");


    llmr::Map map(view);

    map.resize(width, height);

    timer.report("map resize");

    // Run the loop. It will terminate when we don't have any further listeners.
    map.run();

    timer.report("map loop");

    uint32_t *pixels = new uint32_t[width * height];

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    timer.report("gl readpixels");

    std::string result = llmr::util::compress_png(width, height, pixels, true);

    timer.report("compress png");

    llmr::util::write_file("out.png", result);

    timer.report("save file");


    delete[] pixels;

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &fbo);
    glDeleteTextures(1, &fbo_color);
    glDeleteRenderbuffersEXT(1, &fbo_depth_stencil);

    CGLDestroyContext(view.gl_context);

    timer.report("destruct");
}
