#include <mbgl/gl/object.hpp>
#include <mbgl/gl/context.hpp>

#include <cassert>

namespace mbgl {
namespace gl {
namespace detail {

void ProgramDeleter::operator()(ProgramID id) const {
    assert(context);
    context->abandonedPrograms.push_back(id);
}

void ShaderDeleter::operator()(ShaderID id) const {
    assert(context);
    context->abandonedShaders.push_back(id);
}

void BufferDeleter::operator()(BufferID id) const {
    assert(context);
    context->abandonedBuffers.push_back(id);
}

void TextureDeleter::operator()(TextureID id) const {
    assert(context);
    if (context->pooledTextures.size() >= TextureMax) {
        context->abandonedTextures.push_back(id);
    } else {
        context->pooledTextures.push_back(id);
    }
}

void VertexArrayDeleter::operator()(VertexArrayID id) const {
    assert(context);
    if (id != 0) {
        context->abandonedVertexArrays.push_back(id);
    }
}

void FramebufferDeleter::operator()(FramebufferID id) const {
    assert(context);
    context->abandonedFramebuffers.push_back(id);
}

void RenderbufferDeleter::operator()(RenderbufferID id) const {
    assert(context);
    context->abandonedRenderbuffers.push_back(id);
}

} // namespace detail
} // namespace gl
} // namespace mbgl
