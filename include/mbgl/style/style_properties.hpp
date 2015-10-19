#ifndef MBGL_STYLE_STYLE_PROPERTIES
#define MBGL_STYLE_STYLE_PROPERTIES

#include <mapbox/variant.hpp>

#include <mbgl/style/types.hpp>

#include <array>
#include <string>
#include <type_traits>
#include <memory>
#include <vector>

namespace mbgl {

class FillPaintProperties {
public:
    bool antialias = true;
    float opacity = 1.0f;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, -1 }};
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchorType translateAnchor = TranslateAnchorType::Map;
    Faded<std::string> image;

    inline bool isVisible() const {
        return opacity > 0 && (fill_color[3] > 0 || stroke_color[3] > 0);
    }
};

class FillLayoutProperties {
public:
};

class LinePaintProperties {
public:
    float opacity = 1.0f;
    Color color = {{ 0, 0, 0, 1 }};
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchorType translateAnchor = TranslateAnchorType::Map;
    float width = 1;
    float gap_width = 0;
    float blur = 0;
    Faded<std::vector<float>> dash_array;
    float dash_line_width = 1;
    Faded<std::string> image;

    inline bool isVisible() const {
        return opacity > 0 && color[3] > 0 && width > 0;
    }
};

class LineLayoutProperties {
public:
    CapType cap = CapType::Butt;
    JoinType join = JoinType::Miter;
    float miter_limit = 2.0f;
    float round_limit = 1.0f;
};

class SymbolPaintProperties {
public:
    struct {
        float opacity = 1.0f;
        float size = 1.0f;
        Color color = {{ 0, 0, 0, 1 }};
        Color halo_color = {{ 0, 0, 0, 0 }};
        float halo_width = 0.0f;
        float halo_blur = 0.0f;
        std::array<float, 2> translate = {{ 0, 0 }};
        TranslateAnchorType translate_anchor = TranslateAnchorType::Map;
    } icon;

    struct {
        float opacity = 1.0f;
        float size = 16.0f;
        Color color = {{ 0, 0, 0, 1 }};
        Color halo_color = {{ 0, 0, 0, 0 }};
        float halo_width = 0.0f;
        float halo_blur = 0.0f;
        std::array<float, 2> translate = {{ 0, 0 }};
        TranslateAnchorType translate_anchor = TranslateAnchorType::Map;
    } text;

    inline bool isVisible() const {
        return (icon.opacity > 0 && (icon.color[3] > 0 || icon.halo_color[3] > 0) && icon.size > 0) ||
               (text.opacity > 0 && (text.color[3] > 0 || text.halo_color[3] > 0) && text.size > 0);
    }
};

class SymbolLayoutProperties {
public:
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


class CirclePaintProperties {
public:
    float radius = 5.0f;
    Color color = {{ 0, 0, 0, 1 }};
    float opacity = 1.0f;
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchorType translateAnchor = TranslateAnchorType::Map;
    float blur = 0;

    inline bool isVisible() const {
        return radius > 0 && color[3] > 0 && opacity > 0;
    }
};

class RasterPaintProperties {
public:
    float opacity = 1.0f;
    float hue_rotate = 0.0f;
    std::array<float, 2> brightness = {{ 0, 1 }};
    float saturation = 0.0f;
    float contrast = 0.0f;
    float fade = 0.0f;

    inline bool isVisible() const {
        return opacity > 0;
    }
};

class RasterLayoutProperties {
public:
};

class BackgroundPaintProperties {
public:
    float opacity = 1.0f;
    Color color = {{ 0, 0, 0, 1 }};
    Faded<std::string> image;
};

class BackgroundLayoutProperties {
public:
};

typedef mapbox::util::variant<
    FillPaintProperties,
    LinePaintProperties,
    CirclePaintProperties,
    SymbolPaintProperties,
    RasterPaintProperties,
    BackgroundPaintProperties,
    std::false_type
> StyleProperties;

}

#endif
