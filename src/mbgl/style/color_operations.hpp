#ifndef MBGL_STYLE_COLOR_OPERATIONS
#define MBGL_STYLE_COLOR_OPERATIONS

#include <csscolorparser/csscolorparser.hpp>
#include "rapidjson/document.h"
#include <mbgl/style/value.hpp>

#include <string>
#include <array>
#include <unordered_map>

namespace mbgl {

CSSColorParser::Color parseColorOp(const rapidjson::Value&, const std::unordered_map<std::string, const rapidjson::Value *>&);
CSSColorParser::Color parseColorarr(const rapidjson::Value&, const std::unordered_map<std::string, const rapidjson::Value *>& constants);

class ColorOperation {
protected:
    double degree_;
    CSSColorParser::Color color_;
public:
    ColorOperation() : degree_(0), color_() {}
    ColorOperation(double degree, CSSColorParser::Color color) : degree_(degree), color_(color) {}
    virtual ~ColorOperation() {}
    std::array<double, 4> toHSL();
    double clamp(double);

};

class Lighten : public ColorOperation {
public:
    CSSColorParser::Color evaluate();
};

class Saturate : public ColorOperation {
public:
    CSSColorParser::Color evaluate();
};

class Fade : public ColorOperation {
public:
    CSSColorParser::Color evaluate();
};

class Spin : public ColorOperation {
public:
    CSSColorParser::Color evaluate();
};

class Mix : public ColorOperation {
protected:
    CSSColorParser::Color color2_;
public:
    Mix(ColorOperation colorOp, CSSColorParser::Color color2) : ColorOperation(colorOp), color2_(color2) {}
    CSSColorParser::Color evaluate();
    std::array<double, 4> toHSL() { return ColorOperation(0, color2_).ColorOperation::toHSL();}
};
} 

#endif