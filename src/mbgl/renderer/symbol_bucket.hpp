#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/programs/collision_box_program.hpp>
#include <mbgl/text/glyph_range.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

#include <vector>

namespace mbgl {

class SymbolBucket : public Bucket {
public:
    SymbolBucket(style::SymbolLayoutProperties::Evaluated,
                 const std::unordered_map<std::string, style::SymbolPaintProperties::Evaluated>&,
                 float zoom,
                 bool sdfIcons,
                 bool iconsNeedLinear);

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool hasTextData() const;
    bool hasIconData() const;
    bool hasCollisionBoxData() const;

    const style::SymbolLayoutProperties::Evaluated layout;
    const bool sdfIcons;
    const bool iconsNeedLinear;

    std::unordered_map<std::string, SymbolIconProgram::PaintPropertyBinders> paintPropertyBinders;

    struct TextBuffer {
        gl::VertexVector<SymbolLayoutVertex> vertices;
        gl::IndexVector<gl::Triangles> triangles;
        gl::SegmentVector<SymbolAttributes> segments;

        optional<gl::VertexBuffer<SymbolLayoutVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;
    } text;

    struct IconBuffer {
        gl::VertexVector<SymbolLayoutVertex> vertices;
        gl::IndexVector<gl::Triangles> triangles;
        gl::SegmentVector<SymbolAttributes> segments;

        optional<gl::VertexBuffer<SymbolLayoutVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;
    } icon;

    struct CollisionBoxBuffer {
        gl::VertexVector<CollisionBoxVertex> vertices;
        gl::IndexVector<gl::Lines> lines;
        gl::SegmentVector<CollisionBoxAttributes> segments;

        optional<gl::VertexBuffer<CollisionBoxVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Lines>> indexBuffer;
    } collisionBox;
};

} // namespace mbgl
