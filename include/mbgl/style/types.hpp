#pragma once

#include <mbgl/util/enum.hpp>

#include <string>
#include <array>
#include <vector>
#include <utility>

namespace mbgl {

// Stores a premultiplied color, with all four channels ranging from 0..1
using Color = std::array<float, 4>;

// An array of font names
using FontStack = std::vector<std::string>;

std::string fontStackToString(const FontStack&);

struct FontStackHash {
    std::size_t operator()(const FontStack&) const;
};

template <typename T>
class Function {
public:
    using Stop = std::pair<float, T>;
    using Stops = std::vector<Stop>;

    explicit Function(const Stops& stops_, float base_)
        : base(base_), stops(stops_) {}

    float getBase() const { return base; }
    const std::vector<std::pair<float, T>>& getStops() const { return stops; }

private:
    float base = 1;
    std::vector<std::pair<float, T>> stops;
};

// -------------------------------------------------------------------------------------------------

enum class SourceType : uint8_t {
    Vector,
    Raster,
    GeoJSON,
    Video,
    Annotations
};

MBGL_DEFINE_ENUM_CLASS(SourceTypeClass, SourceType, {
    { SourceType::Vector, "vector" },
    { SourceType::Raster, "raster" },
    { SourceType::GeoJSON, "geojson" },
    { SourceType::Video, "video" },
    { SourceType::Annotations, "annotations" },
});

// -------------------------------------------------------------------------------------------------

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

} // namespace mbgl

