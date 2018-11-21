#pragma once
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/line_program.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>
#include <mbgl/style/image_impl.hpp>

#include <vector>

namespace mbgl {

class BucketParameters;
class RenderLineLayer;

class LineBucket : public Bucket {
public:

    // These aliases are used by the PatternLayout template
    using RenderLayerType = RenderLineLayer;
    using PossiblyEvaluatedPaintProperties = RenderLinePaintProperties::PossiblyEvaluated;
    using PossiblyEvaluatedLayoutProperties = style::LineLayoutProperties::PossiblyEvaluated;

    LineBucket(const PossiblyEvaluatedLayoutProperties layout,
               std::map<std::string, PossiblyEvaluatedPaintProperties> layerPaintProperties,
               const float zoom,
               const uint32_t overscaling);

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&,
                    const mbgl::ImagePositions& patternPositions,
                    const PatternLayerMap&) override;

    bool hasData() const override;

    void upload(gl::Context&) override;

    float getQueryRadius(const RenderLayer&) const override;

    PossiblyEvaluatedLayoutProperties layout;

    gl::VertexVector<LineLayoutVertex> vertices;
    gl::IndexVector<gl::Triangles> triangles;
    SegmentVector<LineAttributes> segments;

    optional<gl::VertexBuffer<LineLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;

    std::map<std::string, LineProgram::PaintPropertyBinders> paintPropertyBinders;

private:
    void addGeometry(const GeometryCoordinates&, const GeometryTileFeature&);

    struct TriangleElement {
        TriangleElement(uint16_t a_, uint16_t b_, uint16_t c_) : a(a_), b(b_), c(c_) {}
        uint16_t a, b, c;
    };

    class Distances;
    void addCurrentVertex(const GeometryCoordinate& currentVertex, double& distance,
            const Point<double>& normal, double endLeft, double endRight, bool round,
            std::size_t startVertex, std::vector<LineBucket::TriangleElement>& triangleStore,
            optional<Distances> distances);

    void addPieSliceVertex(const GeometryCoordinate& currentVertex, double distance,
            const Point<double>& extrude, bool lineTurnsLeft, std::size_t startVertex,
            std::vector<TriangleElement>& triangleStore,
            optional<Distances> distances);

    std::ptrdiff_t e1;
    std::ptrdiff_t e2;
    std::ptrdiff_t e3;

    const float zoom;
    const uint32_t overscaling;

    float getLineWidth(const RenderLineLayer& layer) const;
};

template <>
inline bool Bucket::is<LineBucket>() const {
    return layerType == style::LayerType::Line;
}

} // namespace mbgl
