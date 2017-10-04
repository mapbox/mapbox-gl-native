#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/programs/collision_box_program.hpp>
#include <mbgl/text/glyph_range.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/layout/symbol_feature.hpp>

#include <vector>

namespace mbgl {

class PlacedSymbol {
public:
    PlacedSymbol(Point<float> anchorPoint_, uint16_t segment_, float lowerSize_, float upperSize_,
            std::array<float, 2> lineOffset_, float placementZoom_, bool useVerticalMode_, GeometryCoordinates line_) :
        anchorPoint(anchorPoint_), segment(segment_), lowerSize(lowerSize_), upperSize(upperSize_),
        lineOffset(lineOffset_), placementZoom(placementZoom_), useVerticalMode(useVerticalMode_), line(std::move(line_))
    {
        // TODO WIP hook these up
        writingMode = WritingModeType::None;
        tileDistances = std::vector<float>(line.size());
        hidden = false;
    }
    Point<float> anchorPoint;
    uint16_t segment;
    float lowerSize;
    float upperSize;
    std::array<float, 2> lineOffset;
    float placementZoom;
    bool useVerticalMode;
    GeometryCoordinates line;
    std::vector<float> tileDistances;
    std::vector<float> glyphOffsets;
    WritingModeType writingMode;
    bool hidden;
};

class SymbolBucket : public Bucket {
public:
    SymbolBucket(style::SymbolLayoutProperties::PossiblyEvaluated,
                 const std::map<std::string, std::pair<style::IconPaintProperties::PossiblyEvaluated, style::TextPaintProperties::PossiblyEvaluated>>&,
                 const style::DataDrivenPropertyValue<float>& textSize,
                 const style::DataDrivenPropertyValue<float>& iconSize,
                 float zoom,
                 bool sdfIcons,
                 bool iconsNeedLinear);

    void upload(gl::Context&) override;
    bool hasData() const override;
    bool hasTextData() const;
    bool hasIconData() const;
    bool hasCollisionBoxData() const;

    const style::SymbolLayoutProperties::PossiblyEvaluated layout;
    const bool sdfIcons;
    const bool iconsNeedLinear;

    std::map<std::string, std::pair<
        SymbolIconProgram::PaintPropertyBinders,
        SymbolSDFTextProgram::PaintPropertyBinders>> paintPropertyBinders;
    
    std::unique_ptr<SymbolSizeBinder> textSizeBinder;

    struct TextBuffer {
        gl::VertexVector<SymbolLayoutVertex> vertices;
        gl::VertexVector<SymbolDynamicLayoutAttributes::Vertex> dynamicVertices;
        gl::VertexVector<SymbolOpacityAttributes::Vertex> opacityVertices;
        gl::IndexVector<gl::Triangles> triangles;
        SegmentVector<SymbolTextAttributes> segments;
        std::vector<PlacedSymbol> placedSymbols;

        optional<gl::VertexBuffer<SymbolLayoutVertex>> vertexBuffer;
        optional<gl::VertexBuffer<SymbolDynamicLayoutAttributes::Vertex>> dynamicVertexBuffer;
        optional<gl::VertexBuffer<SymbolOpacityAttributes::Vertex>> opacityVertexBuffer;
        optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;
    } text;
    
    std::unique_ptr<SymbolSizeBinder> iconSizeBinder;
    
    struct IconBuffer {
        gl::VertexVector<SymbolLayoutVertex> vertices;
        gl::VertexVector<SymbolDynamicLayoutAttributes::Vertex> dynamicVertices;
        gl::VertexVector<SymbolOpacityAttributes::Vertex> opacityVertices;
        gl::IndexVector<gl::Triangles> triangles;
        SegmentVector<SymbolIconAttributes> segments;
        std::vector<PlacedSymbol> placedSymbols;
        PremultipliedImage atlasImage;

        optional<gl::VertexBuffer<SymbolLayoutVertex>> vertexBuffer;
        optional<gl::VertexBuffer<SymbolDynamicLayoutAttributes::Vertex>> dynamicVertexBuffer;
        optional<gl::VertexBuffer<SymbolOpacityAttributes::Vertex>> opacityVertexBuffer;
        optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;
    } icon;

    struct CollisionBoxBuffer {
        gl::VertexVector<CollisionBoxVertex> vertices;
        gl::IndexVector<gl::Lines> lines;
        SegmentVector<CollisionBoxAttributes> segments;

        optional<gl::VertexBuffer<CollisionBoxVertex>> vertexBuffer;
        optional<gl::VertexBuffer<SymbolDynamicLayoutAttributes::Vertex>> dynamicVertexBuffer;
        optional<gl::IndexBuffer<gl::Lines>> indexBuffer;
    } collisionBox;
};

} // namespace mbgl
