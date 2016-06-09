#pragma once

#include <mbgl/gl/gl.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <unique_resource.hpp>

#include <array>
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

namespace mbgl {
namespace gl {

constexpr GLsizei TextureMax = 64;

struct ProgramDeleter {
    void operator()(GLuint) const;
};

struct ShaderDeleter {
    void operator()(GLuint) const;
};

struct BufferDeleter {
    void operator()(GLuint) const;
};

struct TextureDeleter {
    void operator()(GLuint) const;
};

struct VAODeleter {
    void operator()(GLuint) const;
};

using ObjectPool = std::array<GLuint, TextureMax>;

struct TexturePoolDeleter {
    void operator()(ObjectPool ids) const;
};

using UniqueProgram = std_experimental::unique_resource<GLuint, ProgramDeleter>;
using UniqueShader = std_experimental::unique_resource<GLuint, ShaderDeleter>;
using UniqueBuffer = std_experimental::unique_resource<GLuint, BufferDeleter>;
using UniqueTexture = std_experimental::unique_resource<GLuint, TextureDeleter>;
using UniqueVAO = std_experimental::unique_resource<GLuint, VAODeleter>;
using UniqueTexturePool = std_experimental::unique_resource<ObjectPool, TexturePoolDeleter>;

class ObjectStore : private util::noncopyable {
public:
    static ObjectStore& get() {
        static ObjectStore store;
        return store;
    }

    ~ObjectStore();

    UniqueProgram createProgram() {
        return UniqueProgram { MBGL_CHECK_ERROR(glCreateProgram()), ProgramDeleter {} };
    }

    UniqueShader createShader(GLenum type) {
        return UniqueShader { MBGL_CHECK_ERROR(glCreateShader(type)), ShaderDeleter {} };
    }

    UniqueBuffer createBuffer() {
        GLuint id = 0;
        MBGL_CHECK_ERROR(glGenBuffers(1, &id));
        return UniqueBuffer { std::move(id), BufferDeleter {} };
    }

    UniqueTexture createTexture() {
        GLuint id = 0;
        MBGL_CHECK_ERROR(glGenTextures(1, &id));
        return UniqueTexture { std::move(id), TextureDeleter {} };
    }

    UniqueVAO createVAO() {
        GLuint id = 0;
        MBGL_CHECK_ERROR(gl::GenVertexArrays(1, &id));
        return UniqueVAO { std::move(id), VAODeleter {} };
    }

    UniqueTexturePool createTexturePool() {
        ObjectPool ids;
        MBGL_CHECK_ERROR(glGenTextures(TextureMax, ids.data()));
        assert(ids.size() == size_t(TextureMax));
        return UniqueTexturePool { std::move(ids), TexturePoolDeleter {} };
    }

    // Actually remove the objects we marked as abandoned with the above methods.
    // Only call this while the OpenGL context is exclusive to this thread.
    void performCleanup();

    bool empty() const {
        return abandonedPrograms.empty()
            && abandonedShaders.empty()
            && abandonedBuffers.empty()
            && abandonedTextures.empty()
            && abandonedVAOs.empty();
    }

private:
    ObjectStore() = default;

    friend ProgramDeleter;
    friend ShaderDeleter;
    friend BufferDeleter;
    friend TextureDeleter;
    friend VAODeleter;
    friend TexturePoolDeleter;

    std::vector<GLuint> abandonedPrograms;
    std::vector<GLuint> abandonedShaders;
    std::vector<GLuint> abandonedBuffers;
    std::vector<GLuint> abandonedTextures;
    std::vector<GLuint> abandonedVAOs;
};

} // namespace gl
} // namespace mbgl
