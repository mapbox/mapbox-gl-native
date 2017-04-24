#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/programs/circle_program.hpp>
#include <mbgl/style/layers/circle_layer_properties.hpp>

namespace mbgl {

namespace style {
class BucketParameters;
} // namespace style

class CircleBucket : public Bucket {
public:
    CircleBucket(const style::BucketParameters&, const std::vector<const style::Layer*>&);

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&) override;
    bool hasData() const override;

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;

    float getQueryRadius(const style::Layer&) const override;

    gl::VertexVector<CircleLayoutVertex> vertices;
    gl::IndexVector<gl::Triangles> triangles;
    gl::SegmentVector<CircleAttributes> segments;

    optional<gl::VertexBuffer<CircleLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;

    std::map<std::string, CircleProgram::PaintPropertyBinders> paintPropertyBinders;

    const MapMode mode;
};

} // namespace mbgl
