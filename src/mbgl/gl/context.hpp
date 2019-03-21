#pragma once

#include <mbgl/gfx/context.hpp>
#include <mbgl/gl/features.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/gl/state.hpp>
#include <mbgl/gl/value.hpp>
#include <mbgl/gl/renderbuffer.hpp>
#include <mbgl/gl/framebuffer.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/gl/types.hpp>
#include <mbgl/gfx/draw_mode.hpp>
#include <mbgl/gfx/depth_mode.hpp>
#include <mbgl/gfx/stencil_mode.hpp>
#include <mbgl/gfx/color_mode.hpp>
#include <mbgl/platform/gl_functions.hpp>
#include <mbgl/util/noncopyable.hpp>


#include <functional>
#include <memory>
#include <vector>
#include <array>
#include <string>

namespace mbgl {
namespace gl {

constexpr size_t TextureMax = 64;
using ProcAddress = void (*)();

namespace extension {
class VertexArray;
class Debugging;
class ProgramBinary;
} // namespace extension

class Context final : public gfx::Context {
public:
    Context();
    ~Context() override;
    Context(const Context&) = delete;
    Context& operator=(const Context& other) = delete;

    void initializeExtensions(const std::function<gl::ProcAddress(const char*)>&);

    void enableDebugging();

    UniqueShader createShader(ShaderType type, const std::initializer_list<const char*>& sources);
    UniqueProgram createProgram(ShaderID vertexShader, ShaderID fragmentShader);
    UniqueProgram createProgram(BinaryProgramFormat binaryFormat, const std::string& binaryProgram);
    void verifyProgramLinkage(ProgramID);
    void linkProgram(ProgramID);
    UniqueTexture createUniqueTexture();

#if MBGL_HAS_BINARY_PROGRAMS
    bool supportsProgramBinaries() const;
#else
    constexpr bool supportsProgramBinaries() const { return false; }
#endif
    optional<std::pair<BinaryProgramFormat, std::string>> getBinaryProgram(ProgramID) const;

    template <RenderbufferType type>
    Renderbuffer<type> createRenderbuffer(const Size size) {
        static_assert(type == RenderbufferType::RGBA ||
                      type == RenderbufferType::DepthStencil ||
                      type == RenderbufferType::DepthComponent,
                      "invalid renderbuffer type");
        return { size, createRenderbuffer(type, size) };
    }

    Framebuffer createFramebuffer(const Renderbuffer<RenderbufferType::RGBA>&,
                                  const Renderbuffer<RenderbufferType::DepthStencil>&);
    Framebuffer createFramebuffer(const Renderbuffer<RenderbufferType::RGBA>&);
    Framebuffer createFramebuffer(const gfx::Texture&,
                                  const Renderbuffer<RenderbufferType::DepthStencil>&);
    Framebuffer createFramebuffer(const gfx::Texture&);
    Framebuffer createFramebuffer(const gfx::Texture&,
                                  const Renderbuffer<RenderbufferType::DepthComponent>&);

    template <typename Image,
              gfx::TexturePixelType format = Image::channels == 4 ? gfx::TexturePixelType::RGBA
                                                          : gfx::TexturePixelType::Alpha>
    Image readFramebuffer(const Size size, bool flip = true) {
        static_assert(Image::channels == (format == gfx::TexturePixelType::RGBA ? 4 : 1),
                      "image format mismatch");
        return { size, readFramebuffer(size, format, flip) };
    }

#if not MBGL_USE_GLES2
    template <typename Image>
    void drawPixels(const Image& image) {
        auto format = image.channels == 4 ? gfx::TexturePixelType::RGBA : gfx::TexturePixelType::Alpha;
        drawPixels(image.size, image.data.get(), format);
    }
#endif // MBGL_USE_GLES2

    void clear(optional<mbgl::Color> color,
               optional<float> depth,
               optional<int32_t> stencil);

    void setDepthMode(const gfx::DepthMode&);
    void setStencilMode(const gfx::StencilMode&);
    void setColorMode(const gfx::ColorMode&);
    void setCullFaceMode(const gfx::CullFaceMode&);

    void draw(const gfx::DrawMode&,
              std::size_t indexOffset,
              std::size_t indexLength);

    // Actually remove the objects we marked as abandoned with the above methods.
    // Only call this while the OpenGL context is exclusive to this thread.
    void performCleanup() override;

    // Drain pools and remove abandoned objects, in preparation for destroying the store.
    // Only call this while the OpenGL context is exclusive to this thread.
    void reset();

    bool empty() const {
        return pooledTextures.empty()
            && abandonedPrograms.empty()
            && abandonedShaders.empty()
            && abandonedBuffers.empty()
            && abandonedTextures.empty()
            && abandonedVertexArrays.empty()
            && abandonedFramebuffers.empty();
    }

    void setDirtyState();

    extension::Debugging* getDebuggingExtension() const {
        return debugging.get();
    }

