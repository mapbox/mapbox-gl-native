#pragma once

#include <mbgl/gl/types.hpp>

#include <unique_resource.hpp>

namespace mbgl {
namespace gl {

class Context;

namespace detail {

struct ProgramDeleter {
    Context* context;
    void operator()(ProgramID) const;
};

struct ShaderDeleter {
    Context* context;
    void operator()(ShaderID) const;
};

struct BufferDeleter {
    Context* context;
    void operator()(BufferID) const;
};

struct TextureDeleter {
    Context* context;
    void operator()(TextureID) const;
};

struct VertexArrayDeleter {
    Context* context;
    void operator()(VertexArrayID) const;
};

struct FramebufferDeleter {
    Context* context;
    void operator()(FramebufferID) const;
};

struct RenderbufferDeleter {
    Context* context;
    void operator()(RenderbufferID) const;
};

} // namespace detail

using UniqueProgram = std_experimental::unique_resource<ProgramID, detail::ProgramDeleter>;
using UniqueShader = std_experimental::unique_resource<ShaderID, detail::ShaderDeleter>;
using UniqueBuffer = std_experimental::unique_resource<BufferID, detail::BufferDeleter>;
using UniqueTexture = std_experimental::unique_resource<TextureID, detail::TextureDeleter>;
using UniqueVertexArray = std_experimental::unique_resource<VertexArrayID, detail::VertexArrayDeleter>;
using UniqueFramebuffer = std_experimental::unique_resource<FramebufferID, detail::FramebufferDeleter>;
using UniqueRenderbuffer = std_experimental::unique_resource<RenderbufferID, detail::RenderbufferDeleter>;

} // namespace gl
} // namespace mbgl
