#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>

namespace mbgl {

using namespace style;

SymbolBucket::SymbolBucket(const MapMode mode_,
                           style::SymbolLayoutProperties::Evaluated layout_,
                           bool sdfIcons_,
                           bool iconsNeedLinear_)
    : mode(mode_),
      layout(std::move(layout_)),
      sdfIcons(sdfIcons_),
      iconsNeedLinear(iconsNeedLinear_) {
}

void SymbolBucket::upload(gl::Context& context) {
    if (hasTextData()) {
        text.vertexBuffer = context.createVertexBuffer(std::move(text.vertices));
        text.indexBuffer = context.createIndexBuffer(std::move(text.triangles));
    }

    if (hasIconData()) {
        icon.vertexBuffer = context.createVertexBuffer(std::move(icon.vertices));
        icon.indexBuffer = context.createIndexBuffer(std::move(icon.triangles));
    }

    if (!collisionBox.vertices.empty()) {
        collisionBox.vertexBuffer = context.createVertexBuffer(std::move(collisionBox.vertices));
        collisionBox.indexBuffer = context.createIndexBuffer(std::move(collisionBox.lines));
    }

    uploaded = true;
}

void SymbolBucket::render(Painter& painter,
                          PaintParameters& parameters,
                          const Layer& layer,
                          const RenderTile& tile) {
    painter.renderSymbol(parameters, *this, *layer.as<SymbolLayer>(), tile);
}

bool SymbolBucket::hasData() const {
    assert(false); // Should be calling SymbolLayout::has{Text,Icon,CollisonBox}Data() instead.
    return false;
}

bool SymbolBucket::hasTextData() const {
    return !text.segments.empty();
}

bool SymbolBucket::hasIconData() const {
    return !icon.segments.empty();
}

bool SymbolBucket::hasCollisionBoxData() const {
    return !collisionBox.segments.empty();
}

} // namespace mbgl
