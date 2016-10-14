// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {
namespace style {

class CascadeParameters;
class CalculationParameters;

class TerrainPaintProperties {
public:
    void cascade(const CascadeParameters&);
    bool recalculate(const CalculationParameters&);

    PaintProperty<Color> terrainShadowColor { { 0, 0, 1, 1 } };
    PaintProperty<Color> terrainHighlightColor { { 1, 1, 0, 1 } };
    PaintProperty<Color> terrainAccentColor { Color::black() };
    PaintProperty<float> terrainIlluminationDirection { 135 };
    PaintProperty<AlignmentType> terrainIlluminationAlignment { AlignmentType::Viewport };
    PaintProperty<float> terrainExaggeration { 1 };
};

} // namespace style
} // namespace mbgl
