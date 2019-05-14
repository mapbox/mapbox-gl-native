#pragma once

#include <mbgl/gfx/debug_group.hpp>
#include <mbgl/gfx/vertex_vector.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_vector.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/util/size.hpp>

namespace mbgl {
namespace gfx {

class UploadPass {
protected:
    UploadPass() = default;

    friend class DebugGroup<UploadPass>;
    virtual void pushDebugGroup(const char* name) = 0;
    virtual void popDebugGroup() = 0;

public:
    virtual ~UploadPass() = default;
    UploadPass(const UploadPass&) = delete;
    UploadPass& operator=(const UploadPass&) = delete;

    DebugGroup<UploadPass> createDebugGroup(const char* name) {
        return { *this, name };
    }

public:
    template <class Vertex>
    VertexBuffer<Vertex>
    createVertexBuffer(VertexVector<Vertex>&& v,
                       const BufferUsageType usage = BufferUsageType::StaticDraw) {
        return { v.elements(), createVertexBufferResource(v.data(), v.bytes(), usage) };
    }

    template <class Vertex>
    void updateVertexBuffer(VertexBuffer<Vertex>& buffer, VertexVector<Vertex>&& v) {
        assert(v.elements() == buffer.elements);
        updateVertexBufferResource(buffer.getResource(), v.data(), v.bytes());
    }

    template <class DrawMode>
    IndexBuffer createIndexBuffer(IndexVector<DrawMode>&& v,
                                  const BufferUsageType usage = BufferUsageType::StaticDraw) {
        return { v.elements(), createIndexBufferResource(v.data(), v.bytes(), usage) };
    }

    template <class DrawMode>
    void updateIndexBuffer(IndexBuffer& buffer, IndexVector<DrawMode>&& v) {
        assert(v.elements() == buffer.elements);
        updateIndexBufferResource(buffer.getResource(), v.data(), v.bytes());
    }

protected:
    virtual std::unique_ptr<VertexBufferResource>
    createVertexBufferResource(const void* data, std::size_t size, const BufferUsageType) = 0;
    virtual void
    updateVertexBufferResource(VertexBufferResource&, const void* data, std::size_t size) = 0;

    virtual std::unique_ptr<IndexBufferResource>
    createIndexBufferResource(const void* data, std::size_t size, const BufferUsageType) = 0;
    virtual void
    updateIndexBufferResource(IndexBufferResource&, const void* data, std::size_t size) = 0;

public:
    // Create a texture from an image with data.
    template <typename Image>
    Texture createTexture(const Image& image,
                          TextureChannelDataType type = TextureChannelDataType::UnsignedByte) {
        auto format = image.channels == 4 ? TexturePixelType::RGBA : TexturePixelType::Alpha;
        return { image.size,
                 createTextureResource(image.size, image.data.get(), format, type) };
    }

    template <typename Image>
    void updateTexture(Texture& texture,
                       const Image& image,
                       TextureChannelDataType type = TextureChannelDataType::UnsignedByte) {
        auto format = image.channels == 4 ? TexturePixelType::RGBA : TexturePixelType::Alpha;
        updateTextureResource(texture.getResource(), image.size, image.data.get(), format, type);
        texture.size = image.size;
    }

    template <typename Image>
    void updateTextureSub(Texture& texture,
                          const Image& image,
                          const uint16_t offsetX,
                          const uint16_t offsetY,
                          TextureChannelDataType type = TextureChannelDataType::UnsignedByte) {
        assert(image.size.width + offsetX <= texture.size.width);
        assert(image.size.height + offsetY <= texture.size.height);
        auto format = image.channels == 4 ? TexturePixelType::RGBA : TexturePixelType::Alpha;
        updateTextureResourceSub(texture.getResource(), offsetX, offsetY, image.size, image.data.get(), format, type);
    }

protected:
    virtual std::unique_ptr<TextureResource> createTextureResource(
        Size, const void* data, TexturePixelType, TextureChannelDataType) = 0;
    virtual void updateTextureResource(TextureResource&, Size, const void* data,
        TexturePixelType, TextureChannelDataType) = 0;
    virtual void updateTextureResourceSub(TextureResource&, uint16_t xOffset, uint16_t yOffset, Size, const void* data,
        TexturePixelType, TextureChannelDataType) = 0;
};

} // namespace gfx
} // namespace mbgl
