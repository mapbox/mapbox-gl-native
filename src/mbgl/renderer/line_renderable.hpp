#pragma once

#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/line_buffer.hpp>

namespace mbgl {

class LineRenderable {
public:
    using TriangleGroup = ElementGroup<3>;

    LineVertexBuffer vertexBuffer;
    TriangleElementsBuffer triangleElementsBuffer;
    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;
};

} // namespace mbgl
