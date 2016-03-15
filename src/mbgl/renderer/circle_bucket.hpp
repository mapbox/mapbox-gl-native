#ifndef MBGL_RENDERER_CIRCLE_BUCKET
#define MBGL_RENDERER_CIRCLE_BUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/circle_buffer.hpp>

namespace mbgl {

class CircleVertexBuffer;
class CircleShader;

class CircleBucket : public Bucket {
    using TriangleGroup = ElementGroup<3>;

public:
    CircleBucket(const MapMode);
    ~CircleBucket() override;

    void upload(gl::GLObjectStore&) override;
    void render(Painter&, const StyleLayer&, const TileID&, const mat4&) override;

    bool hasData() const override;
    bool needsClipping() const override;
    void addGeometry(const GeometryCollection&);

    void drawCircles(CircleShader&, gl::GLObjectStore&);

private:
    CircleVertexBuffer vertexBuffer_;
    TriangleElementsBuffer elementsBuffer_;

    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups_;

    const MapMode mode;
};

} // namespace mbgl

#endif // MBGL_RENDERER_CIRCLE_BUCKET
