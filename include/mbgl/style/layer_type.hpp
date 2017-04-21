#pragma once

namespace mbgl {
namespace style {

enum class LayerType {
    Fill,
    Line,
    Circle,
    Symbol,
    Raster,
    Background,
    Custom,
    FillExtrusion,
};

} // namespace style
} // namespace mbgl