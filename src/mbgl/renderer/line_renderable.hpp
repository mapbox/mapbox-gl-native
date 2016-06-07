#pragma once

#include <mbgl/renderer/renderable.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/line_buffer.hpp>

namespace mbgl {

class LineRenderable : public Renderable {
public:
    using TriangleGroup = ElementGroup<3>;

    void upload(gl::ObjectStore& store) override {
        if (!uploaded) {
            vertexBuffer.upload(store);
            triangleElementsBuffer.upload(store);
            uploaded = true;
        }
    }

    LineVertexBuffer vertexBuffer;
    TriangleElementsBuffer triangleElementsBuffer;
    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;
};

} // namespace mbgl
