#pragma once

#include <mbgl/renderer/renderable.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>
#include <mbgl/geometry/vao.hpp>

namespace mbgl {

class DebugRenderable : public Renderable {
public:
    void upload(gl::ObjectStore& store) override {
        if (!uploaded) {
            fontBuffer.upload(store);
            uploaded = true;
        }
    }

    DebugFontBuffer fontBuffer;
    VertexArrayObject array;
};

} // namespace mbgl
