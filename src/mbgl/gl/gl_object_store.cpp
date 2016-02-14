#include <mbgl/gl/gl_object_store.hpp>
#include <mbgl/util/thread_context.hpp>

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

void BufferHolder::create() {
    if (id) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    MBGL_CHECK_ERROR(glGenBuffers(1, &id));
}

void BufferHolder::reset() {
    if (!id) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    MBGL_CHECK_ERROR(glDeleteBuffers(1, &id));
    id = 0;
}

void TextureHolder::create() {
    if (id) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    MBGL_CHECK_ERROR(glGenTextures(1, &id));
}

void TextureHolder::reset() {
    if (!id) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    MBGL_CHECK_ERROR(glDeleteTextures(1, &id));
    id = 0;
}

void TexturePoolHolder::create() {
    if (bool()) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    MBGL_CHECK_ERROR(glGenTextures(TextureMax, ids.data()));
}

void TexturePoolHolder::reset() {
    if (!bool()) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    MBGL_CHECK_ERROR(glDeleteTextures(TextureMax, ids.data()));
    ids.fill(0);
}

void VAOHolder::create() {
    if (id) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    MBGL_CHECK_ERROR(gl::GenVertexArrays(1, &id));
}

void VAOHolder::reset() {
    if (!id) return;
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    MBGL_CHECK_ERROR(gl::DeleteVertexArrays(1, &id));
    id = 0;
}

void GLObjectStore::abandon(BufferHolder&& buffer) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    abandonedBuffers.push_back(std::move(buffer));
}

void GLObjectStore::abandon(TextureHolder&& texture) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    abandonedTextures.push_back(std::move(texture));
}

void GLObjectStore::abandon(TexturePoolHolder&& texture) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    abandonedTexturePools.push_back(std::move(texture));
}

void GLObjectStore::abandon(VAOHolder&& vao) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    abandonedVAOs.push_back(std::move(vao));
}

void GLObjectStore::performCleanup() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    abandonedBuffers.clear();
    abandonedTextures.clear();
    abandonedTexturePools.clear();
    abandonedVAOs.clear();
}

} // namespace gl
} // namespace mbgl
