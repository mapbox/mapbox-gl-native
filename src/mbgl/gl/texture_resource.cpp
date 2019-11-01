#include <mbgl/gl/context.hpp>
#include <mbgl/gl/texture_resource.hpp>

namespace mbgl {
namespace gl {

static int channelCount(gfx::TexturePixelType format) {
    switch (format) {
        case gfx::TexturePixelType::Alpha:
        case gfx::TexturePixelType::Depth:
        case gfx::TexturePixelType::Luminance:
        case gfx::TexturePixelType::Stencil:
            return 1;
        case gfx::TexturePixelType::RGBA:
            return 4;
        default:
            assert(!"Unknown texture pixel type");
            return 0;
    }
}

static int channelStorageSize(gfx::TextureChannelDataType type) {
    switch (type) {
        case gfx::TextureChannelDataType::HalfFloat:
            return 2;
        case gfx::TextureChannelDataType::UnsignedByte:
            return 1;
        default:
            assert(!"Unknown texture channel data type");
            return 0;
    }
}

TextureResource::~TextureResource() {
    auto& stats = texture.get_deleter().context->renderingStats();
    stats.memTextures -= byteSize;
    assert(stats.memTextures >= 0);
}

int TextureResource::getStorageSize(const Size& size, gfx::TexturePixelType format, gfx::TextureChannelDataType type) {
    return size.width * size.height * channelCount(format) * channelStorageSize(type);
}

} // namespace gl
} // namespace mbgl