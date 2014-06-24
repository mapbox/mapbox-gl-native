#ifndef LLMR_STYLE_TYPES
#define LLMR_STYLE_TYPES

#include <string>

namespace llmr {

enum class BucketType : uint8_t {
    None,
    Fill,
    Line,
    Icon,
    Text,
    Raster,
    Default = None
};

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

inline BucketType parseBucketType(const std::string &type) {
    if (type == "fill") return BucketType::Fill;
    if (type == "line") return BucketType::Line;
    if (type == "icon") return BucketType::Icon;
    if (type == "text") return BucketType::Text;
    if (type == "raster") return BucketType::Raster;
    return BucketType::None;
}

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

