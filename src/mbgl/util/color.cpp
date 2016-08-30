#include <mbgl/util/color.hpp>

#include <csscolorparser/csscolorparser.hpp>

namespace mbgl {

optional<Color> Color::parse(const std::string& s) {
    CSSColorParser::Color css_color = CSSColorParser::parse(s);

    // Premultiply the color.
    const float factor = css_color.a / 255;
    return {{
        css_color.r * factor,
        css_color.g * factor,
        css_color.b * factor,
        css_color.a
    }};
}

} // namespace mbgl
