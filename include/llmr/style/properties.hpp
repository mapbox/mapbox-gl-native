#ifndef LLMR_STYLE_PROPERTIES
#define LLMR_STYLE_PROPERTIES

#include <array>
#include <vector>

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
    Stops = 3
};

namespace functions {

float null(float z, const std::vector<float>&);
bool null(float z, const std::vector<bool>&);

float constant(float z, const std::vector<float>& values);
bool constant(float z, const std::vector<bool>& values);

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


// StrokeClass is the information we parse from the stylesheet
struct StrokeClass {
    FunctionProperty<bool> hidden;
    FunctionProperty<float> width;
    FunctionProperty<float> offset;
    Color color = {{ 0, 0, 0, 1 }};
};

// StrokeProperties is the one we resolve this to.
struct StrokeProperties {
    bool hidden = false;
    float width = 0;
    float offset = 0;
    Color color = {{ 0, 0, 0, 1 }};
};




struct FillClass {
    FunctionProperty<bool> hidden;
    Winding winding = Winding::NonZero;
    FunctionProperty<bool> antialias = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, std::numeric_limits<float>::infinity() }};
    FunctionProperty<float> opacity = 1;
};

struct FillProperties {
    bool hidden = false;
    Winding winding = Winding::NonZero;
    bool antialias = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, 1 }};
    float opacity = 1.0;
};

}

#endif
