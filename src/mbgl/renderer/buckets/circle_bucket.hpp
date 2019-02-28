#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/circle_program.hpp>
#include <mbgl/style/layers/circle_layer_properties.hpp>

namespace mbgl {

class BucketParameters;

class CircleBucket final : public Bucket {
public:
    CircleBucket(const BucketParameters&, const std::vector<const RenderLayer*>&);
    ~CircleBucket() override;

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&,
                    const ImagePositions&,
                    const PatternLayerMap&) override;

    bool hasData() const override;

    void upload(gl::Context&) override;

    float getQueryRadius(const RenderLayer&) const override;
    bool supportsLayer(const style::Layer::Impl&) const override;

    gl::VertexVector<CircleLayoutVertex> vertices;
    gl::IndexVector<gfx::Triangles> triangles;
    SegmentVector<CircleAttributes> segments;

    optional<gl::VertexBuffer<CircleLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gfx::Triangles>> indexBuffer;

    std::map<std::string, CircleProgram::Binders> paintPropertyBinders;

    const MapMode mode;
};

} // namespace mbgl
