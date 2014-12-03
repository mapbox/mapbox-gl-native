#include <mbgl/renderer/prerendered_texture.hpp>

#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/style_bucket.hpp>

#include <mbgl/platform/log.hpp>

using namespace mbgl;

PrerenderedTexture::PrerenderedTexture(const StyleBucketRaster &properties_)
    : properties(properties_) {
}

PrerenderedTexture::~PrerenderedTexture() {
    if (texture != 0) {
        glDeleteTextures(1, &texture);
        texture = 0;
    }

    if (fboDepth != 0) {
        glDeleteRenderbuffers(1, &fboDepth);
        fboDepth = 0;
    }

    if (fboStencil != 0) {
        glDeleteRenderbuffers(1, &fboStencil);
        fboStencil = 0;
    }

    if (fbo != 0) {
        glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }
}


void PrerenderedTexture::bindTexture() {
    if (texture == 0) {
        bindFramebuffer();
        unbindFramebuffer();
    }

    glBindTexture(GL_TEXTURE_2D, texture);
}

void PrerenderedTexture::bindFramebuffer() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFbo);

    if (texture == 0) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
#ifndef GL_ES_VERSION_2_0
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
#endif
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, properties.size, properties.size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (fboDepth == 0) {
        // Create depth buffer
        glGenRenderbuffers(1, &fboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
        if (gl::isPackedDepthStencilSupported) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, properties.size, properties.size);
        } else {
            if (gl::isDepth24Supported) {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, properties.size, properties.size);
            } else {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, properties.size, properties.size);
            }
        }

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    if (!gl::isPackedDepthStencilSupported && (fboStencil == 0)) {
        // Create stencil buffer
        glGenRenderbuffers(1, &fboStencil);
        glBindRenderbuffer(GL_RENDERBUFFER, fboStencil);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, properties.size, properties.size);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    if (fbo == 0) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        if (gl::isPackedDepthStencilSupported) {
#ifdef GL_ES_VERSION_2_0
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepth);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepth);
#else
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepth);
#endif
        } else {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepth);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboStencil);
        }

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Couldn't create framebuffer: ");
            switch (status) {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: mbgl::Log::Error(mbgl::Event::OpenGL, "incomplete attachment\n"); break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: mbgl::Log::Error(mbgl::Event::OpenGL, "incomplete missing attachment\n"); break;
#ifdef GL_ES_VERSION_2_0
                case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: mbgl::Log::Error(mbgl::Event::OpenGL, "incomplete dimensions\n"); break;
#else
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: mbgl::Log::Error(mbgl::Event::OpenGL, "incomplete draw buffer\n"); break;
#endif
                case GL_FRAMEBUFFER_UNSUPPORTED: mbgl::Log::Error(mbgl::Event::OpenGL, "unsupported\n"); break;
                default: mbgl::Log::Error(mbgl::Event::OpenGL, "other\n"); break;
            }
            return;
        }
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }
}

void PrerenderedTexture::unbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, previousFbo);

    if (fbo != 0) {
        glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }
}

void PrerenderedTexture::blur(Painter& painter, uint16_t passes) {
    const GLuint originalTexture = texture;

    // Create a secondary texture
    GLuint secondaryTexture;
    glGenTextures(1, &secondaryTexture);
    glBindTexture(GL_TEXTURE_2D, secondaryTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, properties.size, properties.size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);


    painter.useProgram(painter.gaussianShader->program);
    painter.gaussianShader->u_matrix = painter.flipMatrix;
    painter.gaussianShader->u_image = 0;
    glActiveTexture(GL_TEXTURE0);

    for (int i = 0; i < passes; i++) {
        // Render horizontal
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, secondaryTexture, 0);
#if GL_EXT_discard_framebuffer && !__ANDROID__
        const GLenum discards[] = { GL_COLOR_ATTACHMENT0 };
        glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
#endif
        glClear(GL_COLOR_BUFFER_BIT);

        painter.gaussianShader->u_offset = {{ 1.0f / float(properties.size), 0 }};
        glBindTexture(GL_TEXTURE_2D, originalTexture);
        painter.coveringGaussianArray.bind(*painter.gaussianShader, painter.tileStencilBuffer, BUFFER_OFFSET(0));
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)painter.tileStencilBuffer.index());



        // Render vertical
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, originalTexture, 0);
#if GL_EXT_discard_framebuffer && !__ANDROID__
        glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
#endif
        glClear(GL_COLOR_BUFFER_BIT);

        painter.gaussianShader->u_offset = {{ 0, 1.0f / float(properties.size) }};
        glBindTexture(GL_TEXTURE_2D, secondaryTexture);
        painter.coveringGaussianArray.bind(*painter.gaussianShader, painter.tileStencilBuffer, BUFFER_OFFSET(0));
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)painter.tileStencilBuffer.index());
    }

    glDeleteTextures(1, &secondaryTexture);
}
