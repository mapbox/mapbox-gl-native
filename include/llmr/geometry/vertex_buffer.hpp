#ifndef LLMR_GEOMETRY_VERTEX_BUFFER
#define LLMR_GEOMETRY_VERTEX_BUFFER

#include <vector>
#include <cstdint>
#include <cmath>

namespace llmr {

class VertexBuffer {
public:
    VertexBuffer(std::initializer_list<int16_t> init);
    ~VertexBuffer();

    /*
     * Returns the number of elements in this buffer. This is not the number of
     * bytes, but rather the number of coordinates with associated information.
     */
    uint32_t length() const;

    /*
     * Transfers this buffer to the GPU and binds the buffer to the GL context.
     */
    void bind();

private:
    const std::vector<int16_t> array;
    uint32_t buffer = 0;
};

}

#endif
