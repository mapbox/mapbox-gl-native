#pragma once

#include <mbgl/renderer/renderable.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/circle_buffer.hpp>

namespace mbgl {

class CircleRenderable : public Renderable {
public:
    using TriangleGroup = ElementGroup<3>;

    void upload(gl::ObjectStore& store) override {
        if (!uploaded) {
            vertexBuffer.upload(store);
            elementsBuffer.upload(store);
            uploaded = true;
        }
    }

    CircleVertexBuffer vertexBuffer;
    TriangleElementsBuffer elementsBuffer;
    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;
};

} // namespace mbgl
