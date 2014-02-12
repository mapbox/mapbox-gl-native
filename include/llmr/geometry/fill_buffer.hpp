#ifndef LLMR_GEOMETRY_FILL_BUFFER
#define LLMR_GEOMETRY_FILL_BUFFER

#include "buffer.hpp"
#include <vector>
#include <cstdint>

namespace llmr {

class FillVertexBuffer : public Buffer<
    4 // bytes per coordinates (2 * unsigned short == 4 bytes)
> {
public:
    typedef int16_t vertex_type;

    void addDegenerate();
    void add(vertex_type x, vertex_type y);
};

}

#endif
