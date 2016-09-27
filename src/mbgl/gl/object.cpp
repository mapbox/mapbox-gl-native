#include <mbgl/gl/object.hpp>
#include <mbgl/gl/context.hpp>

#include <cassert>

namespace mbgl {
namespace gl {
namespace detail {

void ProgramDeleter::operator()(GLuint id) const {
    assert(context);
    context->abandonedPrograms.push_back(id);
}

void ShaderDeleter::operator()(GLuint id) const {
    assert(context);
    context->abandonedShaders.push_back(id);
}

void BufferDeleter::operator()(GLuint id) const {
    assert(context);
    context->abandonedBuffers.push_back(id);
}

void TextureDeleter::operator()(GLuint id) const {
    assert(context);
    if (context->pooledTextures.size() >= TextureMax) {
        context->abandonedTextures.push_back(id);
    } else {
        context->pooledTextures.push_back(id);
    }
}

void VAODeleter::operator()(GLuint id) const {
    assert(context);
    context->abandonedVAOs.push_back(id);
}

void FBODeleter::operator()(GLuint id) const {
    assert(context);
    context->abandonedFBOs.push_back(id);
}

} // namespace detail
} // namespace gl
} // namespace mbgl
