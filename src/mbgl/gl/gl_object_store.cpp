#include <mbgl/gl/gl_object_store.hpp>
#include <mbgl/util/thread_context.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/gl/gl.hpp>

#include <cassert>

namespace mbgl {
namespace gl {

void ProgramHolder::create() {
    if (id) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    id = MBGL_CHECK_ERROR(glCreateProgram());
}

void ProgramHolder::reset() {
    if (!id) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    MBGL_CHECK_ERROR(glDeleteProgram(id));
    id = 0;
}

void ShaderHolder::create() {
    if (id) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    id = MBGL_CHECK_ERROR(glCreateShader(type));
}

void ShaderHolder::reset() {
    if (!id) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    MBGL_CHECK_ERROR(glDeleteShader(id));
    id = 0;
}


void GLObjectStore::abandonVAO(GLuint vao) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    abandonedVAOs.emplace_back(vao);
}

void GLObjectStore::abandonBuffer(GLuint buffer) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    abandonedBuffers.emplace_back(buffer);
}

void GLObjectStore::abandonTexture(GLuint texture) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    abandonedTextures.emplace_back(texture);
}

void GLObjectStore::performCleanup() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    if (!abandonedVAOs.empty()) {
        MBGL_CHECK_ERROR(VertexArrayObject::Delete(static_cast<GLsizei>(abandonedVAOs.size()),
                                                   abandonedVAOs.data()));
        abandonedVAOs.clear();
    }

    if (!abandonedTextures.empty()) {
        MBGL_CHECK_ERROR(glDeleteTextures(static_cast<GLsizei>(abandonedTextures.size()),
                                          abandonedTextures.data()));
        abandonedTextures.clear();
    }

    if (!abandonedBuffers.empty()) {
        MBGL_CHECK_ERROR(glDeleteBuffers(static_cast<GLsizei>(abandonedBuffers.size()),
                                         abandonedBuffers.data()));
        abandonedBuffers.clear();
    }
}

} // namespace gl
} // namespace mbgl
