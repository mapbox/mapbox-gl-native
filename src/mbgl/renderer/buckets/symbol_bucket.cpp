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
                           bool sortFeaturesByY_,
                           const std::vector<SymbolInstance>& symbolInstances_)
    : layout(std::move(layout_)),
      sdfIcons(sdfIcons_),
      iconsNeedLinear(iconsNeedLinear_ || iconSize.isDataDriven() || !iconSize.isZoomConstant()),
      sortFeaturesByY(sortFeaturesByY_),
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
            text.indexBuffer = context.createIndexBuffer(std::move(text.triangles), sortFeaturesByY ? gl::BufferUsage::StreamDraw : gl::BufferUsage::StaticDraw);
            text.vertexBuffer = context.createVertexBuffer(std::move(text.vertices));
        } else if (!sortUploaded) {
            context.updateIndexBuffer(*text.indexBuffer, std::move(text.triangles));
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
            icon.indexBuffer = context.createIndexBuffer(std::move(icon.triangles), sortFeaturesByY ? gl::BufferUsage::StreamDraw : gl::BufferUsage::StaticDraw);
            icon.vertexBuffer = context.createVertexBuffer(std::move(icon.vertices));
        } else if (!sortUploaded) {
            context.updateIndexBuffer(*icon.indexBuffer, std::move(icon.triangles));
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
    sortUploaded = true;
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

void SymbolBucket::sortFeatures(const float angle) {
    if (!sortFeaturesByY) {
        return;
    }

    if (sortedAngle == angle) {
        return;
    }
    
    sortedAngle = angle;

    // The current approach to sorting doesn't sort across segments so don't try.
    // Sorting within segments separately seemed not to be worth the complexity.
    if (text.segments.size() > 1 || icon.segments.size() > 1) {
        return;
    }
    
    sortUploaded = false;
    uploaded = false;

    // If the symbols are allowed to overlap sort them by their vertical screen position.
    // The index array buffer is rewritten to reference the (unchanged) vertices in the
    // sorted order.

    // To avoid sorting the actual symbolInstance array we sort an array of indexes.
    std::vector<size_t> symbolInstanceIndexes;
    for (size_t i = 0; i < symbolInstances.size(); i++) {
        symbolInstanceIndexes.push_back(i);
    }
    
    const float sin = std::sin(angle);
    const float cos = std::cos(angle);

    std::sort(symbolInstanceIndexes.begin(), symbolInstanceIndexes.end(), [sin, cos, this](size_t &aIndex, size_t &bIndex) {
        const SymbolInstance& a = symbolInstances[aIndex];
        const SymbolInstance& b = symbolInstances[bIndex];
        const int32_t aRotated = sin * a.anchor.point.x + cos * a.anchor.point.y;
        const int32_t bRotated = sin * b.anchor.point.x + cos * b.anchor.point.y;
        return aRotated != bRotated ?
            aRotated < bRotated :
            a.index > b.index;
    });

    text.triangles.clear();
    icon.triangles.clear();

    for (auto i : symbolInstanceIndexes) {
        const SymbolInstance& symbolInstance = symbolInstances[i];

        for (auto& placedTextSymbolIndex : symbolInstance.placedTextIndices) {
            const PlacedSymbol& placedSymbol = text.placedSymbols[placedTextSymbolIndex];

            auto endIndex = placedSymbol.vertexStartIndex + placedSymbol.glyphOffsets.size() * 4;
            for (auto vertexIndex = placedSymbol.vertexStartIndex; vertexIndex < endIndex; vertexIndex += 4) {
                text.triangles.emplace_back(vertexIndex + 0, vertexIndex + 1, vertexIndex + 2);
                text.triangles.emplace_back(vertexIndex + 1, vertexIndex + 2, vertexIndex + 3);
            }
        }

        for (auto& placedIconSymbolIndex : symbolInstance.placedIconIndices) { // TODO: This iteration is an awkward way to say "if the symbol has an icon"
            const PlacedSymbol& placedIcon = icon.placedSymbols[placedIconSymbolIndex];
            
            const auto vertexIndex = placedIcon.vertexStartIndex;
            icon.triangles.emplace_back(vertexIndex + 0, vertexIndex + 1, vertexIndex + 2);
            icon.triangles.emplace_back(vertexIndex + 1, vertexIndex + 2, vertexIndex + 3);
        }
    }
}

} // namespace mbgl
