#include <mbgl/gl/gl_config.hpp>
#include <mbgl/util/offscreen_texture.hpp>

#include <cassert>

namespace mbgl {

void OffscreenTexture::bind(gl::ObjectStore& store,
                            gl::Config& config,
                            std::array<uint16_t, 2> size) {
    assert(size[0] > 0 && size[1] > 0);

    if (raster.getSize() != size) {
        raster.load(PremultipliedImage(size[0], size[1], nullptr));
        raster.upload(store, config, 0);
    }

    if (!fbo) {
        fbo = store.createFBO();
        config.bindFramebuffer = *fbo;
        MBGL_CHECK_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                                raster.getID(), 0));

        GLenum status = MBGL_CHECK_ERROR(glCheckFramebufferStatus(GL_FRAMEBUFFER));
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            switch (status) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                throw std::runtime_error("Couldn't create framebuffer: incomplete attachment");
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                throw std::runtime_error(
                    "Couldn't create framebuffer: incomplete missing attachment");
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                throw std::runtime_error("Couldn't create framebuffer: incomplete draw buffer");
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                throw std::runtime_error("Couldn't create framebuffer: incomplete read buffer");
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
                throw std::runtime_error("Couldn't create framebuffer: incomplete dimensions");
#endif

            case GL_FRAMEBUFFER_UNSUPPORTED:
                throw std::runtime_error("Couldn't create framebuffer: unsupported");
            default:
                throw std::runtime_error("Couldn't create framebuffer: other");
            }
        }
    } else {
        config.bindFramebuffer = *fbo;
    }

    config.viewport = { { 0, 0, static_cast<GLint>(size[0]), static_cast<GLint>(size[1]) } };
}

Raster& OffscreenTexture::getTexture() {
    return raster;
}

std::array<uint16_t, 2> OffscreenTexture::getSize() const {
    return raster.getSize();
}

} // namespace mbgl
