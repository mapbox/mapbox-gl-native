#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>

#include <vector>

namespace mbgl {

class BucketParameters;

class FillBucket : public Bucket {
public:
    FillBucket(const BucketParameters&, const std::vector<const RenderLayer*>&);

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&) override;
    bool hasData() const override;

    void upload(gl::Context&) override;

    float getQueryRadius(const RenderLayer&) const override;

    gl::VertexVector<FillLayoutVertex> vertices;
    gl::IndexVector<gl::Lines> lines;
    gl::IndexVector<gl::Triangles> triangles;
    SegmentVector<FillAttributes> lineSegments;
    SegmentVector<FillAttributes> triangleSegments;

    optional<gl::VertexBuffer<FillLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Lines>> lineIndexBuffer;
    optional<gl::IndexBuffer<gl::Triangles>> triangleIndexBuffer;

    std::map<std::string, FillProgram::PaintPropertyBinders> paintPropertyBinders;
};

template <>
inline bool Bucket::is<FillBucket>() const {
    return layerType == style::LayerType::Fill;
}

} // namespace mbgl
