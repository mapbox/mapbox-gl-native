#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/text/cross_tile_symbol_index.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/placement.hpp>

namespace mbgl {

using namespace style;
namespace {
std::atomic<uint32_t> maxBucketInstanceId;
} // namespace

SymbolBucket::SymbolBucket(Immutable<style::SymbolLayoutProperties::PossiblyEvaluated> layout_,
                           const std::map<std::string, Immutable<style::LayerProperties>>& paintProperties_,
                           const style::PropertyValue<float>& textSize,
                           const style::PropertyValue<float>& iconSize,
                           float zoom,
                           bool iconsNeedLinear_,
                           bool sortFeaturesByY_,
                           const std::string bucketName_,
                           const std::vector<SymbolInstance>&& symbolInstances_,
                           float tilePixelRatio_,
                           bool allowVerticalPlacement_,
                           std::vector<style::TextWritingModeType> placementModes_,
                           bool iconsInText_)
    : layout(std::move(layout_)),
      bucketLeaderID(bucketName_),
      iconsNeedLinear(iconsNeedLinear_ || iconSize.isDataDriven() || !iconSize.isZoomConstant()),
      sortFeaturesByY(sortFeaturesByY_),
      staticUploaded(false),
      placementChangesUploaded(false),
      dynamicUploaded(false),
      sortUploaded(false),
      iconsInText(iconsInText_),
      justReloaded(false),
      hasVariablePlacement(false),
      symbolInstances(symbolInstances_),
      textSizeBinder(SymbolSizeBinder::create(zoom, textSize, TextSize::defaultValue())),
      iconSizeBinder(SymbolSizeBinder::create(zoom, iconSize, IconSize::defaultValue())),
      tilePixelRatio(tilePixelRatio_),
      bucketInstanceId(++maxBucketInstanceId),
      allowVerticalPlacement(allowVerticalPlacement_),
      placementModes(std::move(placementModes_)) {
    for (const auto& pair : paintProperties_) {
        const auto& evaluated = getEvaluated<SymbolLayerProperties>(pair.second);
        paintProperties.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(pair.first),
            std::forward_as_tuple(PaintProperties {
                { RenderSymbolLayer::iconPaintProperties(evaluated), zoom },
                { RenderSymbolLayer::textPaintProperties(evaluated), zoom }
            }));
    }
}

SymbolBucket::~SymbolBucket() = default;

