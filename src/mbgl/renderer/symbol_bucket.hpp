#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/element_group.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/shader/symbol_vertex.hpp>
#include <mbgl/shader/collision_box_vertex.hpp>
#include <mbgl/text/glyph_range.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

#include <memory>
#include <vector>

namespace mbgl {

class SymbolSDFShader;
class SymbolIconShader;
class CollisionBoxShader;

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
    bool needsClipping() const override;

    void drawGlyphs(SymbolSDFShader&, gl::Context&, PaintMode);
    void drawIcons(SymbolSDFShader&, gl::Context&, PaintMode);
    void drawIcons(SymbolIconShader&, gl::Context&, PaintMode);
    void drawCollisionBoxes(CollisionBoxShader&, gl::Context&);

    const MapMode mode;
    const style::SymbolLayoutProperties layout;
    const bool sdfIcons;
    const bool iconsNeedLinear;

private:
    friend class SymbolLayout;

    struct TextBuffer {
        std::vector<SymbolVertex> vertices;
        std::vector<gl::Triangle> triangles;
        std::vector<ElementGroup<SymbolSDFShader>> groups;

        optional<gl::VertexBuffer<SymbolVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Triangle>> indexBuffer;
    } text;

    struct IconBuffer {
        std::vector<SymbolVertex> vertices;
        std::vector<gl::Triangle> triangles;
        std::vector<ElementGroup<SymbolSDFShader, SymbolIconShader>> groups;

        optional<gl::VertexBuffer<SymbolVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Triangle>> indexBuffer;
    } icon;

    struct CollisionBoxBuffer {
        std::vector<CollisionBoxVertex> vertices;
        std::vector<gl::Line> lines;
        std::vector<ElementGroup<CollisionBoxShader>> groups;

        optional<gl::VertexBuffer<CollisionBoxVertex>> vertexBuffer;
        optional<gl::IndexBuffer<gl::Line>> indexBuffer;
    } collisionBox;
};

} // namespace mbgl
