#include <mbgl/util/offscreen_texture.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>

#include <cstring>
#include <cassert>

namespace mbgl {

OffscreenTexture::OffscreenTexture(gl::Context& context_, const Size size_)
    : size(std::move(size_)), context(context_) {
    assert(size);
}

void OffscreenTexture::bind() {
    if (!framebuffer) {
        texture = context.createTexture(size);
        framebuffer = context.createFramebuffer(*texture);
    } else {
        context.bindFramebuffer = framebuffer->framebuffer;
    }

    context.viewport = { 0, 0, size };
}

gl::Texture& OffscreenTexture::getTexture() {
    assert(texture);
    return *texture;
}

PremultipliedImage OffscreenTexture::readStillImage() {
    PremultipliedImage image { size };
    MBGL_CHECK_ERROR(
        glReadPixels(0, 0, size.width, size.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data.get()));

    const auto stride = image.stride();
    auto tmp = std::make_unique<uint8_t[]>(stride);
    uint8_t* rgba = image.data.get();
    for (int i = 0, j = size.height - 1; i < j; i++, j--) {
        std::memcpy(tmp.get(), rgba + i * stride, stride);
        std::memcpy(rgba + i * stride, rgba + j * stride, stride);
        std::memcpy(rgba + j * stride, tmp.get(), stride);
    }

    return image;
}


} // namespace mbgl
