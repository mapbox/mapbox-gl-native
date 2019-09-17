#pragma once
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/line_program.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>
#include <mbgl/style/image_impl.hpp>

#include <vector>

namespace mbgl {

class BucketParameters;
class RenderLineLayer;

class LineBucket final : public Bucket {
public:
    using PossiblyEvaluatedPaintProperties = style::LinePaintProperties::PossiblyEvaluated;
    using PossiblyEvaluatedLayoutProperties = style::LineLayoutProperties::PossiblyEvaluated;

    LineBucket(const PossiblyEvaluatedLayoutProperties layout,
               const std::map<std::string, Immutable<style::LayerProperties>>& layerPaintProperties,
               const float zoom,
               const uint32_t overscaling);
    ~LineBucket() override;

    void addFeature(const GeometryTileFeature&, const GeometryCollection&, const mbgl::ImagePositions& patternPositions,
                    const PatternLayerMap&, std::size_t) override;

    bool hasData() const override;

    void upload(gfx::UploadPass&) override;

    float getQueryRadius(const RenderLayer&) const override;

    void update(const FeatureStates&, const GeometryTileLayer&, const std::string&, const ImagePositions&) override;

    PossiblyEvaluatedLayoutProperties layout;

    gfx::VertexVector<LineLayoutVertex> vertices;
    gfx::IndexVector<gfx::Triangles> triangles;
    SegmentVector<LineAttributes> segments;

    optional<gfx::VertexBuffer<LineLayoutVertex>> vertexBuffer;
    optional<gfx::IndexBuffer> indexBuffer;

    std::map<std::string, LineProgram::Binders> paintPropertyBinders;

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
};

} // namespace mbgl
