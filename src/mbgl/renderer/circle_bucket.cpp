#include <mbgl/renderer/circle_bucket.hpp>

#include <mbgl/renderer/circle_renderable.hpp>
#include <mbgl/renderer/painter.hpp>

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/util/constants.hpp>

#include <cassert>

namespace mbgl {

using namespace style;

CircleBucket::CircleBucket(MapMode mode_)
    : mode(mode_), renderable(std::make_unique<CircleRenderable>()) {
}

CircleBucket::~CircleBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}

CircleRenderable& CircleBucket::getRenderable() const {
    assert(renderable);
    return *renderable;
}

void CircleBucket::upload(gl::ObjectStore& store) {
    renderable->upload(store);
    uploaded = true;
}

void CircleBucket::render(Painter& painter,
                        const Layer& layer,
                        const UnwrappedTileID& tileID,
                        const mat4& matrix) {
    painter.renderCircle(*this, *layer.as<CircleLayer>(), tileID, matrix);
}

bool CircleBucket::hasData() const {
    return renderable && !renderable->triangleGroups.empty();
}

bool CircleBucket::needsClipping() const {
    return true;
}

void CircleBucket::addGeometry(const GeometryCollection& geometryCollection) {
    auto& vertexBuffer = renderable->vertexBuffer;
    auto& elementsBuffer = renderable->elementsBuffer;
    auto& triangleGroups = renderable->triangleGroups;

    for (auto& circle : geometryCollection) {
        for(auto & geometry : circle) {
            auto x = geometry.x;
            auto y = geometry.y;

            // Do not include points that are outside the tile boundaries.
            // Include all points in Still mode. You need to include points from
            // neighbouring tiles so that they are not clipped at tile boundaries.
            if ((mode != MapMode::Still) &&
                (x < 0 || x >= util::EXTENT || y < 0 || y >= util::EXTENT)) continue;

            // this geometry will be of the Point type, and we'll derive
            // two triangles from it.
            //
            // ┌─────────┐
            // │ 4     3 │
            // │         │
            // │ 1     2 │
            // └─────────┘
            //
            vertexBuffer.add(x, y, -1, -1); // 1
            vertexBuffer.add(x, y, 1, -1); // 2
            vertexBuffer.add(x, y, 1, 1); // 3
            vertexBuffer.add(x, y, -1, 1); // 4

            if (!triangleGroups.size() || (triangleGroups.back()->vertex_length + 4 > 65535)) {
                // Move to a new group because the old one can't hold the geometry.
                triangleGroups.emplace_back(std::make_unique<CircleRenderable::TriangleGroup>());
            }

            auto& group = *triangleGroups.back();
            auto index = group.vertex_length;

            // 1, 2, 3
            // 1, 4, 3
            elementsBuffer.add(index, index + 1, index + 2);
            elementsBuffer.add(index, index + 3, index + 2);

            group.vertex_length += 4;
            group.elements_length += 2;
        }
    }
}

} // namespace mbgl
