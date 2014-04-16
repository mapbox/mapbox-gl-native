#include <iostream>
#include "gtest/gtest.h"




#include <llmr/llmr.hpp>
#include <llmr/util/image.hpp>
#include <llmr/util/io.hpp>

double llmr::platform::elapsed() {
    return 0;
}

void llmr::platform::restart() {
    // TODO
}

std::shared_ptr<llmr::platform::Request>
llmr::platform::request_http(const std::string &url, std::function<void(Response *)> background_function,
                       std::function<void()> foreground_callback) {
    // TODO
    return nullptr;
}

void llmr::platform::cancel_request_http(const std::shared_ptr<Request> &req) {
    // TODO
}





TEST(Headless, initialize) {
    llmr::Settings settings;

    llmr::Map map(settings);


    // Setup OpenGL
    CGLContextObj gl_context;

    // TODO: test if OpenGL 4.1 with GL_ARB_ES2_compatibility is supported
    // If it is, use kCGLOGLPVersion_3_2_Core and enable that extension.
    CGLPixelFormatAttribute attributes[] = {
        kCGLPFAOpenGLProfile,
        (CGLPixelFormatAttribute) kCGLOGLPVersion_Legacy,
        kCGLPFAAccelerated,
        kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
        kCGLPFAAlphaSize, (CGLPixelFormatAttribute)8,
        kCGLPFAStencilSize, (CGLPixelFormatAttribute)8,
        kCGLPFADepthSize, (CGLPixelFormatAttribute)24,
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

    error = CGLSetCurrentContext(gl_context);
    if (error) {
        fprintf(stderr, "Switching OpenGL context failed\n");
        return;
    }


    int width = 1024;
    int height = 768;

    // Create depth/stencil buffer
    GLuint fbo_depth_stencil = 0;
    glGenRenderbuffersEXT(1, &fbo_depth_stencil);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_depth_stencil);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, width, height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER_EXT, fbo_depth_stencil);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    GLuint fbo_color = 0;
    glGenRenderbuffersEXT(1, &fbo_color);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_color);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, width, height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, fbo_color);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);


    GLuint fbo = 0;
    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, fbo_color);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo_depth_stencil);

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

    map.setup();
    map.resize(width, height);
    map.loadSettings();

    map.update();
    map.render();


    uint32_t *pixels = new uint32_t[width * height];

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    std::string result = llmr::util::compress_png(width, height, pixels, true);
    llmr::util::write_file("out.png", result);

    delete[] pixels;

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &fbo);
    glDeleteTextures(1, &fbo_color);
    glDeleteRenderbuffersEXT(1, &fbo_depth_stencil);

    CGLDestroyContext(gl_context);
}
