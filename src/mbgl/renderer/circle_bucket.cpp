#include <mbgl/renderer/circle_bucket.hpp>

using namespace mbgl;

CircleBucket::CircleBucket(CircleVertexBuffer& vertexBuffer,
                           TriangleElementsBuffer& elementsBuffer)
    : vertexBuffer_(vertexBuffer)
    , elementsBuffer_(elementsBuffer) {
}

CircleBucket::~CircleBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}

void CircleBucket::upload() {
    vertexBuffer_.upload();
    elementsBuffer_.upload();
    uploaded = true;
}

void CircleBucket::render(Painter& /* painter */,
                        const StyleLayer& /* layer_desc */,
                        const TileID& /* id */,
                        const mat4& /* matrix */) {
    // XXX Just a stub for now.
    //painter.renderCircle(*this, layer_desc, id, matrix);
}

bool CircleBucket::hasData() const {
    return !triangleGroups_.empty();
}

void CircleBucket::addGeometry(const GeometryCollection& /* geometryCollection */) {
    // XXX Just a stub for now.
}
