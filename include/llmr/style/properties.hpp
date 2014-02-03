#ifndef LLMR_STYLE_PROPERTIES
#define LLMR_STYLE_PROPERTIES

#include <array>
#include <vector>
#include <string>
#include <limits>

namespace llmr {

typedef std::array<float, 4> Color;

enum class Winding {
    EvenOdd = 1,
    NonZero = 2
};

// enum LineCap {
//     Round = 1
// };

// enum LineJoin {
//     Butt = 1,
//     Bevel = 2
// };

enum class Property {
    Null = 1,
    Constant = 2,
    Stops = 3,
    Linear = 4
};

namespace functions {

float null(float z, const std::vector<float>&);
bool null(float z, const std::vector<bool>&);

float constant(float z, const std::vector<float>& values);
bool constant(float z, const std::vector<bool>& values);

float stops(float z, const std::vector<float>& values);
bool stops(float z, const std::vector<bool>& values);

float linear(float z, const std::vector<float>& values);
bool linear(float z, const std::vector<bool>& values);

}


template <typename T>
struct FunctionProperty {
    typedef T (*fn)(float z, const std::vector<T>& values);

    fn function;
    std::vector<T> values;

    inline FunctionProperty() : function(&functions::null) {}
    inline FunctionProperty(T value) : function(&functions::constant), values(1, value) {}
    inline T operator()(float z) const { return function(z, values); }
};


// LineClass is the information we parse from the stylesheet
struct LineClass {
    FunctionProperty<bool> hidden;
    FunctionProperty<float> width;
    FunctionProperty<float> offset;
    Color color = {{ 0, 0, 0, 1 }};
    FunctionProperty<float> opacity = 1;
};

// LineProperties is the one we resolve this to.
struct LineProperties {
    bool hidden = false;
    float width = 0;
    float offset = 0;
    Color color = {{ 0, 0, 0, 1 }};
    float opacity = 1.0;
};




struct FillClass {
    FunctionProperty<bool> hidden;
    Winding winding = Winding::NonZero;
    FunctionProperty<bool> antialias = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, std::numeric_limits<float>::infinity() }};
    FunctionProperty<float> opacity = 1;
    std::string image;
};

struct FillProperties {
    bool hidden = false;
    Winding winding = Winding::NonZero;
    bool antialias = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, 1 }};
    float opacity = 1.0;
    std::string image;
};

}

#endif
