#pragma once

#include <mbgl/gfx/vertex_vector.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_vector.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/gfx/draw_scope.hpp>
#include <mbgl/gfx/program.hpp>
#include <mbgl/gfx/types.hpp>

namespace mbgl {

class ProgramParameters;

namespace gfx {

class Context {
protected:
    Context(ContextType type_, uint32_t maximumVertexBindingCount_)
        : backend(type_), maximumVertexBindingCount(maximumVertexBindingCount_) {
    }

public:
    const ContextType backend;
    static constexpr const uint32_t minimumRequiredVertexBindingCount = 8;
    const uint32_t maximumVertexBindingCount;

public:
    Context(Context&&) = delete;
    Context(const Context&) = delete;
    Context& operator=(Context&& other) = delete;
    Context& operator=(const Context& other) = delete;
    virtual ~Context() = default;

public:
    // Called at the end of a frame.
    virtual void performCleanup() = 0;

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
        updateVertexBufferResource(*buffer.resource, v.data(), v.bytes());
    }

    template <class DrawMode>
    IndexBuffer createIndexBuffer(IndexVector<DrawMode>&& v,
                                  const BufferUsageType usage = BufferUsageType::StaticDraw) {
        return { v.elements(), createIndexBufferResource(v.data(), v.bytes(), usage) };
    }

    template <class DrawMode>
    void updateIndexBuffer(IndexBuffer& buffer, IndexVector<DrawMode>&& v) {
        assert(v.elements() == buffer.elements);
        updateIndexBufferResource(*buffer.resource, v.data(), v.bytes());
    }

protected:
    virtual std::unique_ptr<const VertexBufferResource>
    createVertexBufferResource(const void* data, std::size_t size, const BufferUsageType) = 0;
    virtual void
    updateVertexBufferResource(const VertexBufferResource&, const void* data, std::size_t size) = 0;

    virtual std::unique_ptr<const IndexBufferResource>
    createIndexBufferResource(const void* data, std::size_t size, const BufferUsageType) = 0;
    virtual void
    updateIndexBufferResource(const IndexBufferResource&, const void* data, std::size_t size) = 0;

public:
    // Create a texture from an image with data.
    template <typename Image>
    Texture createTexture(const Image& image,
                          TextureChannelDataType type = TextureChannelDataType::UnsignedByte) {
        auto format = image.channels == 4 ? TexturePixelType::RGBA : TexturePixelType::Alpha;
        return { image.size,
                 createTextureResource(image.size, image.data.get(), format, type) };
    }

    // Creates an empty texture with the specified dimensions.
    Texture createTexture(const Size size,
                          TexturePixelType format = TexturePixelType::RGBA,
                          TextureChannelDataType type = TextureChannelDataType::UnsignedByte) {
        return { size, createTextureResource(size, nullptr, format, type) };
    }

    template <typename Image>
    void updateTexture(Texture& texture,
                       const Image& image,
                       TextureChannelDataType type = TextureChannelDataType::UnsignedByte) {
        auto format = image.channels == 4 ? TexturePixelType::RGBA : TexturePixelType::Alpha;
        updateTextureResource(*texture.resource, image.size, image.data.get(), format, type);
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
        updateTextureResourceSub(*texture.resource, offsetX, offsetY, image.size, image.data.get(), format, type);
    }

protected:
    virtual std::unique_ptr<TextureResource> createTextureResource(
        Size, const void* data, TexturePixelType, TextureChannelDataType) = 0;
    virtual void updateTextureResource(const TextureResource&, Size, const void* data,
        TexturePixelType, TextureChannelDataType) = 0;
    virtual void updateTextureResourceSub(const TextureResource&, uint16_t xOffset, uint16_t yOffset, Size, const void* data,
        TexturePixelType, TextureChannelDataType) = 0;

public:
    DrawScope createDrawScope() {
        return { createDrawScopeResource() };
    }

protected:
    virtual std::unique_ptr<DrawScopeResource> createDrawScopeResource() = 0;

public:
    template <typename Name>
    std::unique_ptr<Program<Name>> createProgram(const ProgramParameters&);

private:
    template <typename Backend, typename Name>
    std::unique_ptr<Program<Name>> createProgram(const ProgramParameters&);
};

} // namespace gfx
} // namespace mbgl
