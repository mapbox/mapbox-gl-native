#pragma once

#include <mbgl/gl/features.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/gl/state.hpp>
#include <mbgl/gl/value.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/renderbuffer.hpp>
#include <mbgl/gl/framebuffer.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/gl/types.hpp>
#include <mbgl/gl/draw_mode.hpp>
#include <mbgl/gl/depth_mode.hpp>
#include <mbgl/gl/stencil_mode.hpp>
#include <mbgl/gl/color_mode.hpp>
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

class Context {
public:
    Context();
    ~Context();
    Context(const Context&) = delete;
    Context& operator=(const Context& other) = delete;

    void initializeExtensions(const std::function<gl::ProcAddress(const char*)>&);

    void enableDebugging();

    UniqueShader createShader(ShaderType type, const std::string& source);
    UniqueProgram createProgram(ShaderID vertexShader, ShaderID fragmentShader);
    UniqueProgram createProgram(BinaryProgramFormat binaryFormat, const std::string& binaryProgram);
    void verifyProgramLinkage(ProgramID);
    void linkProgram(ProgramID);
    UniqueTexture createTexture();
    VertexArray createVertexArray();

#if MBGL_HAS_BINARY_PROGRAMS
    bool supportsProgramBinaries() const;
#else
    constexpr bool supportsProgramBinaries() const { return false; }
#endif
    optional<std::pair<BinaryProgramFormat, std::string>> getBinaryProgram(ProgramID) const;

    template <class Vertex, class DrawMode>
    VertexBuffer<Vertex, DrawMode> createVertexBuffer(VertexVector<Vertex, DrawMode>&& v, const BufferUsage usage = BufferUsage::StaticDraw) {
        return VertexBuffer<Vertex, DrawMode> {
            v.vertexSize(),
            createVertexBuffer(v.data(), v.byteSize(), usage)
        };
    }

    template <class Vertex, class DrawMode>
    void updateVertexBuffer(VertexBuffer<Vertex, DrawMode>& buffer, VertexVector<Vertex, DrawMode>&& v) {
        assert(v.vertexSize() == buffer.vertexCount);
        updateVertexBuffer(buffer.buffer, v.data(), v.byteSize());
    }

    template <class DrawMode>
    IndexBuffer<DrawMode> createIndexBuffer(IndexVector<DrawMode>&& v, const BufferUsage usage = BufferUsage::StaticDraw) {
        return IndexBuffer<DrawMode> {
            v.indexSize(),
            createIndexBuffer(v.data(), v.byteSize(), usage)
        };
    }
    
    template <class DrawMode>
    void updateIndexBuffer(IndexBuffer<DrawMode>& buffer, IndexVector<DrawMode>&& v) {
        assert(v.indexSize() == buffer.indexCount);
        updateIndexBuffer(buffer.buffer, v.data(), v.byteSize());
    }

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
    Framebuffer createFramebuffer(const Texture&,
                                  const Renderbuffer<RenderbufferType::DepthStencil>&);
    Framebuffer createFramebuffer(const Texture&);
    Framebuffer createFramebuffer(const Texture&,
                                  const Renderbuffer<RenderbufferType::DepthComponent>&);

    template <typename Image,
              TextureFormat format = Image::channels == 4 ? TextureFormat::RGBA
                                                          : TextureFormat::Alpha>
    Image readFramebuffer(const Size size, bool flip = true) {
        static_assert(Image::channels == (format == TextureFormat::RGBA ? 4 : 1),
                      "image format mismatch");
        return { size, readFramebuffer(size, format, flip) };
    }

#if not MBGL_USE_GLES2
    template <typename Image>
    void drawPixels(const Image& image) {
        auto format = image.channels == 4 ? TextureFormat::RGBA : TextureFormat::Alpha;
        drawPixels(image.size, image.data.get(), format);
    }
#endif // MBGL_USE_GLES2

    // Create a texture from an image with data.
    template <typename Image>
    Texture createTexture(const Image& image,
                          TextureUnit unit = 0,
                          TextureType type = TextureType::UnsignedByte) {
        auto format = image.channels == 4 ? TextureFormat::RGBA : TextureFormat::Alpha;
        return { image.size, createTexture(image.size, image.data.get(), format, unit, type) };
    }

    template <typename Image>
    void updateTexture(Texture& obj,
                       const Image& image,
                       TextureUnit unit = 0,
                       TextureType type = TextureType::UnsignedByte) {
        auto format = image.channels == 4 ? TextureFormat::RGBA : TextureFormat::Alpha;
        updateTexture(obj.texture.get(), image.size, image.data.get(), format, unit, type);
        obj.size = image.size;
    }

    // Creates an empty texture with the specified dimensions.
    Texture createTexture(const Size size,
                          TextureFormat format = TextureFormat::RGBA,
                          TextureUnit unit = 0,
                          TextureType type = TextureType::UnsignedByte) {
        return { size, createTexture(size, nullptr, format, unit, type) };
    }

    void bindTexture(Texture&,
                     TextureUnit = 0,
                     TextureFilter = TextureFilter::Nearest,
                     TextureMipMap = TextureMipMap::No,
                     TextureWrap wrapX = TextureWrap::Clamp,
                     TextureWrap wrapY = TextureWrap::Clamp);

    void clear(optional<mbgl::Color> color,
               optional<float> depth,
               optional<int32_t> stencil);

    void setDrawMode(const Points&);
    void setDrawMode(const Lines&);
    void setDrawMode(const LineStrip&);
    void setDrawMode(const Triangles&);
    void setDrawMode(const TriangleStrip&);

    void setDepthMode(const DepthMode&);
    void setStencilMode(const StencilMode&);
    void setColorMode(const ColorMode&);

    void draw(PrimitiveType,
              std::size_t indexOffset,
              std::size_t indexLength);

    // Actually remove the objects we marked as abandoned with the above methods.
    // Only call this while the OpenGL context is exclusive to this thread.
    void performCleanup();

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
    const uint32_t maximumVertexBindingCount;
    static constexpr const uint32_t minimumRequiredVertexBindingCount = 8;
    
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
#if not MBGL_USE_GLES2
    State<value::PointSize> pointSize;
#endif // MBGL_USE_GLES2

    UniqueBuffer createVertexBuffer(const void* data, std::size_t size, const BufferUsage usage);
    void updateVertexBuffer(UniqueBuffer& buffer, const void* data, std::size_t size);
    UniqueBuffer createIndexBuffer(const void* data, std::size_t size, const BufferUsage usage);
    void updateIndexBuffer(UniqueBuffer& buffer, const void* data, std::size_t size);
    UniqueTexture createTexture(Size size, const void* data, TextureFormat, TextureUnit, TextureType);
    void updateTexture(TextureID, Size size, const void* data, TextureFormat, TextureUnit, TextureType);
    UniqueFramebuffer createFramebuffer();
    UniqueRenderbuffer createRenderbuffer(RenderbufferType, Size size);
    std::unique_ptr<uint8_t[]> readFramebuffer(Size, TextureFormat, bool flip);
#if not MBGL_USE_GLES2
    void drawPixels(Size size, const void* data, TextureFormat);
#endif // MBGL_USE_GLES2

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
    // For testing and Windows because Qt + ANGLE
    // crashes with VAO enabled.
#if defined(_WINDOWS)
    bool disableVAOExtension = true;
#else
    bool disableVAOExtension = false;
#endif
};

} // namespace gl
} // namespace mbgl
