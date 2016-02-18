#ifndef MBGL_MAP_UTIL_GL_OBJECT_STORE
#define MBGL_MAP_UTIL_GL_OBJECT_STORE

#include <mbgl/gl/gl.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <array>
#include <algorithm>
#include <memory>
#include <vector>

namespace mbgl {
namespace gl {

class GLObjectStore : private util::noncopyable {
public:
    ~GLObjectStore();

    // Actually remove the objects we marked as abandoned with the above methods.
    // Only call this while the OpenGL context is exclusive to this thread.
    void performCleanup();

private:
    friend class ProgramHolder;
    friend class ShaderHolder;
    friend class BufferHolder;
    friend class TextureHolder;
    friend class TexturePoolHolder;
    friend class VAOHolder;

    std::vector<GLuint> abandonedPrograms;
    std::vector<GLuint> abandonedShaders;
    std::vector<GLuint> abandonedBuffers;
    std::vector<GLuint> abandonedTextures;
    std::vector<GLuint> abandonedVAOs;
};

class GLHolder : private util::noncopyable {
public:
    GLHolder() {}

    GLHolder(GLHolder&& o) noexcept : id(o.id) { o.id = 0; }
    GLHolder& operator=(GLHolder&& o) noexcept { id = o.id; o.id = 0; return *this; }

    explicit operator bool() const { return id; }
    GLuint getID() const { return id; }

protected:
    GLuint id = 0;
    GLObjectStore* objectStore = nullptr;
};

class ProgramHolder : public GLHolder {
public:
    ProgramHolder() = default;
    ~ProgramHolder() { reset(); }

    ProgramHolder(ProgramHolder&& o) noexcept : GLHolder(std::move(o)) {}
    ProgramHolder& operator=(ProgramHolder&& o) noexcept { GLHolder::operator=(std::move(o)); return *this; }

    void create(GLObjectStore&);
    void reset();
};

class ShaderHolder : public GLHolder {
public:
    ShaderHolder(GLenum type_) : type(type_) {}
    ~ShaderHolder() { reset(); }

    ShaderHolder(ShaderHolder&& o) noexcept : GLHolder(std::move(o)), type(o.type) {}
    ShaderHolder& operator=(ShaderHolder&& o) noexcept { GLHolder::operator=(std::move(o)); type = o.type; return *this; }

    void create(GLObjectStore&);
    void reset();

private:
    GLenum type = 0;
};

class BufferHolder : public GLHolder {
public:
    BufferHolder() = default;
    ~BufferHolder() { reset(); }

    BufferHolder(BufferHolder&& o) noexcept : GLHolder(std::move(o)) {}
    BufferHolder& operator=(BufferHolder&& o) noexcept { GLHolder::operator=(std::move(o)); return *this; }

    void create(GLObjectStore&);
    void reset();
};

class TextureHolder : public GLHolder {
public:
    TextureHolder() = default;
    ~TextureHolder() { reset(); }

    TextureHolder(TextureHolder&& o) noexcept : GLHolder(std::move(o)) {}
    TextureHolder& operator=(TextureHolder&& o) noexcept { GLHolder::operator=(std::move(o)); return *this; }

    void create(GLObjectStore&);
    void reset();
};

class TexturePoolHolder : private util::noncopyable {
public:
    static const GLsizei TextureMax = 64;

    TexturePoolHolder() { ids.fill(0); }
    ~TexturePoolHolder() { reset(); }

    TexturePoolHolder(TexturePoolHolder&& o) noexcept : ids(std::move(o.ids)) {}
    TexturePoolHolder& operator=(TexturePoolHolder&& o) noexcept { ids = std::move(o.ids); return *this; }

    explicit operator bool() { return std::none_of(ids.begin(), ids.end(), [](int id) { return id == 0; }); }
    const std::array<GLuint, TextureMax>& getIDs() const { return ids; }
    const GLuint& operator[](size_t pos) { return ids[pos]; }

    void create(GLObjectStore&);
    void reset();

private:
    std::array<GLuint, TextureMax> ids;
    GLObjectStore* objectStore = nullptr;
};

class VAOHolder : public GLHolder {
public:
    VAOHolder() = default;
    ~VAOHolder() { reset(); }

    VAOHolder(VAOHolder&& o) noexcept : GLHolder(std::move(o)) {}
    VAOHolder& operator=(VAOHolder&& o) noexcept { GLHolder::operator=(std::move(o)); return *this; }

    void create(GLObjectStore&);
    void reset();
};

} // namespace gl
} // namespace mbgl

#endif
