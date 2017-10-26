#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/text/glyph_atlas.hpp>

namespace mbgl {

using namespace style;

SymbolBucket::SymbolBucket(style::SymbolLayoutProperties::PossiblyEvaluated layout_,
                           const std::map<std::string, std::pair<
                               style::IconPaintProperties::PossiblyEvaluated,
                               style::TextPaintProperties::PossiblyEvaluated>>& layerPaintProperties,
                           const style::DataDrivenPropertyValue<float>& textSize,
                           const style::DataDrivenPropertyValue<float>& iconSize,
                           float zoom,
                           bool sdfIcons_,
                           bool iconsNeedLinear_,
                           const std::vector<SymbolInstance>& symbolInstances_)
    : layout(std::move(layout_)),
      sdfIcons(sdfIcons_),
      iconsNeedLinear(iconsNeedLinear_ || iconSize.isDataDriven() || !iconSize.isZoomConstant()),
      symbolInstances(symbolInstances_), // TODO maybe not copy
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
        if (!staticUploaded) {
            text.indexBuffer = context.createIndexBuffer(std::move(text.triangles));
            text.vertexBuffer = context.createVertexBuffer(std::move(text.vertices));
        }
        if (!dynamicUploaded) {
            text.dynamicVertexBuffer = context.createVertexBuffer(std::move(text.dynamicVertices), gl::BufferUsage::StreamDraw);
        }
        if (!opacityUploaded) {
            if (!text.opacityVertexBuffer) {
                text.opacityVertexBuffer = context.createVertexBuffer(std::move(text.opacityVertices), gl::BufferUsage::StreamDraw);
            } else {
                context.updateVertexBuffer(*text.opacityVertexBuffer, std::move(text.opacityVertices));
            }
        }
    }

    if (hasIconData()) {
        if (!staticUploaded) {
            icon.indexBuffer = context.createIndexBuffer(std::move(icon.triangles));
            icon.vertexBuffer = context.createVertexBuffer(std::move(icon.vertices));
        }
        if (!dynamicUploaded) {
            icon.dynamicVertexBuffer = context.createVertexBuffer(std::move(icon.dynamicVertices), gl::BufferUsage::StreamDraw);
        }
        if (!opacityUploaded) {
            if (!icon.opacityVertexBuffer) {
                icon.opacityVertexBuffer = context.createVertexBuffer(std::move(icon.opacityVertices), gl::BufferUsage::StreamDraw);
            } else {
                context.updateVertexBuffer(*icon.opacityVertexBuffer, std::move(icon.opacityVertices));
            }
        }
    }

    if (hasCollisionBoxData()) {
        if (!staticUploaded) {
            collisionBox.indexBuffer = context.createIndexBuffer(std::move(collisionBox.lines));
            collisionBox.vertexBuffer = context.createVertexBuffer(std::move(collisionBox.vertices));
        }
        if (!opacityUploaded) {
            if (!collisionBox.opacityVertexBuffer) {
                collisionBox.opacityVertexBuffer = context.createVertexBuffer(std::move(collisionBox.opacityVertices), gl::BufferUsage::StreamDraw);
            } else {
                context.updateVertexBuffer(*collisionBox.opacityVertexBuffer, std::move(collisionBox.opacityVertices));
            }
        }
    }
    
    if (hasCollisionCircleData()) {
        if (!staticUploaded) {
            collisionCircle.indexBuffer = context.createIndexBuffer(std::move(collisionCircle.triangles));
            collisionCircle.vertexBuffer = context.createVertexBuffer(std::move(collisionCircle.vertices));
        }
        if (!opacityUploaded) {
            if (!collisionCircle.opacityVertexBuffer) {
                collisionCircle.opacityVertexBuffer = context.createVertexBuffer(std::move(collisionCircle.opacityVertices), gl::BufferUsage::StreamDraw);
            } else {
                context.updateVertexBuffer(*collisionCircle.opacityVertexBuffer, std::move(collisionCircle.opacityVertices));
            }
        }
    }

    if (!staticUploaded) {
        for (auto& pair : paintPropertyBinders) {
            pair.second.first.upload(context);
            pair.second.second.upload(context);
        }
    }

    uploaded = true;
    staticUploaded = true;
    opacityUploaded = true;
    dynamicUploaded = true;
}

bool SymbolBucket::hasData() const {
    return hasTextData() || hasIconData() || hasCollisionBoxData();
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

bool SymbolBucket::hasCollisionCircleData() const {
    return !collisionCircle.segments.empty();
}

void SymbolBucket::updateOpacity() {
    opacityUploaded = false;
    uploaded = false;
}

} // namespace mbgl
