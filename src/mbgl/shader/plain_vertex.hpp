#pragma once

#include <cstdint>

namespace mbgl {

class PlainVertex {
public:
    PlainVertex(int16_t x, int16_t y)
        : a_pos { x, y } {}

    const int16_t a_pos[2];

    static void bind(const int8_t* offset);
};

} // namespace mbgl
