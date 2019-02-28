#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/heatmap_program.hpp>
#include <mbgl/style/layers/heatmap_layer_properties.hpp>

namespace mbgl {

class BucketParameters;

class HeatmapBucket final : public Bucket {
public:
    HeatmapBucket(const BucketParameters&, const std::vector<const RenderLayer*>&);
    ~HeatmapBucket() override;

    void addFeature(const GeometryTileFeature&,
                            const GeometryCollection&,
                            const ImagePositions&,
                            const PatternLayerMap&) override;
    bool hasData() const override;

    void upload(gl::Context&) override;

    float getQueryRadius(const RenderLayer&) const override;
    bool supportsLayer(const style::Layer::Impl&) const override;

    gl::VertexVector<HeatmapLayoutVertex> vertices;
    gl::IndexVector<gfx::Triangles> triangles;
    SegmentVector<HeatmapAttributes> segments;

    optional<gl::VertexBuffer<HeatmapLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gfx::Triangles>> indexBuffer;

    std::map<std::string, HeatmapProgram::Binders> paintPropertyBinders;

    const MapMode mode;
};

} // namespace mbgl
