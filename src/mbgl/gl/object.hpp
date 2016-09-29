#pragma once

#include <mbgl/gl/gl.hpp>

#include <unique_resource.hpp>

namespace mbgl {
namespace gl {

class Context;

namespace detail {

struct ProgramDeleter {
    Context* context;
    void operator()(GLuint) const;
};

struct ShaderDeleter {
    Context* context;
    void operator()(GLuint) const;
};

struct BufferDeleter {
    Context* context;
    void operator()(GLuint) const;
};

struct TextureDeleter {
    Context* context;
    void operator()(GLuint) const;
};

struct VertexArrayDeleter {
    Context* context;
    void operator()(GLuint) const;
};

struct FramebufferDeleter {
    Context* context;
    void operator()(GLuint) const;
};

} // namespace detail

using UniqueProgram = std_experimental::unique_resource<GLuint, detail::ProgramDeleter>;
using UniqueShader = std_experimental::unique_resource<GLuint, detail::ShaderDeleter>;
using UniqueBuffer = std_experimental::unique_resource<GLuint, detail::BufferDeleter>;
using UniqueTexture = std_experimental::unique_resource<GLuint, detail::TextureDeleter>;
using UniqueVertexArray = std_experimental::unique_resource<GLuint, detail::VertexArrayDeleter>;
using UniqueFramebuffer = std_experimental::unique_resource<GLuint, detail::FramebufferDeleter>;

} // namespace gl
} // namespace mbgl
