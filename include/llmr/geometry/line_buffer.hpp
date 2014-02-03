#ifndef LLMR_GEOMETRY_LINE_BUFFER
#define LLMR_GEOMETRY_LINE_BUFFER

#include <vector>
#include <cstdint>
#include <cmath>

namespace llmr {

class LineBuffer {
public:
    ~LineBuffer();

    /*
     * Scale the extrusion vector so that the normal length is this value.
     * Contains the "texture" normals (-1..1). This is distinct from the extrude
     * normals for line joins, because the x-value remains 0 for the texture
     * normal array, while the extrude normal actually moves the vertex to create
     * the acute/bevelled line join.
     */
    const int8_t extrudeScale = 63;

    /*
     * Add a vertex to this buffer
     *
     * @param {number} x vertex position
     * @param {number} y vertex position
     * @param {number} ex extrude normal
     * @param {number} ey extrude normal
     * @param {number} tx texture normal
     * @param {number} ty texture normal
     */
    void add(int16_t x, int16_t y, float ex, float ey, int8_t tx, int8_t ty, int32_t linesofar = 0);

    /*
     * Add a degenerate triangle to the buffer
     *
     * > So we need a way to get from the end of one triangle strip
     * to the beginning of the next strip without actually filling triangles
     * on the way. We can do this with "degenerate" triangles: We simply
     * repeat the last coordinate of the first triangle strip and the first
     * coordinate of the next triangle strip.
     */
    void addDegenerate();

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
    bool dirty = true;
    std::vector<int16_t> array;
    uint32_t buffer = 0;
};

}

#endif
