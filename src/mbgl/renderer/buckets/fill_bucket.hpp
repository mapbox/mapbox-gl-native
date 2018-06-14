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
class RenderFillLayer;

class FillBucket : public Bucket {
public:

    // These aliases are used by the PatternLayout template
    using RenderLayerType = RenderFillLayer;
    using PossiblyEvaluatedPaintProperties = style::FillPaintProperties::PossiblyEvaluated;
    using PossiblyEvaluatedLayoutProperties = style::Properties<>::PossiblyEvaluated;

    FillBucket(const PossiblyEvaluatedLayoutProperties layout,
               std::map<std::string, PossiblyEvaluatedPaintProperties> layerPaintProperties,
               const float zoom,
               const uint32_t overscaling);

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&,
                    const mbgl::ImagePositions&,
                    const PatternLayerMap&) override;

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