void SymbolBucket::upload(gfx::UploadPass& uploadPass) {
    if (hasTextData()) {
        if (!staticUploaded) {
            text.indexBuffer = uploadPass.createIndexBuffer(std::move(text.triangles), sortFeaturesByY ? gfx::BufferUsageType::StreamDraw : gfx::BufferUsageType::StaticDraw);
            text.vertexBuffer = uploadPass.createVertexBuffer(std::move(text.vertices));
            for (auto& pair : paintProperties) {
                pair.second.textBinders.upload(uploadPass);
            }
        } else if (!sortUploaded) {
            uploadPass.updateIndexBuffer(*text.indexBuffer, std::move(text.triangles));
        }

        if (!dynamicUploaded) {
            if (!text.dynamicVertexBuffer) {
                text.dynamicVertexBuffer = uploadPass.createVertexBuffer(std::move(text.dynamicVertices), gfx::BufferUsageType::StreamDraw);
            } else {
                uploadPass.updateVertexBuffer(*text.dynamicVertexBuffer, std::move(text.dynamicVertices));
            }
        }
        if (!placementChangesUploaded) {
            if (!text.opacityVertexBuffer) {
                text.opacityVertexBuffer = uploadPass.createVertexBuffer(std::move(text.opacityVertices), gfx::BufferUsageType::StreamDraw);
            } else {
                uploadPass.updateVertexBuffer(*text.opacityVertexBuffer, std::move(text.opacityVertices));
            }
        }
    }

    auto updateIconBuffer = [&](Buffer& iconBuffer) {
        if (!staticUploaded) {
            iconBuffer.indexBuffer = uploadPass.createIndexBuffer(std::move(iconBuffer.triangles), sortFeaturesByY ? gfx::BufferUsageType::StreamDraw : gfx::BufferUsageType::StaticDraw);
            iconBuffer.vertexBuffer = uploadPass.createVertexBuffer(std::move(iconBuffer.vertices));
            for (auto& pair : paintProperties) {
                pair.second.iconBinders.upload(uploadPass);
            }
        } else if (!sortUploaded) {
            uploadPass.updateIndexBuffer(*iconBuffer.indexBuffer, std::move(iconBuffer.triangles));
        }
        if (!dynamicUploaded) {
            if (!iconBuffer.dynamicVertexBuffer) {
                iconBuffer.dynamicVertexBuffer = uploadPass.createVertexBuffer(std::move(iconBuffer.dynamicVertices), gfx::BufferUsageType::StreamDraw);
            } else {
                uploadPass.updateVertexBuffer(*iconBuffer.dynamicVertexBuffer, std::move(iconBuffer.dynamicVertices));
            }
        }
        if (!placementChangesUploaded) {
            if (!iconBuffer.opacityVertexBuffer) {
                iconBuffer.opacityVertexBuffer = uploadPass.createVertexBuffer(std::move(iconBuffer.opacityVertices), gfx::BufferUsageType::StreamDraw);
            } else {
                uploadPass.updateVertexBuffer(*iconBuffer.opacityVertexBuffer, std::move(iconBuffer.opacityVertices));
            }
        }
    };
    if (hasIconData()) {
        updateIconBuffer(icon);
    }
    if (hasSdfIconData()) {
        updateIconBuffer(sdfIcon);
    }

    const auto updateCollisionBox = [&](CollisionBoxBuffer& collisionBox) {
        if (!staticUploaded) {
            collisionBox.indexBuffer = uploadPass.createIndexBuffer(std::move(collisionBox.lines));
            collisionBox.vertexBuffer = uploadPass.createVertexBuffer(std::move(collisionBox.vertices));
        }
        if (!placementChangesUploaded) {
            if (!collisionBox.dynamicVertexBuffer) {
                collisionBox.dynamicVertexBuffer = uploadPass.createVertexBuffer(std::move(collisionBox.dynamicVertices), gfx::BufferUsageType::StreamDraw);
            } else {
                uploadPass.updateVertexBuffer(*collisionBox.dynamicVertexBuffer, std::move(collisionBox.dynamicVertices));
            }
        }
    };
    if (hasIconCollisionBoxData()) {
        updateCollisionBox(*iconCollisionBox);
    }

    if (hasTextCollisionBoxData()) {
        updateCollisionBox(*textCollisionBox);
    }

    const auto updateCollisionCircle = [&](CollisionCircleBuffer& collisionCircle) {
        if (!staticUploaded) {
            collisionCircle.indexBuffer = uploadPass.createIndexBuffer(std::move(collisionCircle.triangles));
            collisionCircle.vertexBuffer = uploadPass.createVertexBuffer(std::move(collisionCircle.vertices));
        }
        if (!placementChangesUploaded) {
            if (!collisionCircle.dynamicVertexBuffer) {
                collisionCircle.dynamicVertexBuffer = uploadPass.createVertexBuffer(std::move(collisionCircle.dynamicVertices), gfx::BufferUsageType::StreamDraw);
            } else {
                uploadPass.updateVertexBuffer(*collisionCircle.dynamicVertexBuffer, std::move(collisionCircle.dynamicVertices));
            }
        }
    };
    if (hasIconCollisionCircleData()) {
        updateCollisionCircle(*iconCollisionCircle);
    }

    if (hasTextCollisionCircleData()) {
        updateCollisionCircle(*textCollisionCircle);
    }

    uploaded = true;
    staticUploaded = true;
    placementChangesUploaded = true;
    dynamicUploaded = true;
    sortUploaded = true;
}

bool SymbolBucket::hasData() const {
    return hasTextData() || hasIconData() || hasSdfIconData() || hasIconCollisionBoxData() ||
           hasTextCollisionBoxData() || hasIconCollisionCircleData() || hasTextCollisionCircleData();
}

bool SymbolBucket::hasTextData() const {
    return !text.segments.empty();
}

bool SymbolBucket::hasIconData() const {
    return !icon.segments.empty();
}

bool SymbolBucket::hasSdfIconData() const {
    return !sdfIcon.segments.empty();
}

bool SymbolBucket::hasIconCollisionBoxData() const {
    return iconCollisionBox && !iconCollisionBox->segments.empty();
}

bool SymbolBucket::hasIconCollisionCircleData() const {
    return iconCollisionCircle && !iconCollisionCircle->segments.empty();
}

bool SymbolBucket::hasTextCollisionBoxData() const {
    return textCollisionBox && !textCollisionBox->segments.empty();
}

bool SymbolBucket::hasTextCollisionCircleData() const {
    return textCollisionCircle && !textCollisionCircle->segments.empty();
}

void addPlacedSymbol(gfx::IndexVector<gfx::Triangles>& triangles, const PlacedSymbol& placedSymbol) {
    auto endIndex = placedSymbol.vertexStartIndex + placedSymbol.glyphOffsets.size() * 4;
    for (auto vertexIndex = placedSymbol.vertexStartIndex; vertexIndex < endIndex; vertexIndex += 4) {
        triangles.emplace_back(vertexIndex + 0, vertexIndex + 1, vertexIndex + 2);
        triangles.emplace_back(vertexIndex + 1, vertexIndex + 2, vertexIndex + 3);
    }
}

