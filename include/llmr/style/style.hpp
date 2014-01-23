#ifndef LLMR_STYLE_STYLE
#define LLMR_STYLE_STYLE

#include <array>

namespace llmr {

typedef std::array<float, 4> Color;

enum Winding {
    EvenOdd = 1,
    NonZero = 2
};

enum LineCap {
    Round = 1
};

enum LineJoin {
    Butt = 1,
    Bevel = 2
};

struct StrokeStyle {
    Color line_color = {{ 0, 0, 0, 1 }};
    float line_width = 1;
};

struct FillStyle {
    Winding winding = NonZero;
    bool antialiasing = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, 1 }};
    float stroke_width = 1;
};

class Style {
public:
    StrokeStyle strokeStyle(const std::string& layer_name) const;
    FillStyle fillStyle(const std::string& layer_name) const;
};

}

#endif