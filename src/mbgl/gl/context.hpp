#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/gl/state.hpp>
#include <mbgl/gl/value.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/renderbuffer.hpp>
#include <mbgl/gl/framebuffer.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/types.hpp>
#include <mbgl/gl/draw_mode.hpp>
#include <mbgl/gl/depth_mode.hpp>
#include <mbgl/gl/stencil_mode.hpp>
#include <mbgl/gl/color_mode.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/util/noncopyable.hpp>


#include <functional>
#include <memory>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>

namespace mbgl {

class View;

namespace gl {

constexpr size_t TextureMax = 64;

class Context : private util::noncopyable {
public:
    ~Context();

    UniqueShader createShader(ShaderType type, const std::string& source);
    UniqueProgram createProgram(ShaderID vertexShader, ShaderID fragmentShader);
    void linkProgram(ProgramID);
    UniqueTexture createTexture();

    template <class Vertex, class DrawMode>
    VertexBuffer<Vertex, DrawMode> createVertexBuffer(VertexVector<Vertex, DrawMode>&& v) {
        return VertexBuffer<Vertex, DrawMode> {
            v.vertexSize(),
            createVertexBuffer(v.data(), v.byteSize())
        };
    }

    template <class DrawMode>
    IndexBuffer<DrawMode> createIndexBuffer(IndexVector<DrawMode>&& v) {
        return IndexBuffer<DrawMode> {
            createIndexBuffer(v.data(), v.byteSize())
        };
    }

    template <RenderbufferType type>
    Renderbuffer<type> createRenderbuffer(const Size size) {
        static_assert(type == RenderbufferType::RGBA || type == RenderbufferType::DepthStencil,
                      "invalid renderbuffer type");
        return { size, createRenderbuffer(type, size) };
    }

    Framebuffer createFramebuffer(const Renderbuffer<RenderbufferType::RGBA>&,
                                  const Renderbuffer<RenderbufferType::DepthStencil>&);
    Framebuffer createFramebuffer(const Renderbuffer<RenderbufferType::RGBA>&);
    Framebuffer createFramebuffer(const Texture&,
                                  const Renderbuffer<RenderbufferType::DepthStencil>&);
    Framebuffer createFramebuffer(const Texture&);

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
    Texture createTexture(const Image& image, TextureUnit unit = 0) {
        auto format = image.channels == 4 ? TextureFormat::RGBA : TextureFormat::Alpha;
        return { image.size, createTexture(image.size, image.data.get(), format, unit) };
    }

    template <typename Image>
    void updateTexture(Texture& obj, const Image& image, TextureUnit unit = 0) {
        auto format = image.channels == 4 ? TextureFormat::RGBA : TextureFormat::Alpha;
        updateTexture(obj.texture.get(), image.size, image.data.get(), format, unit);
        obj.size = image.size;
    }

    // Creates an empty texture with the specified dimensions.
    Texture createTexture(const Size size,
                          TextureFormat format = TextureFormat::RGBA,
                          TextureUnit unit = 0) {
        return { size, createTexture(size, nullptr, format, unit) };
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

    struct Drawable {
        DrawMode drawMode;
        DepthMode depthMode;
        StencilMode stencilMode;
        ColorMode colorMode;
        gl::ProgramID program;
        gl::BufferID vertexBuffer;
        gl::BufferID indexBuffer;
        const std::vector<Segment>& segments;
        std::function<void ()> bindUniforms;
        std::function<void (std::size_t)> bindAttributes;
    };

    void draw(const Drawable&);

    void setDepthMode(const DepthMode&);
    void setStencilMode(const StencilMode&);
    void setColorMode(const ColorMode&);

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

    State<value::ActiveTexture> activeTexture;
    State<value::BindFramebuffer> bindFramebuffer;
    State<value::Viewport> viewport;
    std::array<State<value::BindTexture>, 2> texture;
    State<value::BindVertexArray> vertexArrayObject;
    State<value::Program> program;

#if not MBGL_USE_GLES2
    State<value::PixelZoom> pixelZoom;
    State<value::RasterPos> rasterPos;
    State<value::PixelStorePack> pixelStorePack;
    State<value::PixelStoreUnpack> pixelStoreUnpack;
    State<value::PixelTransferDepth> pixelTransferDepth;
    State<value::PixelTransferStencil> pixelTransferStencil;
#endif // MBGL_USE_GLES2

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
    State<value::BindVertexBuffer> vertexBuffer;
    State<value::BindElementBuffer> elementBuffer;

    UniqueBuffer createVertexBuffer(const void* data, std::size_t size);
    UniqueBuffer createIndexBuffer(const void* data, std::size_t size);
    UniqueTexture createTexture(Size size, const void* data, TextureFormat, TextureUnit);
    void updateTexture(TextureID, Size size, const void* data, TextureFormat, TextureUnit);
    UniqueFramebuffer createFramebuffer();
    UniqueRenderbuffer createRenderbuffer(RenderbufferType, Size size);
    std::unique_ptr<uint8_t[]> readFramebuffer(Size, TextureFormat, bool flip);
#if not MBGL_USE_GLES2
    void drawPixels(Size size, const void* data, TextureFormat);
#endif // MBGL_USE_GLES2

    PrimitiveType operator()(const Points&);
    PrimitiveType operator()(const Lines&);
    PrimitiveType operator()(const LineStrip&);
    PrimitiveType operator()(const Triangles&);
    PrimitiveType operator()(const TriangleStrip&);

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

    using VertexArrayObjectKey = std::tuple<
        ProgramID,  // Program
        BufferID,   // Vertex buffer
        BufferID,   // Index buffer
        std::size_t // Vertex buffer offset
    >;

    struct VertexArrayObjectHash {
        std::size_t operator()(const VertexArrayObjectKey&) const;
    };

    using VertexArrayObjectMap = std::unordered_map<VertexArrayObjectKey, UniqueVertexArray, VertexArrayObjectHash>;
    VertexArrayObjectMap vaos;
};

} // namespace gl
} // namespace mbgl
