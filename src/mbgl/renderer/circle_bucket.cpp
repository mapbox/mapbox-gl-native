#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/gl/context.hpp>

#include <mbgl/shader/circle_shader.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

using namespace style;

CircleBucket::CircleBucket(MapMode mode_) : mode(mode_) {
}

void CircleBucket::upload(gl::Context& context) {
    vertexBuffer = context.createVertexBuffer(std::move(vertices));
    indexBuffer = context.createIndexBuffer(std::move(triangles));
    uploaded = true;
}

void CircleBucket::render(Painter& painter,
                        PaintParameters& parameters, 
                        const Layer& layer,
                        const RenderTile& tile) {
    painter.renderCircle(parameters, *this, *layer.as<CircleLayer>(), tile);
}

bool CircleBucket::hasData() const {
    return !segments.empty();
}

void CircleBucket::addGeometry(const GeometryCollection& geometryCollection) {
    constexpr const uint16_t vertexLength = 4;

    for (auto& circle : geometryCollection) {
        for(auto& point : circle) {
            auto x = point.x;
            auto y = point.y;

            // Do not include points that are outside the tile boundaries.
            // Include all points in Still mode. You need to include points from
            // neighbouring tiles so that they are not clipped at tile boundaries.
            if ((mode != MapMode::Still) &&
                (x < 0 || x >= util::EXTENT || y < 0 || y >= util::EXTENT)) continue;

            if (segments.empty() || segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max()) {
                // Move to a new segments because the old one can't hold the geometry.
                segments.emplace_back(vertices.size(), triangles.size());
            }

            // this geometry will be of the Point type, and we'll derive
            // two triangles from it.
            //
            // ┌─────────┐
            // │ 4     3 │
            // │         │
            // │ 1     2 │
            // └─────────┘
            //
            vertices.emplace_back(CircleAttributes::vertex(point, -1, -1)); // 1
            vertices.emplace_back(CircleAttributes::vertex(point,  1, -1)); // 2
            vertices.emplace_back(CircleAttributes::vertex(point,  1,  1)); // 3
            vertices.emplace_back(CircleAttributes::vertex(point, -1,  1)); // 4

            auto& segment = segments.back();
            assert(segment.vertexLength <= std::numeric_limits<uint16_t>::max());
            uint16_t index = segment.vertexLength;

            // 1, 2, 3
            // 1, 4, 3
            triangles.emplace_back(index, index + 1, index + 2);
            triangles.emplace_back(index, index + 3, index + 2);

            segment.vertexLength += vertexLength;
            segment.primitiveLength += 2;
        }
    }
}

} // namespace mbgl
