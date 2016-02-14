#ifndef MBGL_GEOMETRY_BUFFER
#define MBGL_GEOMETRY_BUFFER

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/gl_object_store.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/thread_context.hpp>

#include <memory>
#include <cstdlib>
#include <cassert>
#include <stdexcept>

namespace mbgl {

template <
    GLsizei item_size,
    GLenum bufferType = GL_ARRAY_BUFFER,
    GLsizei defaultLength = 8192,
    bool retainAfterUpload = false
>
class Buffer : private util::noncopyable {
public:
    ~Buffer() {
        cleanup();
        if (buffer) {
            util::ThreadContext::getGLObjectStore()->abandon(std::move(buffer));
        }
    }

    // Returns the number of elements in this buffer. This is not the number of
    // bytes, but rather the number of coordinates with associated information.
    inline GLsizei index() const {
        return static_cast<GLsizei>(pos / itemSize);
    }

    inline bool empty() const {
        return pos == 0;
    }

    // Transfers this buffer to the GPU and binds the buffer to the GL context.
    void bind() {
        if (buffer) {
            MBGL_CHECK_ERROR(glBindBuffer(bufferType, getID()));
        } else {
            buffer.create();
            MBGL_CHECK_ERROR(glBindBuffer(bufferType, getID()));
            if (array == nullptr) {
                Log::Debug(Event::OpenGL, "Buffer doesn't contain elements");
                pos = 0;
            }
            MBGL_CHECK_ERROR(glBufferData(bufferType, pos, array, GL_STATIC_DRAW));
            if (!retainAfterUpload) {
                cleanup();
            }
        }
    }

    void cleanup() {
        if (array) {
            free(array);
            array = nullptr;
        }
    }

    GLuint getID() const {
        return buffer.getID();
    }

    // Uploads the buffer to the GPU to be available when we need it.
    inline void upload() {
        if (!buffer) {
            bind();
        }
    }

protected:
    // increase the buffer size by at least /required/ bytes.
    inline void *addElement() {
        if (buffer) {
            throw std::runtime_error("Can't add elements after buffer was bound to GPU");
        }
        if (length < pos + itemSize) {
            while (length < pos + itemSize) length += defaultLength;
            array = realloc(array, length);
            if (array == nullptr) {
                throw std::runtime_error("Buffer reallocation failed");
            }
        }
        pos += itemSize;
        return reinterpret_cast<char *>(array) + (pos - itemSize);
    }

    // Get a pointer to the item at a given index.
    inline void *getElement(size_t i) {
        if (array == nullptr) {
            throw std::runtime_error("Buffer was already deleted or doesn't contain elements");
        }

        if (i * itemSize >= pos) {
            throw std::runtime_error("Can't get element after array bounds");
        } else {
            return reinterpret_cast<char *>(array) + (i * itemSize);
        }
    }

public:
    static const size_t itemSize = item_size;

private:
    // CPU buffer
    GLvoid *array = nullptr;

    // Byte position where we are writing.
    GLsizeiptr pos = 0;

    // Number of bytes that are valid in this buffer.
    size_t length = 0;

    // GL buffer object handle.
    gl::BufferHolder buffer;
};

} // namespace mbgl

#endif
