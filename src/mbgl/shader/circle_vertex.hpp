#pragma once

#include <cstdint>

namespace mbgl {

class CircleVertex {
public:
    /*
     * @param {number} x vertex position
     * @param {number} y vertex position
     * @param {number} ex extrude normal
     * @param {number} ey extrude normal
     */
    CircleVertex(int16_t x, int16_t y, float ex, float ey)
        : a_pos {
            static_cast<int16_t>((x * 2) + ((ex + 1) / 2)),
            static_cast<int16_t>((y * 2) + ((ey + 1) / 2))
        } {}

    const int16_t a_pos[2];

    static void bind(const int8_t* offset);
};

} // namespace mbgl
