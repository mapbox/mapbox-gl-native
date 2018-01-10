#include <mbgl/util/color.hpp>
#include <mbgl/util/string.hpp>

#include <csscolorparser/csscolorparser.hpp>

namespace mbgl {

optional<Color> Color::parse(const std::string& s) {
    auto css_color = CSSColorParser::parse(s);

    // Premultiply the color.
    if (css_color) {
        const float factor = css_color->a / 255;
        return {{
            css_color->r * factor,
            css_color->g * factor,
            css_color->b * factor,
            css_color->a
        }};
    } else {
        return {};
    }
}

std::string Color::stringify() const {
    return "rgba(" +
        util::toString(r * 255 / a) + "," +
        util::toString(g * 255 / a) + "," +
        util::toString(b * 255 / a) + "," +
        util::toString(a) + ")";
}

} // namespace mbgl
