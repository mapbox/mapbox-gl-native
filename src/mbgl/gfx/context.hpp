#pragma once

#include <mbgl/gfx/renderbuffer.hpp>
#include <mbgl/gfx/command_encoder.hpp>
#include <mbgl/gfx/draw_scope.hpp>
#include <mbgl/gfx/program.hpp>
#include <mbgl/gfx/types.hpp>
#include <mbgl/gfx/texture.hpp>

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
    virtual std::unique_ptr<OffscreenTexture>
        createOffscreenTexture(Size,
                               TextureChannelDataType = TextureChannelDataType::UnsignedByte) = 0;
    virtual std::unique_ptr<OffscreenTexture>
    createOffscreenTexture(Size,
                           Renderbuffer<RenderbufferPixelType::Depth>&,
                           TextureChannelDataType = TextureChannelDataType::UnsignedByte) = 0;

public:
    // Creates an empty texture with the specified dimensions.
    Texture createTexture(const Size size,
                          TexturePixelType format = TexturePixelType::RGBA,
                          TextureChannelDataType type = TextureChannelDataType::UnsignedByte) {
        return { size, createTextureResource(size, format, type) };
    }

protected:
    virtual std::unique_ptr<TextureResource>
        createTextureResource(Size, TexturePixelType, TextureChannelDataType) = 0;

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

    virtual void clearStencilBuffer(int32_t) = 0;
};

} // namespace gfx
} // namespace mbgl
