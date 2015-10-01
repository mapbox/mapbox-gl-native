#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/painter.hpp>

#include <mbgl/shader/circle_shader.hpp>

using namespace mbgl;

CircleBucket::CircleBucket() {
}

CircleBucket::~CircleBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}

void CircleBucket::upload() {
    vertexBuffer_.upload();
    elementsBuffer_.upload();
    uploaded = true;
}

void CircleBucket::render(Painter& painter,
                        const StyleLayer& layer_desc,
                        const TileID& id,
                        const mat4& matrix) {
    painter.renderCircle(*this, layer_desc, id, matrix);
}

bool CircleBucket::hasData() const {
    return !triangleGroups_.empty();
}

void CircleBucket::addGeometry(const GeometryCollection& geometryCollection) {
    const int extent = 4096;
    for (auto& circle : geometryCollection) {
        for(auto & geometry : circle) {
            auto x = geometry.x;
            auto y = geometry.y;

            // Do not include points that are outside the tile boundaries.
            if (x < 0 || x >= extent || y < 0 || y >= extent) continue;

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

void CircleBucket::drawCircles(CircleShader& shader) {
    GLbyte* vertexIndex = BUFFER_OFFSET(0);
    GLbyte* elementsIndex = BUFFER_OFFSET(0);

    for (auto& group : triangleGroups_) {
        assert(group);

        if (!group->elements_length) continue;

        group->array[0].bind(shader, vertexBuffer_, elementsBuffer_, vertexIndex);

        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elementsIndex));

        vertexIndex += group->vertex_length * vertexBuffer_.itemSize;
        elementsIndex += group->elements_length * elementsBuffer_.itemSize;
    }
}
