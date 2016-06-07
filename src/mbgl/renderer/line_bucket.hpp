#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>

#include <vector>

namespace mbgl {

class LineRenderable;

class LineBucket : public Bucket {
public:
    LineBucket(uint32_t overscaling);
    ~LineBucket() override;

    LineRenderable& getRenderable() const;

    void upload(gl::ObjectStore&) override;
    void render(Painter&, const style::Layer&, const UnwrappedTileID&, const mat4&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void addGeometry(const GeometryCollection&);
    void addGeometry(const GeometryCoordinates& line);

private:
    struct TriangleElement {
        TriangleElement(uint16_t a_, uint16_t b_, uint16_t c_) : a(a_), b(b_), c(c_) {}
        uint16_t a, b, c;
    };
    void addCurrentVertex(const GeometryCoordinate& currentVertex, double& distance,
            const Point<double>& normal, double endLeft, double endRight, bool round,
            GLint startVertex, std::vector<LineBucket::TriangleElement>& triangleStore);
    void addPieSliceVertex(const GeometryCoordinate& currentVertex, double distance,
            const Point<double>& extrude, bool lineTurnsLeft, GLint startVertex,
            std::vector<TriangleElement>& triangleStore);

public:
    style::LineLayoutProperties layout;

private:
    GLint e1;
    GLint e2;
    GLint e3;

    const uint32_t overscaling;

    std::unique_ptr<LineRenderable> renderable;
};

} // namespace mbgl
