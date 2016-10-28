#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>

#include <vector>

namespace mbgl {

namespace style {
class BucketParameters;
} // namespace style

class FillBucket : public Bucket {
public:
    FillBucket(const style::BucketParameters&, const std::vector<const style::Layer*>&);

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&) override;
    bool hasData() const override;

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;

    gl::VertexVector<FillLayoutVertex> vertices;
    gl::IndexVector<gl::Lines> lines;
    gl::IndexVector<gl::Triangles> triangles;
    gl::SegmentVector<FillAttributes> lineSegments;
    gl::SegmentVector<FillAttributes> triangleSegments;

    optional<gl::VertexBuffer<FillLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Lines>> lineIndexBuffer;
    optional<gl::IndexBuffer<gl::Triangles>> triangleIndexBuffer;

    std::unordered_map<std::string, FillProgram::PaintPropertyBinders> paintPropertyBinders;
};

} // namespace mbgl
