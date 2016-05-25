#include <mbgl/gl/gl_object_store.hpp>

#include <cassert>

namespace mbgl {
namespace gl {

void ProgramHolder::create(GLObjectStore& objectStore_) {
    if (id) return;
    objectStore = &objectStore_;
    id = MBGL_CHECK_ERROR(glCreateProgram());
}

void ProgramHolder::reset() {
    if (!id) return;
    objectStore->abandonedPrograms.push_back(id);
    id = 0;
}

void ShaderHolder::create(GLObjectStore& objectStore_) {
    if (id) return;
    objectStore = &objectStore_;
    id = MBGL_CHECK_ERROR(glCreateShader(type));
}

void ShaderHolder::reset() {
    if (!id) return;
    objectStore->abandonedShaders.push_back(id);
    id = 0;
}

void BufferHolder::create(GLObjectStore& objectStore_) {
    if (id) return;
    objectStore = &objectStore_;
    MBGL_CHECK_ERROR(glGenBuffers(1, &id));
}

void BufferHolder::reset() {
    if (!id) return;
    objectStore->abandonedBuffers.push_back(id);
    id = 0;
}

void TextureHolder::create(GLObjectStore& objectStore_) {
    if (id) return;
    objectStore = &objectStore_;
    MBGL_CHECK_ERROR(glGenTextures(1, &id));
}

void TextureHolder::reset() {
    if (!id) return;
    objectStore->abandonedTextures.push_back(id);
    id = 0;
}

void TexturePoolHolder::create(GLObjectStore& objectStore_) {
    if (bool()) return;
    objectStore = &objectStore_;
    MBGL_CHECK_ERROR(glGenTextures(TextureMax, ids.data()));
}

void TexturePoolHolder::reset() {
    if (!bool()) return;
    for (GLuint& id : ids) {
        if (id == 0) continue;
        objectStore->abandonedTextures.push_back(id);
        id = 0;
    };
}

void VAOHolder::create(GLObjectStore& objectStore_) {
    if (id) return;
    objectStore = &objectStore_;
    MBGL_CHECK_ERROR(gl::GenVertexArrays(1, &id));
}

void VAOHolder::reset() {
    if (!id) return;
    objectStore->abandonedVAOs.push_back(id);
    id = 0;
}

GLObjectStore::~GLObjectStore() {
    assert(abandonedPrograms.empty());
    assert(abandonedShaders.empty());
    assert(abandonedBuffers.empty());
    assert(abandonedTextures.empty());
    assert(abandonedVAOs.empty());
}

void GLObjectStore::performCleanup() {
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
