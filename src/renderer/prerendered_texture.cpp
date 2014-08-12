#include <mbgl/renderer/prerendered_texture.hpp>

#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/style_bucket.hpp>

using namespace mbgl;

PrerenderedTexture::PrerenderedTexture(const StyleBucketRaster &properties)
    : properties(properties) {
}

PrerenderedTexture::~PrerenderedTexture() {
    if (texture != 0) {
        glDeleteTextures(1, &texture);
        texture = 0;
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
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_fbo);

    if (texture == 0) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, properties.size, properties.size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (fbo_depth_stencil == 0) {
        // Create depth/stencil buffer
        glGenRenderbuffers(1, &fbo_depth_stencil);
        glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth_stencil);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, properties.size, properties.size);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    if (fbo == 0) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
#ifdef GL_ES_VERSION_2_0
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_depth_stencil);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo_depth_stencil);
#else
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo_depth_stencil);
#endif

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            fprintf(stderr, "Couldn't create framebuffer: ");
            switch (status) {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: fprintf(stderr, "incomplete attachment\n"); break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: fprintf(stderr, "incomplete missing attachment\n"); break;
#ifdef GL_ES_VERSION_2_0
                case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: fprintf(stderr, "incomplete dimensions\n"); break;
#else
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: fprintf(stderr, "incomplete draw buffer\n"); break;
#endif
                case GL_FRAMEBUFFER_UNSUPPORTED: fprintf(stderr, "unsupported\n"); break;
                default: fprintf(stderr, "other\n"); break;
            }
            return;
        }
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }
}

void PrerenderedTexture::unbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, previous_fbo);

    if (fbo != 0) {
        glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }
}

void PrerenderedTexture::blur(Painter& painter, uint16_t passes) {
    const GLuint original_texture = texture;

    // Create a secondary texture
    GLuint secondary_texture;
    glGenTextures(1, &secondary_texture);
    glBindTexture(GL_TEXTURE_2D, secondary_texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, properties.size, properties.size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);


    painter.useProgram(painter.gaussianShader->program);
    painter.gaussianShader->setMatrix(painter.flipMatrix);
    painter.gaussianShader->setImage(0);
    glActiveTexture(GL_TEXTURE0);

    for (int i = 0; i < passes; i++) {
        // Render horizontal
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, secondary_texture, 0);
#if GL_EXT_discard_framebuffer
        const GLenum discards[] = { GL_COLOR_ATTACHMENT0 };
        glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
#endif
        glClear(GL_COLOR_BUFFER_BIT);

        painter.gaussianShader->setOffset({{ 1.0f / float(properties.size), 0 }});
        glBindTexture(GL_TEXTURE_2D, original_texture);
        painter.coveringGaussianArray.bind(*painter.gaussianShader, painter.tileStencilBuffer, BUFFER_OFFSET(0));
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)painter.tileStencilBuffer.index());



        // Render vertical
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, original_texture, 0);
#if GL_EXT_discard_framebuffer
        glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
#endif
        glClear(GL_COLOR_BUFFER_BIT);

        painter.gaussianShader->setOffset({{ 0, 1.0f / float(properties.size) }});
        glBindTexture(GL_TEXTURE_2D, secondary_texture);
        painter.coveringGaussianArray.bind(*painter.gaussianShader, painter.tileStencilBuffer, BUFFER_OFFSET(0));
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)painter.tileStencilBuffer.index());
    }

    glDeleteTextures(1, &secondary_texture);
}
