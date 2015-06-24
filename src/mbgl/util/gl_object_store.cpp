#include <mbgl/util/gl_object_store.hpp>

#include <mbgl/util/thread.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/platform/gl.hpp>

namespace mbgl {
namespace util {

void GLObjectStore::abandonVAO(uint32_t vao) {
    assert(ThreadContext::currentlyOn(ThreadType::Map));
    abandonedVAOs.emplace_back(vao);
}

void GLObjectStore::abandonBuffer(uint32_t buffer) {
    assert(ThreadContext::currentlyOn(ThreadType::Map));
    abandonedBuffers.emplace_back(buffer);
}

void GLObjectStore::abandonTexture(uint32_t texture) {
    assert(ThreadContext::currentlyOn(ThreadType::Map));
    abandonedTextures.emplace_back(texture);
}

void GLObjectStore::performCleanup() {
    assert(ThreadContext::currentlyOn(ThreadType::Map));

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

}
}
