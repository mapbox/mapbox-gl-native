#ifndef MBGL_RENDERER_CIRCLE_BUCKET
#define MBGL_RENDERER_CIRCLE_BUCKET

#include <mbgl/renderer/bucket.hpp>

#include <mbgl/map/geometry_tile.hpp>

#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/circle_buffer.hpp>

namespace mbgl {

class CircleVertexBuffer;
class CircleShader;

class CircleBucket : public Bucket {
    using TriangleGroup = ElementGroup<3>;

public:
    CircleBucket();
    ~CircleBucket() override;

    void upload() override;
    void render(Painter&, const StyleLayer&, const TileID&, const mat4&) override;

    bool hasData() const override;
    void addGeometry(const GeometryCollection&);

    void drawCircles(CircleShader& shader);

private:
    CircleVertexBuffer vertexBuffer_;
    TriangleElementsBuffer elementsBuffer_;

    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups_;
};

} // namespace mbgl

#endif // MBGL_RENDERER_CIRCLE_BUCKET
