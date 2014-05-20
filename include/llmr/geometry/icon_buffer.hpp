#ifndef LLMR_GEOMETRY_ICON_BUFFER
#define LLMR_GEOMETRY_ICON_BUFFER

#include "buffer.hpp"

namespace llmr {

    class IconVertexBuffer : public Buffer<
    4 + // int16 x/y coordinates per vertex (== 4 bytes)
    4   // uint16 x/y coordinates of icon in sprite (== 4 bytes)
    > {
    public:
        typedef int16_t vertex_type;

        void add(vertex_type x, vertex_type y, uint16_t tx, uint16_t ty);
    };

}

#endif
