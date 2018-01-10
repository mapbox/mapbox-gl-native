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
#include <mbgl/layout/symbol_instance.hpp>

#include <vector>

namespace mbgl {

class PlacedSymbol {
public:
    PlacedSymbol(Point<float> anchorPoint_, uint16_t segment_, float lowerSize_, float upperSize_,
            std::array<float, 2> lineOffset_, WritingModeType writingModes_, GeometryCoordinates line_, std::vector<float> tileDistances_) :
        anchorPoint(anchorPoint_), segment(segment_), lowerSize(lowerSize_), upperSize(upperSize_),
        lineOffset(lineOffset_), writingModes(writingModes_), line(std::move(line_)), tileDistances(std::move(tileDistances_)), hidden(false), vertexStartIndex(0)
    {
    }
    Point<float> anchorPoint;
    uint16_t segment;
    float lowerSize;
    float upperSize;
    std::array<float, 2> lineOffset;
    WritingModeType writingModes;
    GeometryCoordinates line;
    std::vector<float> tileDistances;
    std::vector<float> glyphOffsets;
    bool hidden;
    size_t vertexStartIndex;
};

class SymbolBucket : public Bucket {
public:
    SymbolBucket(style::SymbolLayoutProperties::PossiblyEvaluated,
                 const std::map<std::string, std::pair<style::IconPaintProperties::PossiblyEvaluated, style::TextPaintProperties::PossiblyEvaluated>>&,
                 const style::DataDrivenPropertyValue<float>& textSize,
                 const style::DataDrivenPropertyValue<float>& iconSize,
                 float zoom,
                 bool sdfIcons,
                 bool iconsNeedLinear,
                 bool sortFeaturesByY,
                 const std::vector<SymbolInstance>&&);

    void upload(gl::Context&) override;
    bool hasData() const override;
    bool hasTextData() const;
    bool hasIconData() const;
    bool hasCollisionBoxData() const;
    bool hasCollisionCircleData() const;
    
    void updateOpacity();
    void sortFeatures(const float angle);

    const style::SymbolLayoutProperties::PossiblyEvaluated layout;
    const bool sdfIcons;
    const bool iconsNeedLinear;
    const bool sortFeaturesByY;
    
    optional<float> sortedAngle;

    bool staticUploaded = false;
    bool placementChangesUploaded = false;
    bool dynamicUploaded = false;
    bool sortUploaded = false;

    std::vector<SymbolInstance> symbolInstances;

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

    struct CollisionBuffer {
        gl::VertexVector<CollisionBoxLayoutAttributes::Vertex> vertices;
        gl::VertexVector<CollisionBoxDynamicAttributes::Vertex> dynamicVertices;
        SegmentVector<CollisionBoxProgram::Attributes> segments;

        optional<gl::VertexBuffer<CollisionBoxLayoutAttributes::Vertex>> vertexBuffer;
        optional<gl::VertexBuffer<CollisionBoxDynamicAttributes::Vertex>> dynamicVertexBuffer;
    };

    struct CollisionBoxBuffer : public CollisionBuffer {
        gl::IndexVector<gl::Lines> lines;
        optional<gl::IndexBuffer<gl::Lines>> indexBuffer;
    } collisionBox;
    
    struct CollisionCircleBuffer : public CollisionBuffer {
        gl::IndexVector<gl::Triangles> triangles;
        optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;
    } collisionCircle;

    uint32_t bucketInstanceId = 0;
    bool justReloaded = false;
};

} // namespace mbgl
