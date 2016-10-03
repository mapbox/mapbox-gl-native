#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/gl/state.hpp>
#include <mbgl/gl/value.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <vector>
#include <array>

namespace mbgl {
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
    UniqueFramebuffer createFramebuffer();

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

    // Create a texture from an image with data.
    template <typename Image>
    Texture createTexture(const Image& image, TextureUnit unit = 0) {
        return { {{ image.width, image.height }},
                 createTexture(image.width, image.height, image.data.get(), unit) };
    }

    // Creates an empty texture with the specified dimensions.
    Texture createTexture(const std::array<uint16_t, 2>& size, TextureUnit unit = 0) {
        return { size, createTexture(size[0], size[1], nullptr, unit) };
    }

    void bindTexture(Texture&,
                     TextureUnit = 0,
                     TextureFilter = TextureFilter::Nearest,
                     TextureMipMap = TextureMipMap::No);

    template <class Shader, class Vertex>
    void bindAttributes(const Shader& shader, const VertexBuffer<Vertex>&, const int8_t* offset) {
        static_assert(std::is_same<typename Shader::VertexType, Vertex>::value, "vertex type mismatch");
        for (const auto& binding : AttributeBindings<Shader, Vertex>()(shader)) {
            bindAttribute(binding, sizeof(Vertex), offset);
        }
    }

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

    void resetState();

    void setDirtyState();

    State<value::StencilFunc> stencilFunc;
    State<value::StencilMask> stencilMask;
    State<value::StencilTest> stencilTest;
    State<value::StencilOp> stencilOp;
    State<value::DepthRange> depthRange;
    State<value::DepthMask> depthMask;
    State<value::DepthTest> depthTest;
    State<value::DepthFunc> depthFunc;
    State<value::Blend> blend;
    State<value::BlendFunc> blendFunc;
    State<value::BlendColor> blendColor;
    State<value::ColorMask> colorMask;
    State<value::ClearDepth> clearDepth;
    State<value::ClearColor> clearColor;
    State<value::ClearStencil> clearStencil;
    State<value::Program> program;
    State<value::LineWidth> lineWidth;
    State<value::ActiveTexture> activeTexture;
    State<value::BindFramebuffer> bindFramebuffer;
    State<value::Viewport> viewport;
#if not MBGL_USE_GLES2
    State<value::PixelZoom> pixelZoom;
    State<value::RasterPos> rasterPos;
#endif // MBGL_USE_GLES2
    std::array<State<value::BindTexture>, 2> texture;
    State<value::BindVertexBuffer> vertexBuffer;
    State<value::BindElementBuffer> elementBuffer;
    State<value::BindVertexArray> vertexArrayObject;

private:
    UniqueBuffer createVertexBuffer(const void* data, std::size_t size);
    UniqueBuffer createIndexBuffer(const void* data, std::size_t size);
    UniqueTexture createTexture(uint16_t width, uint16_t height, const void* data, TextureUnit);
    void bindAttribute(const AttributeBinding&, std::size_t stride, const int8_t* offset);

    friend detail::ProgramDeleter;
    friend detail::ShaderDeleter;
    friend detail::BufferDeleter;
    friend detail::TextureDeleter;
    friend detail::VertexArrayDeleter;
    friend detail::FramebufferDeleter;

    std::vector<TextureID> pooledTextures;

    std::vector<ProgramID> abandonedPrograms;
    std::vector<ShaderID> abandonedShaders;
    std::vector<BufferID> abandonedBuffers;
    std::vector<TextureID> abandonedTextures;
    std::vector<VertexArrayID> abandonedVertexArrays;
    std::vector<FramebufferID> abandonedFramebuffers;
};

} // namespace gl
} // namespace mbgl
