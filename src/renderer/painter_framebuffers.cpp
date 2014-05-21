#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/map/map.hpp>
#include <llmr/util/clip_ids.hpp>

using namespace llmr;


void Painter::clearFramebuffers() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    // Delete any framebuffers that we might have allocated
    glDeleteTextures(fbos_color.size(), fbos.data());
    fbos_color.clear();

    if (fbo_depth_stencil != 0) {
        glDeleteRenderbuffersEXT(1, &fbo_depth_stencil);
        fbo_depth_stencil = 0;
    }

    glDeleteFramebuffersEXT(fbos.size(), fbos.data());
    fbos.clear();
}

void Painter::bindFramebuffer() {
    if (fbo_level < 0) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } else if (fbos.size() > (size_t)fbo_level) {
        GLuint fbo = fbos[fbo_level];
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    } else {
        // TODO: Convert this to textures so we can composite them and use them in blend operations

        GLuint fbo_color;
        glGenTextures(1, &fbo_color);
        glBindTexture(GL_TEXTURE_2D, fbo_color);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, map.getState().getFramebufferWidth(), map.getState().getFramebufferHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        fbos_color.emplace_back(fbo_color);

        // Use a shared depth/stencil buffer for all framebuffer objects
        if (fbo_depth_stencil == 0) {
            // Create depth/stencil buffer
            glGenRenderbuffersEXT(1, &fbo_depth_stencil);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_depth_stencil);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, map.getState().getFramebufferWidth(), map.getState().getFramebufferHeight());
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
        }

        // Allocate new framebuffer
        GLuint fbo = 0;
        glGenFramebuffersEXT(1, &fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
        fbos.emplace_back(fbo);

        // Assemble the FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_color, 0);
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
}

void Painter::resetFramebuffer() {
    if (fbo_level >= 0) {
        fbo_level = -1;
        bindFramebuffer();
    }

    fbo_depth_stencil_valid = false;
}

void Painter::pushFramebuffer() {
    fbo_level++;
    bindFramebuffer();

    if (fbo_depth_stencil_valid) {
        glClear(GL_COLOR_BUFFER_BIT);
    } else {
        depthMask(true);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glColorMask(false, false, false, false);

        drawClippingMasks(map.getSources());

        glColorMask(true, true, true, true);
        fbo_depth_stencil_valid = true;
    }
}

GLuint Painter::popFramebuffer() {
    GLuint texture = fbos_color[fbo_level];
    fbo_level--;
    bindFramebuffer();
    return texture;
}
