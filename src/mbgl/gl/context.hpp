#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/gl/state.hpp>
#include <mbgl/gl/value.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <vector>

namespace mbgl {
namespace gl {

constexpr GLsizei TextureMax = 64;

class Context : private util::noncopyable {
public:
    ~Context();

    UniqueProgram createProgram() {
        return UniqueProgram { MBGL_CHECK_ERROR(glCreateProgram()), { this } };
    }

    UniqueShader createShader(GLenum type) {
        return UniqueShader { MBGL_CHECK_ERROR(glCreateShader(type)), { this } };
    }

    UniqueBuffer createBuffer() {
        GLuint id = 0;
        MBGL_CHECK_ERROR(glGenBuffers(1, &id));
        return UniqueBuffer { std::move(id), { this } };
    }

    UniqueTexture createTexture() {
        if (pooledTextures.empty()) {
            pooledTextures.resize(TextureMax);
            MBGL_CHECK_ERROR(glGenTextures(TextureMax, pooledTextures.data()));
        }

        GLuint id = pooledTextures.back();
        pooledTextures.pop_back();
        return UniqueTexture { std::move(id), { this } };
    }

    UniqueVAO createVAO() {
        GLuint id = 0;
        MBGL_CHECK_ERROR(gl::GenVertexArrays(1, &id));
        return UniqueVAO { std::move(id), { this } };
    }

    UniqueFBO createFBO() {
        GLuint id = 0;
        MBGL_CHECK_ERROR(glGenFramebuffers(1, &id));
        return UniqueFBO { std::move(id), { this } };
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
            && abandonedVAOs.empty()
            && abandonedFBOs.empty();
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
#ifndef GL_ES_VERSION_2_0
    State<value::PixelZoom> pixelZoom;
    State<value::RasterPos> rasterPos;
#endif // GL_ES_VERSION_2_0
    std::array<State<value::BindTexture>, 2> texture;
    State<value::BindBuffer<GL_ARRAY_BUFFER>> vertexBuffer;
    State<value::BindBuffer<GL_ELEMENT_ARRAY_BUFFER>> elementBuffer;
    State<value::BindVAO> vertexArrayObject;

private:
    friend detail::ProgramDeleter;
    friend detail::ShaderDeleter;
    friend detail::BufferDeleter;
    friend detail::TextureDeleter;
    friend detail::VAODeleter;
    friend detail::FBODeleter;

    std::vector<GLuint> pooledTextures;

    std::vector<GLuint> abandonedPrograms;
    std::vector<GLuint> abandonedShaders;
    std::vector<GLuint> abandonedBuffers;
    std::vector<GLuint> abandonedTextures;
    std::vector<GLuint> abandonedVAOs;
    std::vector<GLuint> abandonedFBOs;
};

} // namespace gl
} // namespace mbgl
