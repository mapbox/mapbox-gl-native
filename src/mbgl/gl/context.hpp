#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/gl/state.hpp>
#include <mbgl/gl/value.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/renderbuffer.hpp>
#include <mbgl/gl/framebuffer.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/drawable.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <vector>
#include <array>

namespace mbgl {

class View;

namespace gl {

constexpr size_t TextureMax = 64;

class Context : private util::noncopyable {
public:
    ~Context();

    UniqueProgram createProgram();
    UniqueShader createVertexShader();
    UniqueShader createFragmentShader();
    UniqueTexture createTexture();
    UniqueVertexArray createVertexArray();

    template <class V>
    VertexBuffer<V> createVertexBuffer(std::vector<V>&& v) {
        return VertexBuffer<V> {
            v.size(),
            createVertexBuffer(v.data(), v.size() * sizeof(V))
        };
    }

    template <class P>
    IndexBuffer<P> createIndexBuffer(std::vector<P>&& v) {
        return IndexBuffer<P> {
            createIndexBuffer(v.data(), v.size() * sizeof(P))
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

    // Create a texture from an image with data.
    template <typename Image>
    Texture createTexture(const Image& image, TextureUnit unit = 0) {
        return { image.size, createTexture(image.size, image.data.get(), unit) };
    }

    // Creates an empty texture with the specified dimensions.
    Texture createTexture(const Size size, TextureUnit unit = 0) {
        return { size, createTexture(size, nullptr, unit) };
    }

    void bindTexture(Texture&,
                     TextureUnit = 0,
                     TextureFilter = TextureFilter::Nearest,
                     TextureMipMap = TextureMipMap::No);

    void clear(optional<mbgl::Color> color,
               optional<float> depth,
               optional<int32_t> stencil);

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
    UniqueTexture createTexture(Size size, const void* data, TextureUnit);
    UniqueFramebuffer createFramebuffer();
    UniqueRenderbuffer createRenderbuffer(RenderbufferType, Size size);

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
};

} // namespace gl
} // namespace mbgl
