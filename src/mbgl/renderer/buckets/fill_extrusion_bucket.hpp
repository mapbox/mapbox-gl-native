#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

namespace mbgl {

class BucketParameters;
class RenderFillExtrusionLayer;

class FillExtrusionBucket : public Bucket {
public:

    // These aliases are used by the PatternLayout template
    using RenderLayerType = RenderFillExtrusionLayer;
    using PossiblyEvaluatedPaintProperties = style::FillExtrusionPaintProperties::PossiblyEvaluated;
    using PossiblyEvaluatedLayoutProperties = style::Properties<>::PossiblyEvaluated;

    FillExtrusionBucket(const PossiblyEvaluatedLayoutProperties,
                        std::map<std::string, PossiblyEvaluatedPaintProperties>,
                        const float,
                        const uint32_t);

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&,
                    const mbgl::ImagePositions&,
                    const PatternLayerMap&) override;

    bool hasData() const override;

    void upload(gl::Context&) override;

    float getQueryRadius(const RenderLayer&) const override;

    gl::VertexVector<FillExtrusionLayoutVertex> vertices;
    gl::IndexVector<gl::Triangles> triangles;
    SegmentVector<FillExtrusionAttributes> triangleSegments;

    optional<gl::VertexBuffer<FillExtrusionLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;
    
    std::unordered_map<std::string, FillExtrusionProgram::PaintPropertyBinders> paintPropertyBinders;
};

template <>
inline bool Bucket::is<FillExtrusionBucket>() const {
    return layerType == style::LayerType::FillExtrusion;
}

} // namespace mbgl
