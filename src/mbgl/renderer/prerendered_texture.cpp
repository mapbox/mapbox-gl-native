#include <mbgl/renderer/prerendered_texture.hpp>

#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/style_bucket.hpp>

using namespace mbgl;

PrerenderedTexture::PrerenderedTexture(const StyleBucketRaster &properties_)
    : properties(properties_) {
}

PrerenderedTexture::~PrerenderedTexture() {
    if (texture != 0) {
        CHECK_ERROR(glDeleteTextures(1, &texture));
        texture = 0;
    }

    if (fbo != 0) {
        CHECK_ERROR(glDeleteFramebuffers(1, &fbo));
        fbo = 0;
    }
}


void PrerenderedTexture::bindTexture() {
    if (texture == 0) {
        bindFramebuffer();
        unbindFramebuffer();
    }

    CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
}

void PrerenderedTexture::bindFramebuffer() {
    CHECK_ERROR(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_fbo));

    if (texture == 0) {
        CHECK_ERROR(glGenTextures(1, &texture));
        CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
#ifndef GL_ES_VERSION_2_0
        CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
#endif
        CHECK_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        CHECK_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        CHECK_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        CHECK_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, properties.size, properties.size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
        CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
    }

    if (fbo_depth_stencil == 0) {
        // Create depth/stencil buffer
        CHECK_ERROR(glGenRenderbuffers(1, &fbo_depth_stencil));
        CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth_stencil));
#ifdef GL_ES_VERSION_2_0
        CHECK_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, properties.size, properties.size));
#else
        CHECK_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, properties.size, properties.size));
#endif
        CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }

    if (fbo == 0) {
        CHECK_ERROR(glGenFramebuffers(1, &fbo));
        CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
        CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0));
#ifdef GL_ES_VERSION_2_0
        CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_depth_stencil));
        CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo_depth_stencil));
#else
        CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo_depth_stencil));
#endif

        GLenum status = CHECK_ERROR(glCheckFramebufferStatus(GL_FRAMEBUFFER));
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
        CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
    }
}

void PrerenderedTexture::unbindFramebuffer() {
    CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, previous_fbo));

    if (fbo != 0) {
        CHECK_ERROR(glDeleteFramebuffers(1, &fbo));
        fbo = 0;
    }
}

void PrerenderedTexture::blur(Painter& painter, uint16_t passes) {
    const GLuint original_texture = texture;

    // Create a secondary texture
    GLuint secondary_texture;
    CHECK_ERROR(glGenTextures(1, &secondary_texture));
    CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, secondary_texture));
    CHECK_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    CHECK_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    CHECK_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CHECK_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, properties.size, properties.size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
    CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, 0));


    painter.useProgram(painter.gaussianShader->program);
    painter.gaussianShader->u_matrix = painter.flipMatrix;
    painter.gaussianShader->u_image = 0;
    CHECK_ERROR(glActiveTexture(GL_TEXTURE0));

    for (int i = 0; i < passes; i++) {
        // Render horizontal
        CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, secondary_texture, 0));
#if GL_EXT_discard_framebuffer
        const GLenum discards[] = { GL_COLOR_ATTACHMENT0 };
        CHECK_ERROR(glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards));
#endif
        CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT));

        painter.gaussianShader->u_offset = {{ 1.0f / float(properties.size), 0 }};
        CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, original_texture));
        painter.coveringGaussianArray.bind(*painter.gaussianShader, painter.tileStencilBuffer, BUFFER_OFFSET(0));
        CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)painter.tileStencilBuffer.index()));



        // Render vertical
        CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, original_texture, 0));
#if GL_EXT_discard_framebuffer
        CHECK_ERROR(glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards));
#endif
        CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT));

        painter.gaussianShader->u_offset = {{ 0, 1.0f / float(properties.size) }};
        CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, secondary_texture));
        painter.coveringGaussianArray.bind(*painter.gaussianShader, painter.tileStencilBuffer, BUFFER_OFFSET(0));
        CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)painter.tileStencilBuffer.index()));
    }

    CHECK_ERROR(glDeleteTextures(1, &secondary_texture));
}
