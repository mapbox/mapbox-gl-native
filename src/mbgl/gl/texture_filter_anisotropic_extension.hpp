#pragma once

#include <mbgl/gl/features.hpp>
#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/gl.hpp>

#define GL_TEXTURE_MAX_ANISOTROPY                  0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY              0x84FF

namespace mbgl {
namespace gl {
namespace extension {

class TextureFilterAnisotropic {
public:
    template <typename Fn>
    TextureFilterAnisotropic(const Fn& loadExtension)
        : available(loadExtension({ "GL_EXT_texture_filter_anisotropic" })),
          maximum([this] {
              float value = 1.0f;
              if (available) {
                  MBGL_CHECK_ERROR(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &value));
              }
              return value;
          }()) {
    }

    const bool available;
    const TextureAnisotropic maximum;
};

} // namespace extension
} // namespace gl
} // namespace mbgl
