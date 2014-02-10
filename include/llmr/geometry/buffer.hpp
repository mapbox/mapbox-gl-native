#ifndef LLMR_GEOMETRY_BUFFER
#define LLMR_GEOMETRY_BUFFER

#include <cstdlib>
#include <cassert>
#include <llmr/platform/gl.hpp>

namespace llmr {

template <
    size_t itemSize,
    int bufferType = GL_ARRAY_BUFFER,
    size_t defaultLength = 8192
>
class Buffer {
public:
    ~Buffer() {
        if (array) {
            free(array);
            array = nullptr;
        }
        if (buffer != 0) {
            glDeleteBuffers(1, &buffer);
            buffer = 0;
        }
    }

    inline size_t index() const {
        return pos / itemSize;
    }

    void bind() {
        if (buffer == 0) {
            glGenBuffers(1, &buffer);
            glBindBuffer(bufferType, buffer);
            glBufferData(bufferType, pos, array, GL_STATIC_DRAW);
            free(array);
            array = nullptr;
        } else {
            glBindBuffer(bufferType, buffer);
        }
    }

protected:
    // increase the buffer size by at least /required/ bytes.
    void *addElement() {
        assert("Buffer is already bound to GPU" && buffer == 0);
        if (length < pos + itemSize) {
            while (length < pos + itemSize) length += defaultLength;
            array = realloc(array, length);
            assert("Buffer reallocation failed" && array != nullptr);
        }
        pos += itemSize;
        return static_cast<char *>(array) + (pos - itemSize);
    }

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
