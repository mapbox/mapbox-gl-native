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
    Raster
};

enum class WindingType : uint8_t {
    EvenOdd,
    NonZero
};

enum class CapType : uint8_t {
    None,
    Round,
    Butt,
    Square
};

enum class JoinType : uint8_t {
    None,
    Miter,
    Bevel,
    Round
};

enum class TextPathType : uint8_t {
    Horizontal,
    Curve
};

enum class TranslateAnchorType : uint8_t {
    Map,
    Viewport
};

enum class RotateAnchorType : uint8_t {
    Map,
    Viewport
};

enum class SourceType : uint8_t {
    Vector,
    Raster,
    GeoJSON,
    Video
};

inline constexpr BucketType parseBucketType(const std::string &type = "") {
    return
        type.compare("fill") == 0 ? BucketType::Fill :
        type.compare("line") == 0 ? BucketType::Line :
        type.compare("icon") == 0 ? BucketType::Icon :
        type.compare("text") == 0 ? BucketType::Text :
        type.compare("raster") == 0 ? BucketType::Raster :
        BucketType::None;
}

inline constexpr WindingType parseWindingType(const std::string &type = "") {
    return
        type.compare("even-odd") == 0 ? WindingType::EvenOdd :
        WindingType::NonZero;
}

inline constexpr CapType parseCapType(const std::string &cap = "") {
    return
        cap.compare("round") == 0 ? CapType::Round :
        cap.compare("butt") == 0 ? CapType::Butt :
        cap.compare("square") == 0 ? CapType::Square :
        CapType::None;
}

inline constexpr JoinType parseJoinType(const std::string &join = "") {
    return
        join.compare("miter") == 0 ? JoinType::Miter :
        join.compare("bevel") == 0 ? JoinType::Bevel :
        join.compare("round") == 0 ? JoinType::Round :
        JoinType::None;
}

inline constexpr TextPathType parseTextPathType(const std::string &path = "") {
    return
        path.compare("horizontal") == 0 ? TextPathType::Horizontal :
        path.compare("curve") == 0 ? TextPathType::Curve :
        TextPathType::Horizontal;
}

inline constexpr TranslateAnchorType parseTranslateAnchorType(const std::string &anchor = "") {
    return
        anchor.compare("viewport") == 0 ? TranslateAnchorType::Viewport :
        TranslateAnchorType::Map;
}

inline constexpr RotateAnchorType parseRotateAnchorType(const std::string &anchor = "") {
    return
        anchor.compare("map") == 0 ? RotateAnchorType::Map :
        RotateAnchorType::Viewport;
}

inline constexpr float parseAlignmentType(const std::string &alignment = "") {
    return
        alignment.compare("right") == 0 ? 1.0f :
        alignment.compare("left") == 0 ? 0.0f :
        0.5f;
};

inline constexpr float parseVerticalAlignmentType(const std::string &alignment = "") {
    return
        alignment.compare("bottom") == 0 ? 1.0f :
        alignment.compare("top") == 0 ? 0.0f :
        0.5f;
};

inline constexpr SourceType parseSourceType(const std::string &source = "") {
    return
        source.compare("vector") == 0 ? SourceType::Vector :
        source.compare("raster") == 0 ? SourceType::Raster :
        source.compare("geojson") == 0 ? SourceType::GeoJSON :
        source.compare("video") == 0 ? SourceType::Video :
        SourceType::Vector;
}


}

#endif

