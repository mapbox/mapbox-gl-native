#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <utility>

namespace mbgl {

using namespace style;

namespace {

const Shaping& getAnyShaping(const ShapedTextOrientations& shapedTextOrientations) {
    if (shapedTextOrientations.right) return shapedTextOrientations.right;
    if (shapedTextOrientations.center) return shapedTextOrientations.center;
    if (shapedTextOrientations.left) return shapedTextOrientations.left;
    if (shapedTextOrientations.vertical) return shapedTextOrientations.vertical;
    return shapedTextOrientations.horizontal;
}

} // namespace

SymbolInstanceSharedData::SymbolInstanceSharedData(GeometryCoordinates line_,
                                                   const ShapedTextOrientations& shapedTextOrientations,
                                                   const optional<PositionedIcon>& shapedIcon,
                                                   const style::SymbolLayoutProperties::Evaluated& layout,
                                                   const float layoutTextSize,
                                                   const style::SymbolPlacementType textPlacement,
                                                   const std::array<float, 2>& textOffset,
                                                   const GlyphPositions& positions) : line(std::move(line_)) {
    // Create the quads used for rendering the icon and glyphs.
    if (shapedIcon) {
        iconQuad = getIconQuad(*shapedIcon, layout, layoutTextSize, shapedTextOrientations.horizontal);
    }

    bool singleLineInitialized = false;
    const auto initHorizontalGlyphQuads = [&] (SymbolQuads& quads, const Shaping& shaping) {
        if (!shapedTextOrientations.singleLine) {
            quads = getGlyphQuads(shaping, textOffset, layout, textPlacement, positions);
            return;
        }
        if (!singleLineInitialized) {
            rightJustifiedGlyphQuads = getGlyphQuads(shaping, textOffset, layout, textPlacement, positions);
            singleLineInitialized = true;
        }
    };

    if (shapedTextOrientations.right) {
        initHorizontalGlyphQuads(rightJustifiedGlyphQuads, shapedTextOrientations.right);
    }

    if (shapedTextOrientations.center) {
        initHorizontalGlyphQuads(centerJustifiedGlyphQuads, shapedTextOrientations.center);
    }

    if (shapedTextOrientations.left) {
        initHorizontalGlyphQuads(leftJustifiedGlyphQuads, shapedTextOrientations.left);
    }

    if (shapedTextOrientations.vertical) {
        verticalGlyphQuads = getGlyphQuads(shapedTextOrientations.vertical, textOffset, layout, textPlacement, positions);
    }
}

bool SymbolInstanceSharedData::empty() const {
    return rightJustifiedGlyphQuads.empty() && centerJustifiedGlyphQuads.empty() && leftJustifiedGlyphQuads.empty() && verticalGlyphQuads.empty();
}

SymbolInstance::SymbolInstance(Anchor& anchor_,
                               std::shared_ptr<SymbolInstanceSharedData> sharedData_,
                               const ShapedTextOrientations& shapedTextOrientations,
                               const optional<PositionedIcon>& shapedIcon,
                               const float textBoxScale_,
                               const float textPadding,
                               const SymbolPlacementType textPlacement,
                               const std::array<float, 2>& textOffset_,
                               const float iconBoxScale,
                               const float iconPadding,
                               const std::array<float, 2>& iconOffset_,
                               const IndexedSubfeature& indexedFeature,
                               const std::size_t layoutFeatureIndex_,
                               const std::size_t dataFeatureIndex_,
                               std::u16string key_,
                               const float overscaling,
                               const float rotate,
                               float radialTextOffset_) :
    sharedData(std::move(sharedData_)),
    anchor(anchor_),
    // 'hasText' depends on finding at least one glyph in the shaping that's also in the GlyphPositionMap
    hasText(!sharedData->empty()),
    hasIcon(shapedIcon),
    // Create the collision features that will be used to check whether this symbol instance can be placed
    // As a collision approximation, we can use either the vertical or any of the horizontal versions of the feature
    textCollisionFeature(sharedData->line, anchor, getAnyShaping(shapedTextOrientations), textBoxScale_, textPadding, textPlacement, indexedFeature, overscaling, rotate),
    iconCollisionFeature(sharedData->line, anchor, shapedIcon, iconBoxScale, iconPadding, indexedFeature, rotate),
    writingModes(WritingModeType::None),
    layoutFeatureIndex(layoutFeatureIndex_),
    dataFeatureIndex(dataFeatureIndex_),
    textOffset(textOffset_),
    iconOffset(iconOffset_),
    key(std::move(key_)),
    textBoxScale(textBoxScale_),
    radialTextOffset(radialTextOffset_),
    singleLine(shapedTextOrientations.singleLine) {

    rightJustifiedGlyphQuadsSize = sharedData->rightJustifiedGlyphQuads.size();
    centerJustifiedGlyphQuadsSize = sharedData->centerJustifiedGlyphQuads.size();
    leftJustifiedGlyphQuadsSize = sharedData->leftJustifiedGlyphQuads.size();
    verticalGlyphQuadsSize = sharedData->verticalGlyphQuads.size();

    if (rightJustifiedGlyphQuadsSize || centerJustifiedGlyphQuadsSize || leftJustifiedGlyphQuadsSize) {
        writingModes |= WritingModeType::Horizontal;
    }

    if (verticalGlyphQuadsSize) {
        writingModes |= WritingModeType::Vertical;
    }
}

const GeometryCoordinates& SymbolInstance::line() const {
    assert(sharedData);
    return sharedData->line;
}

const SymbolQuads& SymbolInstance::rightJustifiedGlyphQuads() const {
    assert(sharedData);
    return sharedData->rightJustifiedGlyphQuads;
}

const SymbolQuads& SymbolInstance::leftJustifiedGlyphQuads() const {
    assert(sharedData);
    return sharedData->leftJustifiedGlyphQuads;
}

const SymbolQuads& SymbolInstance::centerJustifiedGlyphQuads() const {
    assert(sharedData);
    return sharedData->centerJustifiedGlyphQuads;
}

const SymbolQuads& SymbolInstance::verticalGlyphQuads() const {
    assert(sharedData);
    return sharedData->verticalGlyphQuads;
}

const optional<SymbolQuad>& SymbolInstance::iconQuad() const {
    assert(sharedData);
    return sharedData->iconQuad;
}

void SymbolInstance::releaseSharedData() {
    sharedData.reset();
}

optional<size_t> SymbolInstance::getDefaultHorizontalPlacedTextIndex() const {
    if (placedRightTextIndex) return placedRightTextIndex;
    if (placedCenterTextIndex) return placedCenterTextIndex;
    if (placedLeftTextIndex) return placedLeftTextIndex;
    return nullopt;
}
} // namespace mbgl
