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
    std::array<double, 4> array = toArray();
    return "rgba(" +
        util::toString(array[0]) + "," +
        util::toString(array[1]) + "," +
        util::toString(array[2]) + "," +
        util::toString(array[3]) + ")";
}

std::array<double, 4> Color::toArray() const {
    if (a == 0) {
        return {{ 0, 0, 0, 0 }};
    } else {
        return {{
            r * 255 / a,
            g * 255 / a,
            b * 255 / a,
            a,
        }};
    }
}

} // namespace mbgl
