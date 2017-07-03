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

class SymbolBucket : public Bucket {
public:
    SymbolBucket(style::SymbolLayoutProperties::PossiblyEvaluated,
                 const std::map<std::string, std::pair<style::IconPaintProperties::Evaluated, style::TextPaintProperties::Evaluated>>&,
                 const style::DataDrivenPropertyValue<float>& textSize,
                 const style::DataDrivenPropertyValue<float>& iconSize,
                 float zoom,
                 bool sdfIcons,
                 bool iconsNeedLinear);

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const RenderLayer&, const RenderTile&) override;
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
        gl::IndexVector<gl::Triangles> triangles;
        SegmentVector<SymbolTextAttributes> segments;

        optional<gl::VertexBuffer<SymbolLayoutVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;
    } text;
    
    std::unique_ptr<SymbolSizeBinder> iconSizeBinder;
    
    struct IconBuffer {
        gl::VertexVector<SymbolLayoutVertex> vertices;
        gl::IndexVector<gl::Triangles> triangles;
        SegmentVector<SymbolIconAttributes> segments;

        optional<gl::VertexBuffer<SymbolLayoutVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;
    } icon;

    struct CollisionBoxBuffer {
        gl::VertexVector<CollisionBoxVertex> vertices;
        gl::IndexVector<gl::Lines> lines;
        SegmentVector<CollisionBoxAttributes> segments;

        optional<gl::VertexBuffer<CollisionBoxVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Lines>> indexBuffer;
    } collisionBox;

    SpriteAtlas* spriteAtlas = nullptr;
};

} // namespace mbgl
