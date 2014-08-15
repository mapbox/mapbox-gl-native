#ifndef MBGL_GEOMETRY_BUFFER
#define MBGL_GEOMETRY_BUFFER

#include <mbgl/platform/gl.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <cstdlib>
#include <cassert>
#include <stdexcept>

namespace mbgl {

template <
    size_t item_size,
    int bufferType = GL_ARRAY_BUFFER,
    size_t defaultLength = 8192,
    bool retainAfterUpload = false
>
class Buffer : private util::noncopyable {
public:
    ~Buffer() {
        cleanup();
        if (buffer != 0) {
            glDeleteBuffers(1, &buffer);
            buffer = 0;
        }
    }

    // Returns the number of elements in this buffer. This is not the number of
    // bytes, but rather the number of coordinates with associated information.
    inline size_t index() const {
        return pos / itemSize;
    }

    inline bool empty() const {
        return pos == 0;
    }

    // Transfers this buffer to the GPU and binds the buffer to the GL context.
    void bind(bool force = false) {
        if (buffer == 0) {
            glGenBuffers(1, &buffer);
            force = true;
        }
        glBindBuffer(bufferType, buffer);
        if (force) {
            if (array == nullptr) {
                throw std::runtime_error("Buffer was already deleted or doesn't contain elements");
            }

            glBufferData(bufferType, pos, array, GL_STATIC_DRAW);
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

    inline GLuint getID() const {
        return buffer;
    }

protected:
    // increase the buffer size by at least /required/ bytes.
    inline void *addElement() {
        if (buffer != 0) {
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
        return static_cast<char *>(array) + (pos - itemSize);
    }

    // Get a pointer to the item at a given index.
    inline void *getElement(size_t index) {
        if (array == nullptr) {
            throw std::runtime_error("Buffer was already deleted or doesn't contain elements");
        }

        if (index * itemSize >= pos) {
            throw new std::runtime_error("Can't get element after array bounds");
        } else {
            return static_cast<char *>(array) + (index * itemSize);
        }
    }

public:
    static const size_t itemSize = item_size;

private:
    // CPU buffer
    void *array = nullptr;

    // Byte position where we are writing.
    size_t pos = 0;

    // Number of bytes that are valid in this buffer.
    size_t length = 0;

    // GL buffer ID
    GLuint buffer = 0;
};

}

#endif
