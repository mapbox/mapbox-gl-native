#ifndef LLMR_GEOMETRY_BUFFER
#define LLMR_GEOMETRY_BUFFER

#include <cstdlib>
#include <cassert>
#include <llmr/platform/gl.hpp>
#include <llmr/util/noncopyable.hpp>

namespace llmr {

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

    // Transfers this buffer to the GPU and binds the buffer to the GL context.
    void bind(bool force = false) {
        if (buffer == 0) {
            glGenBuffers(1, &buffer);
            force = true;
        }
        glBindBuffer(bufferType, buffer);
        if (force) {
            assert("Buffer was already deleted" && array != nullptr);
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

protected:
    // increase the buffer size by at least /required/ bytes.
    inline void *addElement() {
        assert("Buffer is already bound to GPU" && buffer == 0);
        if (length < pos + itemSize) {
            while (length < pos + itemSize) length += defaultLength;
            array = realloc(array, length);
            assert("Buffer reallocation failed" && array != nullptr);
        }
        pos += itemSize;
        return static_cast<char *>(array) + (pos - itemSize);
    }

    // Get a pointer to the item at a given index.
    inline void *getElement(size_t index) {
        assert("Buffer was deleted" && array != nullptr);

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
    uint32_t buffer = 0;
};

}

#endif
