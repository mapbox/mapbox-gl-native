#pragma once

#include <cstdint>

namespace mbgl {

namespace style {

// TODO: should be in public source.hpp header
enum class SourceType : uint8_t {
    Vector,
    Raster,
    RasterDEM,
    GeoJSON,
    Video,
    Annotations,
    Image,
    CustomVector
};

enum class VisibilityType : bool {
    Visible,
    None,
};

enum class LineCapType : uint8_t {
    Round,
    Butt,
    Square,
};

enum class LineJoinType : uint8_t {
    Miter,
    Bevel,
    Round,
    // the following two types are for internal use only
    FakeRound,
    FlipBevel
};

enum class HillshadeIlluminationAnchorType : bool {
    Map,
    Viewport
};

enum class TranslateAnchorType : bool {
    Map,
    Viewport
};

enum class RotateAnchorType : bool {
    Map,
    Viewport,
};

enum class CirclePitchScaleType : bool {
    Map,
    Viewport,
};

enum class SymbolPlacementType : bool {
    Point,
    Line,
};

enum class AlignmentType : uint8_t {
    Map,
    Viewport,
    Auto,
};

enum class TextJustifyType : uint8_t {
    Center,
    Left,
    Right
};

enum class SymbolAnchorType : uint8_t {
    Center,
    Left,
    Right,
    Top,
    Bottom,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

enum class TextTransformType : uint8_t {
    None,
    Uppercase,
    Lowercase,
};

enum class IconTextFitType : uint8_t {
    None,
    Both,
    Width,
    Height
};

enum class LightAnchorType: bool {
    Map,
    Viewport
};

} // namespace style
} // namespace mbgl
