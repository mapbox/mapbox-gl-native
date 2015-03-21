#include <mbgl/style/color_operations.hpp>
#include <mbgl/style/value.hpp>
#include <mbgl/platform/log.hpp>
#include <csscolorparser/csscolorparser.hpp>

#include <string>
#include <array>
#include <cmath>

namespace mbgl {

    CSSColorParser::Color parseColorOp(const rapidjson::Value& value, std::unordered_map<std::string, const rapidjson::Value *> constants) {
        CSSColorParser::Color empty;
        
        if (value.Size() < 3) {
            Log::Warning(Event::ParseStyle, "color operation must have at least 3 elements");
            return empty;
        }
        if (!value[0u].IsString()) {
            Log::Warning(Event::ParseStyle, "color operator must be a string");
            return empty;
        }
        
        if (!value[1u].IsNumber()) {
            Log::Warning(Event::ParseStyle, "degree of change must be a number");
            return empty;
        }
        
        std::string op = { value[0u].GetString(), value[0u].GetStringLength() };
        double degree = value[1u].GetDouble();
        
        ColorOperation opResult = ColorOperation::ColorOperation(degree, parseColorarr(value[2u], constants));
        CSSColorParser::Color result;

        if (op == "darken") {
            Darken * dark = static_cast<Darken*> (&opResult);
            result = dark->evaluate();
        } else if (op == "lighten") {
            Lighten * light = static_cast<Lighten*> (&opResult);
            result = light->evaluate();
        } else if (op == "desaturate") {
            Desaturate * desaturate = static_cast<Desaturate*> (&opResult);
            result = desaturate->evaluate();
        } else if (op == "saturate") {
            Saturate * saturate = static_cast<Saturate*> (&opResult);
            result = saturate->evaluate();
        } else if (op == "fadeout") {
            Fadeout * fadeout = static_cast<Fadeout*> (&opResult);
            result = fadeout->evaluate();
        } else if (op == "fadein") {
            Fadein * fadein = static_cast<Fadein*> (&opResult);
            result = fadein->evaluate();
        } else if (op == "spin") {
            Spin * spin = static_cast<Spin*> (&opResult);
            result = spin->evaluate();
        } else if (op == "mix") {
            if (!value[3u].IsString() && !value[3u].IsArray()) {
                Log::Warning(Event::ParseStyle, "Mix must have a valid second color");
                return empty;
            }
            Mix mix = Mix::Mix(opResult, parseColorarr(value[3u], constants));
            result = mix.evaluate();
        } else {
            Log::Warning(Event::ParseStyle, "color operator must be one of \"darken\", \"lighten\", \"desaturate\", \"saturate\", \"fadeout\", \"fadein\", \"spin\", \"mix\"");
        }
        return result;

    }

    CSSColorParser::Color parseColorarr(const rapidjson::Value& color,  std::unordered_map<std::string, const rapidjson::Value *> constants) {
        CSSColorParser::Color css_color;
        if (color.IsArray()) {
            css_color = parseColorarr(color, constants);
        } else if (color.IsString()) {
            css_color = CSSColorParser::parse( {color.GetString(), color.GetStringLength()});
        } else {
            Log::Warning(Event::ParseStyle, "color must be a string or array");
            return css_color;
        }
        return css_color;
    }

    std::array<double, 4> ColorOperation::toHSL() {

        double r = color_.r / 255.0;
        double g = color_.g / 255.0;
        double b = color_.b / 255.0;
        double a = color_.a;
        
        double max = std::max(r, std::max(g, b)),
            min = std::min(r, std::min(g, b));
        
        double h, s, l = (max + min) / 2,
            d = max - min;
        
        if (max == min) {
            h = s = 0;
        } else {
            if (l > 0.5) {
                s = d / (2 - max - min);
            } else {
                s = d / (max + min);
            }
            if (max == r) {
                h = (g - b) / d + (g < b ? 6 : 0);
            } else if (max == g) {
                h = (b - r) / d + 2;
            } else {
                h = (r - g) / d + 4;
            }
            h /= 6;
        }
        h *= 360;
        return {{h, s, l, a}};
    }

    double ColorOperation::clamp(double val) {
        double max = std::max(0.0, val);
        return std::min(1.0, max);
    }

    CSSColorParser::Color toColor(std::array<double, 4> hsl) {
        std::string str = "hsla(";
        str.append(std::to_string(hsl[0]));
        str = str + ", ";
        str.append(std::to_string(hsl[1]*100));
        str = str + "%, ";
        str.append(std::to_string(hsl[2]*100));
        str = str + "%, ";
        str.append(std::to_string(hsl[3]));
        str = str + ")";
        return CSSColorParser::parse(str);
    }

    CSSColorParser::Color Darken::evaluate() {
        std::array<double, 4> hsl = this->toHSL();
        
        hsl[2] -= this->degree_ / 100.0;
        hsl[2] = clamp(hsl[2]);
        return toColor(hsl);
    }

    CSSColorParser::Color Lighten::evaluate() {
        std::array<double, 4> hsl = this->toHSL();
        
        hsl[2] += this->degree_ / 100.0;
        hsl[2] = clamp(hsl[2]);
        return toColor(hsl);
    }

    CSSColorParser::Color Desaturate::evaluate() {
        std::array<double, 4> hsl = this->toHSL();
        
        hsl[1] -= this->degree_ / 100.0;
        hsl[1] = clamp(hsl[1]);
        return toColor(hsl);
    }

    CSSColorParser::Color Saturate::evaluate() {
        std::array<double, 4> hsl = this->toHSL();
        
        hsl[1] += this->degree_ / 100.0;
        hsl[1] = clamp(hsl[1]);
        return toColor(hsl);
    }

    CSSColorParser::Color Fadeout::evaluate() {
        std::array<double, 4> hsl = this->toHSL();

        hsl[3] -= this->degree_ / 100.0;
        hsl[3] = clamp(hsl[3]);
        return toColor(hsl);
    }

    CSSColorParser::Color Fadein::evaluate() {
        std::array<double, 4> hsl = this->toHSL();
        
        hsl[3] += this->degree_ / 100.0;
        hsl[3] = clamp(hsl[3]);
        return toColor(hsl);
    }

    CSSColorParser::Color Spin::evaluate() {
        std::array<double, 4> hsl = this->toHSL();
        
        double hue = fmod((hsl[0] + this->degree_), 360.0);
        if (hue < 0) {
            hsl[0] = 360 + hue;
        } else {
            hsl[0] = hue;
        }
        return toColor(hsl);
    }

    CSSColorParser::Color Mix::evaluate() {
        double p = this->degree_ / 100.0;
        double w = p * 2 - 1;
        double a =  this->ColorOperation::toHSL()[3] - this->Mix::toHSL()[3];
        
        double w1 = (((w * a == -1) ? w : (w + a) / (1 + w * a)) + 1) / 2.0;
        double w2 = 1 - w1;
        
        std::array<double, 3> rgb = {{color_.r * w1 + color2_.r * w2,
                   color_.g * w1 + color2_.g * w2,
                   color_.b * w1 + color2_.b * w2}};
        
        double alpha = color_.a * p + color2_.a * (1 - p);
        return CSSColorParser::Color(rgb[0], rgb[1], rgb[2], alpha);
    }
}