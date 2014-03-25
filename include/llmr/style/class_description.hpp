#ifndef LLMR_STYLE_CLASS_DESCRIPTION
#define LLMR_STYLE_CLASS_DESCRIPTION

#include <string>
#include <vector>
#include <map>
#include "properties.hpp"

namespace llmr {

class ClassDescription {
public:
    BackgroundClass background;
    std::map<std::string, FillClass> fill;
    std::map<std::string, LineClass> line;
    std::map<std::string, PointClass> point;
    std::map<std::string, TextClass> text;
};


}

#endif
