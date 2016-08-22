#pragma once

#include <mbgl/gl/gl.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <unique_resource.hpp>

#include <algorithm>
#include <memory>
#include <vector>

namespace mbgl {
namespace gl {

constexpr GLsizei TextureMax = 64;

class ObjectStore;

struct ProgramDeleter {
    ObjectStore* store;
    void operator()(GLuint) const;
};

struct ShaderDeleter {
    ObjectStore* store;
    void operator()(GLuint) const;
};

struct BufferDeleter {
    ObjectStore* store;
    void operator()(GLuint) const;
};

struct TextureDeleter {
    ObjectStore* store;
    void operator()(GLuint) const;
};

struct VAODeleter {
    ObjectStore* store;
    void operator()(GLuint) const;
};

struct FBODeleter {
    ObjectStore* store;
    void operator()(GLuint) const;
};

using UniqueProgram = std_experimental::unique_resource<GLuint, ProgramDeleter>;
using UniqueShader = std_experimental::unique_resource<GLuint, ShaderDeleter>;
using UniqueBuffer = std_experimental::unique_resource<GLuint, BufferDeleter>;
using UniqueTexture = std_experimental::unique_resource<GLuint, TextureDeleter>;
using UniqueVAO = std_experimental::unique_resource<GLuint, VAODeleter>;
using UniqueFBO = std_experimental::unique_resource<GLuint, FBODeleter>;

class ObjectStore : private util::noncopyable {
public:
    ~ObjectStore();

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

private:
    friend ProgramDeleter;
    friend ShaderDeleter;
    friend BufferDeleter;
    friend TextureDeleter;
    friend VAODeleter;
    friend FBODeleter;

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
