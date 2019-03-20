#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/texture_resource.hpp>
#include <mbgl/gl/defines.hpp>

namespace mbgl {
namespace gl {

using namespace platform;

void bindTexture(gl::Context& context, const uint8_t unit, const gfx::TextureBinding& binding) {
    auto& resource = static_cast<gl::TextureResource&>(*binding.resource);
    if (binding.filter != resource.filter || binding.mipmap != resource.mipmap ||
        binding.wrapX != resource.wrapX || binding.wrapY != resource.wrapY) {
        context.activeTextureUnit = unit;
        context.texture[unit] = resource.texture;

        if (binding.filter != resource.filter || binding.mipmap != resource.mipmap) {
            MBGL_CHECK_ERROR(glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                binding.filter == gfx::TextureFilterType::Linear
                    ? (binding.mipmap == gfx::TextureMipMapType::Yes ? GL_LINEAR_MIPMAP_NEAREST
                                                                     : GL_LINEAR)
                    : (binding.mipmap == gfx::TextureMipMapType::Yes ? GL_NEAREST_MIPMAP_NEAREST
                                                                     : GL_NEAREST)));
            MBGL_CHECK_ERROR(glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                binding.filter == gfx::TextureFilterType::Linear ? GL_LINEAR : GL_NEAREST));
            resource.filter = binding.filter;
            resource.mipmap = binding.mipmap;
        }
        if (binding.wrapX != resource.wrapX) {

            MBGL_CHECK_ERROR(glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                binding.wrapX == gfx::TextureWrapType::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT));
            resource.wrapX = binding.wrapX;
        }
        if (binding.wrapY != resource.wrapY) {
            MBGL_CHECK_ERROR(glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                binding.wrapY == gfx::TextureWrapType::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT));
            resource.wrapY = binding.wrapY;
        }
    } else if (context.texture[unit] != resource.texture) {
        // We are checking first to avoid setting the active texture without a subsequent
        // texture bind.
        context.activeTextureUnit = unit;
        context.texture[unit] = resource.texture;
    }
}

} // namespace gl
} // namespace mbgl
