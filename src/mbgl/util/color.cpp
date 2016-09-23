#include <cmath>
#include <mbgl/util/color.hpp>
#include <mbgl/util/color_lab.hpp>

#include <csscolorparser/csscolorparser.hpp>

const float Kn = 18;
const float Xn = 0.950470; // D65 standard referent
const float Yn = 1;
const float Zn = 1.088830;
const float t0 = 4 / 29;
const float t1 = 6 / 29;
const float t2 = 3 * t1 * t1;
const float t3 = t1 * t1 * t1;
// const float deg2rad = M_PI / 180;
// const float rad2deg = 180 / M_PI;

float xyz2lab(float t) {
    return t > t3 ? std::pow(t, 1 / 3) : t / t2 + t0;
}

float lab2xyz(float t) {
    return t > t1 ? t * t * t : t2 * (t - t0);
}

float xyz2rgb(float x) {
    return 255 * (x <= 0.0031308 ? 12.92 * x : 1.055 * std::pow(x, 1 / 2.4) - 0.055);
}

float rgb2xyz(float x) {
    return (x /= 255) <= 0.04045 ? x / 12.92 : std::pow((x + 0.055) / 1.055, 2.4);
}

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

optional<ColorLAB> Color::to_lab() {

    const float rawB = rgb2xyz(r);
    const float rawA = rgb2xyz(g);
    const float rawL = rgb2xyz(b);
    const float x = xyz2lab((0.4124564 * rawB + 0.3575761 * rawA + 0.1804375 * rawL) / Xn);
    const float y = xyz2lab((0.2126729 * rawB + 0.7151522 * rawA + 0.0721750 * rawL) / Yn);
    const float z = xyz2lab((0.0193339 * rawB + 0.1191920 * rawA + 0.9503041 * rawL) / Zn);

    return ColorLAB{
        116 * y - 16,
        500 * (x - y),
        200 * (y - z),
        a
    };
}

} // namespace mbgl