    extension::VertexArray* getVertexArrayExtension() const {
        return vertexArray.get();
    }

    void setCleanupOnDestruction(bool cleanup) {
        cleanupOnDestruction = cleanup;
    }

private:
    bool cleanupOnDestruction = true;

    std::unique_ptr<extension::Debugging> debugging;
    std::unique_ptr<extension::VertexArray> vertexArray;
#if MBGL_HAS_BINARY_PROGRAMS
    std::unique_ptr<extension::ProgramBinary> programBinary;
#endif

public:
    State<value::ActiveTextureUnit> activeTextureUnit;
    State<value::BindFramebuffer> bindFramebuffer;
    State<value::Viewport> viewport;
    State<value::ScissorTest> scissorTest;
    std::array<State<value::BindTexture>, 2> texture;
    State<value::Program> program;
    State<value::BindVertexBuffer> vertexBuffer;

    State<value::BindVertexArray, const Context&> bindVertexArray { *this };
    VertexArrayState globalVertexArrayState { UniqueVertexArray(0, { this }) };

    State<value::PixelStorePack> pixelStorePack;
    State<value::PixelStoreUnpack> pixelStoreUnpack;

#if not MBGL_USE_GLES2
    State<value::PixelZoom> pixelZoom;
    State<value::RasterPos> rasterPos;
    State<value::PixelTransferDepth> pixelTransferDepth;
    State<value::PixelTransferStencil> pixelTransferStencil;
#endif // MBGL_USE_GLES2

    bool supportsHalfFloatTextures = false;

private:
    State<value::StencilFunc> stencilFunc;
    State<value::StencilMask> stencilMask;
    State<value::StencilTest> stencilTest;
    State<value::StencilOp> stencilOp;
    State<value::DepthRange> depthRange;
    State<value::DepthMask> depthMask;
    State<value::DepthTest> depthTest;
    State<value::DepthFunc> depthFunc;
    State<value::Blend> blend;
    State<value::BlendEquation> blendEquation;
    State<value::BlendFunc> blendFunc;
    State<value::BlendColor> blendColor;
    State<value::ColorMask> colorMask;
    State<value::ClearDepth> clearDepth;
    State<value::ClearColor> clearColor;
    State<value::ClearStencil> clearStencil;
    State<value::LineWidth> lineWidth;
    State<value::BindRenderbuffer> bindRenderbuffer;
    State<value::CullFace> cullFace;
    State<value::CullFaceSide> cullFaceSide;
    State<value::CullFaceWinding> cullFaceWinding;
#if not MBGL_USE_GLES2
    State<value::PointSize> pointSize;
#endif // MBGL_USE_GLES2

    std::unique_ptr<const gfx::VertexBufferResource> createVertexBufferResource(const void* data, std::size_t size, const gfx::BufferUsageType) override;
    void updateVertexBufferResource(const gfx::VertexBufferResource&, const void* data, std::size_t size) override;
    std::unique_ptr<const gfx::IndexBufferResource> createIndexBufferResource(const void* data, std::size_t size, const gfx::BufferUsageType) override;
    void updateIndexBufferResource(const gfx::IndexBufferResource&, const void* data, std::size_t size) override;

    std::unique_ptr<gfx::TextureResource> createTextureResource(Size, const void* data, gfx::TexturePixelType, gfx::TextureChannelDataType) override;
    void updateTextureResource(const gfx::TextureResource&, Size, const void* data, gfx::TexturePixelType, gfx::TextureChannelDataType) override;

    std::unique_ptr<gfx::DrawScopeResource> createDrawScopeResource() override;

    UniqueFramebuffer createFramebuffer();
    UniqueRenderbuffer createRenderbuffer(RenderbufferType, Size size);
    std::unique_ptr<uint8_t[]> readFramebuffer(Size, gfx::TexturePixelType, bool flip);
#if not MBGL_USE_GLES2
    void drawPixels(Size size, const void* data, gfx::TexturePixelType);
#endif // MBGL_USE_GLES2

    VertexArray createVertexArray();
    bool supportsVertexArrays() const;

    friend detail::ProgramDeleter;
    friend detail::ShaderDeleter;
    friend detail::BufferDeleter;
    friend detail::TextureDeleter;
    friend detail::VertexArrayDeleter;
    friend detail::FramebufferDeleter;
    friend detail::RenderbufferDeleter;

    std::vector<TextureID> pooledTextures;

    std::vector<ProgramID> abandonedPrograms;
    std::vector<ShaderID> abandonedShaders;
    std::vector<BufferID> abandonedBuffers;
    std::vector<TextureID> abandonedTextures;
    std::vector<VertexArrayID> abandonedVertexArrays;
    std::vector<FramebufferID> abandonedFramebuffers;
    std::vector<RenderbufferID> abandonedRenderbuffers;

public:
    // For testing
    bool disableVAOExtension = false;
};

} // namespace gl
} // namespace mbgl
