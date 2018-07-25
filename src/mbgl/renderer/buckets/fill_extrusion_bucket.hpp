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

class FillExtrusionBucket : public Bucket {
public:
    FillExtrusionBucket(const BucketParameters&, const std::vector<const RenderLayer*>&);

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&) override;
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
