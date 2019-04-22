#pragma once

#include <mbgl/gfx/vertex_vector.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_vector.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/gfx/renderbuffer.hpp>
#include <mbgl/gfx/command_encoder.hpp>
#include <mbgl/gfx/draw_scope.hpp>
#include <mbgl/gfx/program.hpp>
#include <mbgl/gfx/types.hpp>

namespace mbgl {

class ProgramParameters;

namespace gfx {

class OffscreenTexture;

class Context {
protected:
    Context(ContextType type_, uint32_t maximumVertexBindingCount_)
        : backend(type_), maximumVertexBindingCount(maximumVertexBindingCount_) {
    }

public:
    const ContextType backend;
    static constexpr const uint32_t minimumRequiredVertexBindingCount = 8;
    const uint32_t maximumVertexBindingCount;
    bool supportsHalfFloatTextures = false;

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

public:
    virtual std::unique_ptr<gfx::OffscreenTexture> createOffscreenTexture(
        Size, gfx::TextureChannelDataType = gfx::TextureChannelDataType::UnsignedByte) = 0;
    virtual std::unique_ptr<gfx::OffscreenTexture> createOffscreenTexture(
        Size,
        gfx::Renderbuffer<gfx::RenderbufferPixelType::Depth>&,
        gfx::TextureChannelDataType = gfx::TextureChannelDataType::UnsignedByte) = 0;

public:
    template <RenderbufferPixelType pixelType>
    Renderbuffer<pixelType>
    createRenderbuffer(const Size size) {
        return { size, createRenderbufferResource(pixelType, size) };
    }

protected:
    virtual std::unique_ptr<RenderbufferResource>
    createRenderbufferResource(RenderbufferPixelType, Size) = 0;

public:
    DrawScope createDrawScope() {
        return DrawScope{ createDrawScopeResource() };
    }

protected:
    virtual std::unique_ptr<DrawScopeResource> createDrawScopeResource() = 0;

public:
    template <typename Name>
    std::unique_ptr<Program<Name>> createProgram(const ProgramParameters&);

private:
    template <typename Backend, typename Name>
    std::unique_ptr<Program<Name>> createProgram(const ProgramParameters&);

public:
    virtual std::unique_ptr<CommandEncoder> createCommandEncoder() = 0;

#if not defined(NDEBUG)
public:
    virtual void visualizeStencilBuffer() = 0;
    virtual void visualizeDepthBuffer(float depthRangeSize) = 0;
#endif
};

} // namespace gfx
} // namespace mbgl
