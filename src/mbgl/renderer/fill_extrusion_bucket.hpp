#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

namespace mbgl {

namespace style {
class BucketParameters;
class Style;
} // namespace style

class FillExtrusionBucket : public Bucket {
public:
    FillExtrusionBucket(const style::BucketParameters&, const std::vector<const style::Layer*>&);

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&) override;
    bool hasData() const override;

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&, const style::Style&) override;

    gl::VertexVector<FillExtrusionLayoutVertex> vertices;
    gl::IndexVector<gl::Triangles> triangles;
    gl::SegmentVector<FillExtrusionAttributes> triangleSegments;

    optional<gl::VertexBuffer<FillExtrusionLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;
    
    std::unordered_map<std::string, FillExtrusionProgram::PaintPropertyBinders> paintPropertyBinders;
};

bool isBoundaryEdge(const Point<int16_t>&, const Point<int16_t>&);

} // namespace mbgl
