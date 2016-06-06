#pragma once

#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/circle_buffer.hpp>

namespace mbgl {

class CircleRenderable {
public:
    using TriangleGroup = ElementGroup<3>;

    CircleVertexBuffer vertexBuffer;
    TriangleElementsBuffer elementsBuffer;
    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;
};

} // namespace mbgl
