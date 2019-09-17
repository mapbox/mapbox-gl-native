#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/circle_program.hpp>
#include <mbgl/style/layers/circle_layer_properties.hpp>

namespace mbgl {

class BucketParameters;

class CircleBucket final : public Bucket {
public:
    CircleBucket(const BucketParameters&, const std::vector<Immutable<style::LayerProperties>>&);
    ~CircleBucket() override;

    void addFeature(const GeometryTileFeature&, const GeometryCollection&, const ImagePositions&,
                    const PatternLayerMap&, std::size_t) override;

    bool hasData() const override;

    void upload(gfx::UploadPass&) override;

    float getQueryRadius(const RenderLayer&) const override;

    void update(const FeatureStates&, const GeometryTileLayer&, const std::string&, const ImagePositions&) override;

    gfx::VertexVector<CircleLayoutVertex> vertices;
    gfx::IndexVector<gfx::Triangles> triangles;
    SegmentVector<CircleAttributes> segments;

    optional<gfx::VertexBuffer<CircleLayoutVertex>> vertexBuffer;
    optional<gfx::IndexBuffer> indexBuffer;

    std::map<std::string, CircleProgram::Binders> paintPropertyBinders;

    const MapMode mode;
};

} // namespace mbgl
