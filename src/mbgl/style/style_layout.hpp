#ifndef MBGL_STYLE_STYLE_LAYOUT
#define MBGL_STYLE_STYLE_LAYOUT

#include <mbgl/style/types.hpp>

namespace mbgl {

class Source;

class StyleLayoutFill {
public:
    // Make movable only.
    StyleLayoutFill() = default;
    StyleLayoutFill(StyleLayoutFill &&) = default;
    StyleLayoutFill& operator=(StyleLayoutFill &&) = default;
    StyleLayoutFill(const StyleLayoutFill &) = delete;
    StyleLayoutFill& operator=(const StyleLayoutFill &) = delete;
};

class StyleLayoutLine {
public:
    // Make movable only.
    StyleLayoutLine() = default;
    StyleLayoutLine(StyleLayoutLine &&) = default;
    StyleLayoutLine& operator=(StyleLayoutLine &&) = default;
    StyleLayoutLine(const StyleLayoutLine &) = delete;
    StyleLayoutLine& operator=(const StyleLayoutLine &) = delete;

    CapType cap = CapType::Butt;
    JoinType join = JoinType::Miter;
    float miter_limit = 2.0f;
    float round_limit = 1.0f;
};

class StyleLayoutSymbol {
public:
    // Make movable only.
    StyleLayoutSymbol() = default;
    StyleLayoutSymbol(StyleLayoutSymbol &&) = default;
    StyleLayoutSymbol& operator=(StyleLayoutSymbol &&) = default;
    StyleLayoutSymbol(const StyleLayoutSymbol &) = delete;
    StyleLayoutSymbol& operator=(const StyleLayoutSymbol &) = delete;

    PlacementType placement = PlacementType::Point;
    float spacing = 250.0f;
    bool avoid_edges = false;

    struct {
        bool allow_overlap = false;
        bool ignore_placement = false;
        bool optional = false;
        RotationAlignmentType rotation_alignment = RotationAlignmentType::Viewport;
        float size = 1.0f;
        float max_size = 1.0f;
        std::string image;
        float rotate = 0.0f;
        float padding = 2.0f;
        bool keep_upright = false;
        std::array<float, 2> offset = {{ 0, 0 }};
    } icon;

    struct {
        RotationAlignmentType rotation_alignment = RotationAlignmentType::Viewport;
        std::string field;
        std::string font = "Open Sans Regular, Arial Unicode MS Regular";
        float size = 16.0f;
        float max_size = 16.0f;
        float max_width = 15.0f /* em */;
        float line_height = 1.2f /* em */;
        float letter_spacing = 0.0f /* em */;
        TextJustifyType justify = TextJustifyType::Center;
        TextAnchorType anchor = TextAnchorType::Center;
        float max_angle = 45.0f /* degrees */;
        float rotate = 0.0f;
        float padding = 2.0f;
        bool keep_upright = true;
        TextTransformType transform = TextTransformType::None;
        std::array<float, 2> offset = {{ 0, 0 }};
        bool allow_overlap = false;
        bool ignore_placement = false;
        bool optional = false;
    } text;
};

class StyleLayoutRaster {
public:
    // Make movable only.
    StyleLayoutRaster() = default;
    StyleLayoutRaster(StyleLayoutRaster &&) = default;
    StyleLayoutRaster& operator=(StyleLayoutRaster &&) = default;
    StyleLayoutRaster(const StyleLayoutRaster &) = delete;
    StyleLayoutRaster& operator=(const StyleLayoutRaster &) = delete;
};

class StyleLayoutBackground {
public:
    // Make movable only.
    StyleLayoutBackground() = default;
    StyleLayoutBackground(StyleLayoutBackground &&) = default;
    StyleLayoutBackground& operator=(StyleLayoutBackground &&) = default;
    StyleLayoutBackground(const StyleLayoutBackground &) = delete;
    StyleLayoutBackground& operator=(const StyleLayoutBackground &) = delete;
};

template <typename T>
const T &defaultStyleLayout();

};

#endif
