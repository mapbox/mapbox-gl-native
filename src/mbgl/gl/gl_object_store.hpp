#ifndef MBGL_MAP_UTIL_GL_OBJECT_STORE
#define MBGL_MAP_UTIL_GL_OBJECT_STORE

#include <mbgl/gl/gl.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <vector>

namespace mbgl {
namespace gl {

class GLHolder : private util::noncopyable {
public:
    GLHolder() {}

    GLHolder(GLHolder&& o) noexcept : id(o.id) { o.id = 0; }
    GLHolder& operator=(GLHolder&& o) noexcept { id = o.id; o.id = 0; return *this; }

    explicit operator bool() const { return id; }
    GLuint getID() const { return id; }

protected:
    GLuint id = 0;
};

class ProgramHolder : public GLHolder {
public:
    ProgramHolder() = default;
    ~ProgramHolder() { reset(); }

    ProgramHolder(ProgramHolder&& o) noexcept : GLHolder(std::move(o)) {}
    ProgramHolder& operator=(ProgramHolder&& o) noexcept { GLHolder::operator=(std::move(o)); return *this; }

    void create();
    void reset();
};

class ShaderHolder : public GLHolder {
public:
    ShaderHolder(GLenum type_) : type(type_) {}
    ~ShaderHolder() { reset(); }

    ShaderHolder(ShaderHolder&& o) noexcept : GLHolder(std::move(o)), type(o.type) {}
    ShaderHolder& operator=(ShaderHolder&& o) noexcept { GLHolder::operator=(std::move(o)); type = o.type; return *this; }

    void create();
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

    void create();
    void reset();
};

class GLObjectStore : private util::noncopyable {
public:
    // Mark OpenGL objects for deletion
    void abandonVAO(GLuint vao);
    void abandon(BufferHolder&&);
    void abandonTexture(GLuint texture);

    // Actually remove the objects we marked as abandoned with the above methods.
    // Only call this while the OpenGL context is exclusive to this thread.
    void performCleanup();

private:
    // We split the holder objects in separate containers because each
    // GLHolder-derived object can vary in size.
    std::vector<GLuint> abandonedVAOs;
    std::vector<BufferHolder> abandonedBuffers;
    std::vector<GLuint> abandonedTextures;
};

} // namespace gl
} // namespace mbgl

#endif
