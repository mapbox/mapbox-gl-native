#include <mbgl/gl/object_store.hpp>

#include <cassert>

namespace mbgl {
namespace gl {

void ProgramDeleter::operator()(GLuint id) const {
    assert(store);
    store->abandonedPrograms.push_back(id);
}

void ShaderDeleter::operator()(GLuint id) const {
    assert(store);
    store->abandonedShaders.push_back(id);
}

void BufferDeleter::operator()(GLuint id) const {
    assert(store);
    store->abandonedBuffers.push_back(id);
}

void TextureDeleter::operator()(GLuint id) const {
    assert(store);
    store->abandonedTextures.push_back(id);
}

void VAODeleter::operator()(GLuint id) const {
    assert(store);
    store->abandonedVAOs.push_back(id);
}

void TexturePoolDeleter::operator()(ObjectPool ids) const {
    assert(store);
    for (GLuint& id : ids) {
        if (id) {
            store->abandonedTextures.push_back(id);
            id = 0;
        };
    }
}

ObjectStore::~ObjectStore() {
    assert(abandonedPrograms.empty());
    assert(abandonedShaders.empty());
    assert(abandonedBuffers.empty());
    assert(abandonedTextures.empty());
    assert(abandonedVAOs.empty());
}

void ObjectStore::performCleanup() {
    for (GLuint id : abandonedPrograms) {
        MBGL_CHECK_ERROR(glDeleteProgram(id));
    }
    abandonedPrograms.clear();

    for (GLuint id : abandonedShaders) {
        MBGL_CHECK_ERROR(glDeleteShader(id));
    }
    abandonedShaders.clear();

    if (!abandonedBuffers.empty()) {
        MBGL_CHECK_ERROR(glDeleteBuffers(int(abandonedBuffers.size()), abandonedBuffers.data()));
        abandonedBuffers.clear();
    }

    if (!abandonedTextures.empty()) {
        MBGL_CHECK_ERROR(glDeleteTextures(int(abandonedTextures.size()), abandonedTextures.data()));
        abandonedTextures.clear();
    }

    if (!abandonedVAOs.empty()) {
        MBGL_CHECK_ERROR(gl::DeleteVertexArrays(int(abandonedVAOs.size()), abandonedVAOs.data()));
        abandonedVAOs.clear();
    }
}

} // namespace gl
} // namespace mbgl
