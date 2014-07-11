#ifndef LLMR_STYLE_TYPES
#define LLMR_STYLE_TYPES

#include <string>
#include <array>

namespace llmr {

// Stores a premultiplied color, with all four channels ranging from 0..1
typedef std::array<float, 4> Color;

enum class StyleLayerType : uint8_t {
    Unknown,
    Fill,
    Line,
    Icon,
    Text,
    Raster,
    Composite,
    Background
};

StyleLayerType parseStyleLayerType(const std::string &type);
const char *stringifyStyleLayerType(StyleLayerType type);
std::ostream& operator<<(std::ostream&, StyleLayerType type);


enum class WindingType : uint8_t {
    EvenOdd,
    NonZero,
    Default = NonZero
};

enum class CapType : uint8_t {
    None,
    Round,
    Butt,
    Square,
    Default = None
};

enum class JoinType : uint8_t {
    None,
    Miter,
    Bevel,
    Round,
    Default = None
};

enum class TextPathType : uint8_t {
    Horizontal,
    Curve,
    Default = Horizontal
};

enum class TextTransformType : uint8_t {
    None,
    Uppercase,
    Lowercase,
    Default = None
};

enum class TranslateAnchorType : uint8_t {
    Map,
    Viewport,
    Default = Map
};

enum class RotateAnchorType : uint8_t {
    Map,
    Viewport,
    Default = Viewport
};

enum class SourceType : uint8_t {
    Vector,
    Raster,
    GeoJSON,
    Video,
    Default = Vector
};

inline WindingType parseWindingType(const std::string &type) {
    if (type == "even-odd") return WindingType::EvenOdd;
    if (type == "non-zero") return WindingType::NonZero;
    return WindingType::Default;
}

inline CapType parseCapType(const std::string &cap) {
    if (cap == "round") return CapType::Round;
    if (cap == "butt") return CapType::Butt;
    if (cap == "square") return CapType::Square;
    return CapType::None;
}

inline JoinType parseJoinType(const std::string &join) {
    if (join == "miter") return JoinType::Miter;
    if (join == "bevel") return JoinType::Bevel;
    if (join == "round") return JoinType::Round;
    return JoinType::None;
}

inline TextPathType parseTextPathType(const std::string &path) {
    if (path == "horizontal") return TextPathType::Horizontal;
    if (path == "curve") return TextPathType::Curve;
    return TextPathType::Default;
}

inline TextTransformType parseTextTransformType(const std::string& transform) {
    if (transform == "uppercase") return TextTransformType::Uppercase;
    if (transform == "lowercase") return TextTransformType::Lowercase;
    return TextTransformType::Default;
};

inline TranslateAnchorType parseTranslateAnchorType(const std::string &anchor) {
    if (anchor == "map") return TranslateAnchorType::Map;
    if (anchor == "viewport") return TranslateAnchorType::Viewport;
    return TranslateAnchorType::Default;
}

inline RotateAnchorType parseRotateAnchorType(const std::string &anchor) {
    if (anchor == "map") return RotateAnchorType::Map;
    if (anchor == "viewport") return RotateAnchorType::Viewport;
    return RotateAnchorType::Default;
}

inline float parseAlignmentType(const std::string &alignment) {
    if (alignment == "right") return 1.0f;
    if (alignment == "left") return 0.0f;
    return 0.5f;
}

inline float parseVerticalAlignmentType(const std::string &alignment) {
    if (alignment == "bottom") return 1.0f;
    if (alignment == "top") return 0.0f;
    return 0.5f;
}

inline SourceType parseSourceType(const std::string &source) {
    if (source == "vector") return SourceType::Vector;
    if (source == "raster") return SourceType::Raster;
    if (source == "geojson") return SourceType::GeoJSON;
    if (source == "video") return SourceType::Video;
    return SourceType::Default;
}

}

#endif

