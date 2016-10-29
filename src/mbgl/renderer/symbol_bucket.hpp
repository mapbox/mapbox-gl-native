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
    SymbolBucket(const MapMode,
                 style::SymbolLayoutProperties,
                 bool sdfIcons,
                 bool iconsNeedLinear);

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool hasTextData() const;
    bool hasIconData() const;
    bool hasCollisionBoxData() const;

    const MapMode mode;
    const style::SymbolLayoutProperties layout;
    const bool sdfIcons;
    const bool iconsNeedLinear;

    struct TextBuffer {
        std::vector<SymbolVertex> vertices;
        std::vector<gl::Triangle> triangles;
        std::vector<gl::Segment> segments;

        optional<gl::VertexBuffer<SymbolVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Triangle>> indexBuffer;
    } text;

    struct IconBuffer {
        std::vector<SymbolVertex> vertices;
        std::vector<gl::Triangle> triangles;
        std::vector<gl::Segment> segments;

        optional<gl::VertexBuffer<SymbolVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Triangle>> indexBuffer;
    } icon;

    struct CollisionBoxBuffer {
        std::vector<CollisionBoxVertex> vertices;
        optional<gl::VertexBuffer<CollisionBoxVertex>> vertexBuffer;
    } collisionBox;
};

} // namespace mbgl
