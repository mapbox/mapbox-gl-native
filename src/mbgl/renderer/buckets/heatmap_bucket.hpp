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

class HeatmapBucket : public Bucket {
public:
    HeatmapBucket(const BucketParameters&, const std::vector<const RenderLayer*>&);

    void addFeature(const GeometryTileFeature&,
                            const GeometryCollection&,
                            const ImagePositions&,
                            const PatternLayerMap&) override;
    bool hasData() const override;

    void upload(gl::Context&) override;

    float getQueryRadius(const RenderLayer&) const override;

    gl::VertexVector<HeatmapLayoutVertex> vertices;
    gl::IndexVector<gl::Triangles> triangles;
    SegmentVector<HeatmapAttributes> segments;

    optional<gl::VertexBuffer<HeatmapLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;

    std::map<std::string, HeatmapProgram::PaintPropertyBinders> paintPropertyBinders;

    const MapMode mode;
};

template <>
inline bool Bucket::is<HeatmapBucket>() const {
    return layerType == style::LayerType::Heatmap;
}

} // namespace mbgl
