#ifndef LLMR_GEOMETRY_POINT_BUFFER
#define LLMR_GEOMETRY_POINT_BUFFER

#include "buffer.hpp"

namespace llmr {

    class PointVertexBuffer : public Buffer<
    4 // 2 coordinates per vertex (== 4 bytes)
    > {
    public:
        typedef int16_t vertex_type;

        void add(vertex_type x, vertex_type y);
    };

}

#endif
