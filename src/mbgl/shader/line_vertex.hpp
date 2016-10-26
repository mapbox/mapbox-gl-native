#pragma once

#include <mbgl/gl/attribute.hpp>

#include <array>
#include <cstdint>
#include <cmath>

namespace mbgl {

class LineVertex {
public:
    /*
     * @param {number} x vertex position
     * @param {number} y vertex position
     * @param {number} ex extrude normal
     * @param {number} ey extrude normal
     * @param {number} tx texture normal
     * @param {number} ty texture normal
     * @param {number} dir direction of the line cap (-1/0/1)
     */
    LineVertex(int16_t x, int16_t y, float ex, float ey, bool tx, bool ty, int8_t dir, int32_t linesofar = 0)
        : a_pos {
              static_cast<int16_t>((x * 2) | tx),
              static_cast<int16_t>((y * 2) | ty)
          },
          a_data {
              // add 128 to store an byte in an unsigned byte
              static_cast<uint8_t>(::round(extrudeScale * ex) + 128),
              static_cast<uint8_t>(::round(extrudeScale * ey) + 128),

              // Encode the -1/0/1 direction value into the first two bits of .z of a_data.
              // Combine it with the lower 6 bits of `linesofar` (shifted by 2 bites to make
              // room for the direction value). The upper 8 bits of `linesofar` are placed in
              // the `w` component. `linesofar` is scaled down by `LINE_DISTANCE_SCALE` so that
              // we can store longer distances while sacrificing precision.

              // Encode the -1/0/1 direction value into .zw coordinates of a_data, which is normally covered
              // by linesofar, so we need to merge them.
              // The z component's first bit, as well as the sign bit is reserved for the direction,
              // so we need to shift the linesofar.
              static_cast<uint8_t>(((dir == 0 ? 0 : (dir < 0 ? -1 : 1 )) + 1) | ((linesofar & 0x3F) << 2)),
              static_cast<uint8_t>(linesofar >> 6)
          } {}

    const int16_t a_pos[2];
    const uint8_t a_data[4];

    /*
     * Scale the extrusion vector so that the normal length is this value.
     * Contains the "texture" normals (-1..1). This is distinct from the extrude
     * normals for line joins, because the x-value remains 0 for the texture
     * normal array, while the extrude normal actually moves the vertex to create
     * the acute/bevelled line join.
     */
    static const int8_t extrudeScale = 63;
};

namespace gl {

template <class Shader>
struct AttributeBindings<Shader, LineVertex> {
    std::array<AttributeBinding, 2> operator()(const Shader& shader) {
        return {{
            MBGL_MAKE_ATTRIBUTE_BINDING(LineVertex, shader, a_pos),
            MBGL_MAKE_ATTRIBUTE_BINDING(LineVertex, shader, a_data)
        }};
    };
};

} // namespace gl
} // namespace mbgl