void SymbolBucket::sortFeatures(const float angle) {
    if (!sortFeaturesByY) {
        return;
    }
    assert(angle != std::numeric_limits<float>::max());

    if (sortedAngle == angle) {
        return;
    }

    sortedAngle = angle;

    // The current approach to sorting doesn't sort across text and icon segments so don't try.
    // Sorting within segments separately seemed not to be worth the complexity.
    if (text.segments.size() > 1 || (icon.segments.size() > 1 || sdfIcon.segments.size() > 1)) {
        return;
    }

    sortUploaded = false;
    uploaded = false;

    text.triangles.clear();
    icon.triangles.clear();
    sdfIcon.triangles.clear();

    auto symbolsSortOrder = std::make_unique<std::vector<size_t>>();
    symbolsSortOrder->reserve(symbolInstances.size());

    // If the symbols are allowed to overlap sort them by their vertical screen position.
    // The index array buffer is rewritten to reference the (unchanged) vertices in the
    // sorted order.
    for (const SymbolInstance& symbolInstance : getSortedSymbols(angle)) {
        symbolsSortOrder->push_back(symbolInstance.dataFeatureIndex);

        if (symbolInstance.placedRightTextIndex) {
            addPlacedSymbol(text.triangles, text.placedSymbols[*symbolInstance.placedRightTextIndex]);
        }

        if (symbolInstance.placedCenterTextIndex && !symbolInstance.singleLine) {
            addPlacedSymbol(text.triangles, text.placedSymbols[*symbolInstance.placedCenterTextIndex]);
        }

        if (symbolInstance.placedLeftTextIndex && !symbolInstance.singleLine) {
            addPlacedSymbol(text.triangles, text.placedSymbols[*symbolInstance.placedLeftTextIndex]);
        }

        if (symbolInstance.placedVerticalTextIndex) {
            addPlacedSymbol(text.triangles, text.placedSymbols[*symbolInstance.placedVerticalTextIndex]);
        }

        auto& iconBuffer = symbolInstance.hasSdfIcon() ? sdfIcon : icon;
        if (symbolInstance.placedIconIndex) {
            addPlacedSymbol(iconBuffer.triangles, iconBuffer.placedSymbols[*symbolInstance.placedIconIndex]);
        }

        if (symbolInstance.placedVerticalIconIndex) {
            addPlacedSymbol(iconBuffer.triangles, iconBuffer.placedSymbols[*symbolInstance.placedVerticalIconIndex]);
        }
    }

    featureSortOrder = std::move(symbolsSortOrder);
}

std::vector<std::reference_wrapper<const SymbolInstance>> SymbolBucket::getSortedSymbols(const float angle) const {
    std::vector<std::reference_wrapper<const SymbolInstance>> result(symbolInstances.begin(), symbolInstances.end());
    const float sin = std::sin(angle);
    const float cos = std::cos(angle);

    std::sort(result.begin(), result.end(), [sin, cos](const SymbolInstance& a, const SymbolInstance& b) {
        const auto aRotated = ::lround(sin * a.anchor.point.x + cos * a.anchor.point.y);
        const auto bRotated = ::lround(sin * b.anchor.point.x + cos * b.anchor.point.y);
        if (aRotated != bRotated) {
            return aRotated < bRotated;
        }
        return a.dataFeatureIndex > b.dataFeatureIndex;  // aRotated == bRotated
    });

    return result;
}

bool SymbolBucket::hasFormatSectionOverrides() const {
    if (!hasFormatSectionOverrides_) {
        hasFormatSectionOverrides_= SymbolLayerPaintPropertyOverrides::hasOverrides(layout->get<TextField>());
    }
    return *hasFormatSectionOverrides_;
}

std::pair<uint32_t, bool> SymbolBucket::registerAtCrossTileIndex(CrossTileSymbolLayerIndex& index, const OverscaledTileID& tileID, uint32_t& maxCrossTileID) {
    bool firstTimeAdded = index.addBucket(tileID, *this, maxCrossTileID);
    return std::make_pair(bucketInstanceId, firstTimeAdded);
}

void SymbolBucket::place(Placement& placement, const BucketPlacementParameters& params, std::set<uint32_t>& seenIds) {
    placement.placeBucket(*this, params, seenIds);
}

void SymbolBucket::updateVertices(const Placement& placement,
                                  bool updateOpacities,
                                  const TransformState& state,
                                  const RenderTile& tile,
                                  std::set<uint32_t>& seenIds) {
    if (updateOpacities) {
        placement.updateBucketOpacities(*this, state, seenIds);
        placementChangesUploaded = false;
        uploaded = false;
    }

    if (placement.updateBucketDynamicVertices(*this, state, tile)) {
        dynamicUploaded = false;
        uploaded = false;
    }
}

} // namespace mbgl
