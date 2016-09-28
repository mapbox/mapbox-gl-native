#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/geometry/collision_box_buffer.hpp>
#include <mbgl/text/glyph_range.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

#include <memory>
#include <vector>

namespace mbgl {

class SDFShader;
class IconShader;
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

    void drawGlyphs(SDFShader&, gl::Context&, PaintMode);
    void drawIcons(SDFShader&, gl::Context&, PaintMode);
    void drawIcons(IconShader&, gl::Context&, PaintMode);
    void drawCollisionBoxes(CollisionBoxShader&, gl::Context&);

    const MapMode mode;
    const style::SymbolLayoutProperties layout;
    const bool sdfIcons;
    const bool iconsNeedLinear;

private:
    friend class SymbolLayout;

    typedef ElementGroup<2> TextElementGroup;
    typedef ElementGroup<4> IconElementGroup;
    typedef ElementGroup<1> CollisionBoxElementGroup;

    struct TextBuffer {
        TextVertexBuffer vertices;
        TriangleElementsBuffer triangles;
        std::vector<std::unique_ptr<TextElementGroup>> groups;
    } text;

    struct IconBuffer {
        IconVertexBuffer vertices;
        TriangleElementsBuffer triangles;
        std::vector<std::unique_ptr<IconElementGroup>> groups;
    } icon;

    struct CollisionBoxBuffer {
        CollisionBoxVertexBuffer vertices;
        std::vector<std::unique_ptr<CollisionBoxElementGroup>> groups;
    } collisionBox;
};

} // namespace mbgl
