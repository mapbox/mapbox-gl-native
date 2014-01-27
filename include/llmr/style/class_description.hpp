#ifndef LLMR_STYLE_CLASS_DESCRIPTION
#define LLMR_STYLE_CLASS_DESCRIPTION

#include <string>
#include <vector>
#include <map>
#include "properties.hpp"

namespace llmr {

class ClassDescription {
public:
    std::map<std::string, FillClass> fill;
    std::map<std::string, StrokeClass> stroke;
};


}

#endif
