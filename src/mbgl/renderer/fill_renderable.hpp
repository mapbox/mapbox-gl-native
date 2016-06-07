#pragma once

#include <mbgl/renderer/renderable.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/fill_buffer.hpp>

namespace mbgl {

class FillRenderable : public Renderable {
public:
    using TriangleGroup = ElementGroup<2>;
    using LineGroup = ElementGroup<2>;

    void upload(gl::ObjectStore& store) override {
        if (!uploaded) {
            vertexBuffer.upload(store);
            triangleElementsBuffer.upload(store);
            lineElementsBuffer.upload(store);
            uploaded = true;
        }
    }

    FillVertexBuffer vertexBuffer;
    TriangleElementsBuffer triangleElementsBuffer;
    LineElementsBuffer lineElementsBuffer;
    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;
    std::vector<std::unique_ptr<LineGroup>> lineGroups;
};

} // namespace mbgl
