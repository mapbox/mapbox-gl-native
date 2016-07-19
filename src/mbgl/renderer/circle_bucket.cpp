#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/painter.hpp>

#include <mbgl/shader/circle_shader.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

using namespace style;

CircleBucket::CircleBucket(MapMode mode_) : mode(mode_) {
}

CircleBucket::~CircleBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}

void CircleBucket::upload(gl::ObjectStore& store, gl::Config&) {
    vertexBuffer_.upload(store);
    elementsBuffer_.upload(store);
    uploaded = true;
}

void CircleBucket::render(Painter& painter,
                        PaintParameters& parameters, 
                        const Layer& layer,
                        const RenderTile& tile) {
    painter.renderCircle(parameters, *this, *layer.as<CircleLayer>(), tile);
}

bool CircleBucket::hasData() const {
    return !triangleGroups_.empty();
}

bool CircleBucket::needsClipping() const {
    return true;
}

void CircleBucket::addGeometry(const GeometryCollection& geometryCollection) {
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
            vertexBuffer_.add(x, y, -1, -1); // 1
            vertexBuffer_.add(x, y, 1, -1); // 2
            vertexBuffer_.add(x, y, 1, 1); // 3
            vertexBuffer_.add(x, y, -1, 1); // 4

            if (!triangleGroups_.size() || (triangleGroups_.back()->vertex_length + 4 > 65535)) {
                // Move to a new group because the old one can't hold the geometry.
                triangleGroups_.emplace_back(std::make_unique<TriangleGroup>());
            }

            TriangleGroup& group = *triangleGroups_.back();
            auto index = group.vertex_length;

            // 1, 2, 3
            // 1, 4, 3
            elementsBuffer_.add(index, index + 1, index + 2);
            elementsBuffer_.add(index, index + 3, index + 2);

            group.vertex_length += 4;
            group.elements_length += 2;
        }
    }
}

void CircleBucket::drawCircles(CircleShader& shader, gl::ObjectStore& store) {
    GLbyte* vertexIndex = BUFFER_OFFSET(0);
    GLbyte* elementsIndex = BUFFER_OFFSET(0);

    for (auto& group : triangleGroups_) {
        assert(group);

        if (!group->elements_length) continue;

        group->array[0].bind(shader, vertexBuffer_, elementsBuffer_, vertexIndex, store);

        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elementsIndex));

        vertexIndex += group->vertex_length * vertexBuffer_.itemSize;
        elementsIndex += group->elements_length * elementsBuffer_.itemSize;
    }
}

} // namespace mbgl
