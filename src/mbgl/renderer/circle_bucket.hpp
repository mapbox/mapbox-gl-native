#ifndef MBGL_RENDERER_CIRCLE_BUCKET
#define MBGL_RENDERER_CIRCLE_BUCKET

#include <mbgl/renderer/bucket.hpp>

#include <mbgl/map/geometry_tile.hpp>

#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/circle_buffer.hpp>

#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_layout.hpp>

namespace mbgl {

class CircleVertexBuffer;
class CircleShader;

class CircleBucket : public Bucket {
    using TriangleGroup = ElementGroup<3>;

public:
    CircleBucket(CircleVertexBuffer &vertexBuffer, TriangleElementsBuffer &elementsBuffer);
    ~CircleBucket() override;

    void upload() override;
    void render(Painter&, const StyleLayer&, const TileID&, const mat4&) override;

    bool hasData() const;
    void addGeometry(const GeometryCollection&);

    void drawCircles(CircleShader& shader);

private:
    CircleVertexBuffer& vertexBuffer_;
    TriangleElementsBuffer& elementsBuffer_;

    const size_t vertexStart_;
    const size_t elementsStart_;

    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups_;
};

} // namespace mbgl

#endif // MBGL_RENDERER_CIRCLE_BUCKET
