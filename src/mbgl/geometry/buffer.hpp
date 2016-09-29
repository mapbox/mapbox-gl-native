#pragma once

#include <mbgl/gl/context.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>

#include <memory>
#include <cstdlib>
#include <cassert>
#include <stdexcept>

namespace mbgl {

template <size_t item_size,
          gl::BufferType target = gl::BufferType::Vertex,
          size_t defaultLength = 8192,
          bool retainAfterUpload = false>
class Buffer : private util::noncopyable {
    static_assert(target == gl::BufferType::Vertex || target == gl::BufferType::Element,
                  "target must be one of gl::BufferType::Vertex or gl::BufferType::Element");

public:
    ~Buffer() {
        cleanup();
    }

    // Returns the number of elements in this buffer. This is not the number of
    // bytes, but rather the number of coordinates with associated information.
    size_t index() const {
        return static_cast<size_t>(pos / itemSize);
    }

    bool empty() const {
        return pos == 0;
    }

    // Transfers this buffer to the GPU and binds the buffer to the GL context.
    void bind(gl::Context& context) {
        const bool initialized { buffer };
        if (!initialized) {
            buffer = context.createBuffer();
        }

        if (target == gl::BufferType::Vertex) {
            context.vertexBuffer = *buffer;
        } else {
            context.elementBuffer = *buffer;
        }

        if (!initialized) {
            if (array == nullptr) {
                Log::Debug(Event::OpenGL, "Buffer doesn't contain elements");
                pos = 0;
            }
            context.uploadBuffer(target, pos, array);
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

    gl::BufferID getID() const {
        return buffer ? *buffer : 0;
    }

    // Uploads the buffer to the GPU to be available when we need it.
    void upload(gl::Context& context) {
        if (!buffer) {
            bind(context);
        }
    }

protected:
    // increase the buffer size by at least /required/ bytes.
    void *addElement() {
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

public:
    static const size_t itemSize = item_size;

private:
    // CPU buffer
    void* array = nullptr;

    // Byte position where we are writing.
    size_t pos = 0;

    // Number of bytes that are valid in this buffer.
    size_t length = 0;

    // GL buffer object handle.
    mbgl::optional<gl::UniqueBuffer> buffer;
};

} // namespace mbgl
