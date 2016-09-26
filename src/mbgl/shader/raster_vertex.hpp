#pragma once

#include <cstdint>

namespace mbgl {

class RasterVertex {
public:
    RasterVertex(int16_t x, int16_t y, int16_t tx, int16_t ty)
        : a_pos {
              x,
              y
          },
          a_texture_pos {
              tx,
              ty
          } {}

    const int16_t a_pos[2];
    const int16_t a_texture_pos[2];

    static void bind(const int8_t* offset);
};

} // namespace mbgl
