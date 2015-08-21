#ifndef MBGL_STYLE_STYLE_PROPERTIES
#define MBGL_STYLE_STYLE_PROPERTIES

#include <mbgl/util/variant.hpp>
#include <mbgl/style/types.hpp>

#include <array>
#include <string>
#include <type_traits>
#include <memory>
#include <vector>

namespace mbgl {

struct FillProperties {
    FillProperties() {}
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

struct LineProperties {
    inline LineProperties() {}
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

struct SymbolProperties {
    inline SymbolProperties() {}

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

struct RasterProperties {
    inline RasterProperties() {}
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

struct BackgroundProperties {
    inline BackgroundProperties() {}
    float opacity = 1.0f;
    Color color = {{ 0, 0, 0, 1 }};
    Faded<std::string> image;
};

typedef mapbox::util::variant<
    FillProperties,
    LineProperties,
    SymbolProperties,
    RasterProperties,
    BackgroundProperties,
    std::false_type
> StyleProperties;

template <typename T>
const T &defaultStyleProperties();

}

#endif
