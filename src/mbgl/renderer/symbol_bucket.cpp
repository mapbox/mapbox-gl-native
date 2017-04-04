#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/bucket_parameters.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>

namespace mbgl {

using namespace style;

SymbolBucket::SymbolBucket(style::SymbolLayoutProperties::PossiblyEvaluated layout_,
                           const std::map<std::string, std::pair<
                               style::IconPaintProperties::Evaluated, style::TextPaintProperties::Evaluated>>& layerPaintProperties,
                           const style::DataDrivenPropertyValue<float>& textSize,
                           const style::DataDrivenPropertyValue<float>& iconSize,
                           float zoom,
                           bool sdfIcons_,
                           bool iconsNeedLinear_)
    : layout(std::move(layout_)),
      sdfIcons(sdfIcons_),
      iconsNeedLinear(iconsNeedLinear_ || iconSize.isDataDriven() || !iconSize.isZoomConstant()),
      textSizeBinder(SymbolSizeBinder::create(zoom, textSize, TextSize::defaultValue())),
      iconSizeBinder(SymbolSizeBinder::create(zoom, iconSize, IconSize::defaultValue())) {
    
    for (const auto& pair : layerPaintProperties) {
        paintPropertyBinders.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(pair.first),
            std::forward_as_tuple(
                std::piecewise_construct,
                std::forward_as_tuple(pair.second.first, zoom),
                std::forward_as_tuple(pair.second.second, zoom)));
    }
}

void SymbolBucket::upload(gl::Context& context) {
    if (hasTextData()) {
        text.vertexBuffer = context.createVertexBuffer(std::move(text.vertices));
        text.indexBuffer = context.createIndexBuffer(std::move(text.triangles));
        textSizeBinder->upload(context);
    }

    if (hasIconData()) {
        icon.vertexBuffer = context.createVertexBuffer(std::move(icon.vertices));
        icon.indexBuffer = context.createIndexBuffer(std::move(icon.triangles));
        iconSizeBinder->upload(context);
    }

    if (!collisionBox.vertices.empty()) {
        collisionBox.vertexBuffer = context.createVertexBuffer(std::move(collisionBox.vertices));
        collisionBox.indexBuffer = context.createIndexBuffer(std::move(collisionBox.lines));
    }

    for (auto& pair : paintPropertyBinders) {
        pair.second.first.upload(context);
        pair.second.second.upload(context);
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
