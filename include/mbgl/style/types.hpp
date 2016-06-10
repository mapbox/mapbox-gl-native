#pragma once

#include <cstdint>

namespace mbgl {

// TODO: should be in public source.hpp header and style namespace
enum class SourceType : uint8_t {
    Vector,
    Raster,
    GeoJSON,
    Video,
    Annotations
};

namespace style {

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

enum class TranslateAnchorType : bool {
    Map,
    Viewport
};

enum class RotateAnchorType : bool {
    Map,
    Viewport,
};

enum class SymbolPlacementType : bool {
    Point,
    Line,
};

enum class RotationAlignmentType : bool {
    Map,
    Viewport,
};

enum class TextJustifyType : uint8_t {
    Center,
    Left,
    Right
};

enum class TextAnchorType : uint8_t {
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

} // namespace style
} // namespace mbgl
