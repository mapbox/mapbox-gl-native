#pragma once

#include <mbgl/geometry/debug_font_buffer.hpp>
#include <mbgl/geometry/vao.hpp>

namespace mbgl {

class DebugRenderable {
public:
    DebugFontBuffer fontBuffer;
    VertexArrayObject array;
};

} // namespace mbgl
