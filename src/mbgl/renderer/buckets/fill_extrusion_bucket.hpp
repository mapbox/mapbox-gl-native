#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

namespace mbgl {

class BucketParameters;
class RenderFillExtrusionLayer;

class FillExtrusionBucket final : public Bucket {
public:
    ~FillExtrusionBucket() override;
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

    bool supportsLayer(const style::Layer::Impl&) const override;

    void upload(gfx::Context&) override;

    float getQueryRadius(const RenderLayer&) const override;

    gfx::VertexVector<FillExtrusionLayoutVertex> vertices;
    gfx::IndexVector<gfx::Triangles> triangles;
    SegmentVector<FillExtrusionAttributes> triangleSegments;

    optional<gfx::VertexBuffer<FillExtrusionLayoutVertex>> vertexBuffer;
    optional<gfx::IndexBuffer> indexBuffer;
    
    std::unordered_map<std::string, FillExtrusionProgram::Binders> paintPropertyBinders;
};

} // namespace mbgl
