#ifndef LLMR_STYLE_PROPERTIES
#define LLMR_STYLE_PROPERTIES

#include <array>
#include <vector>
#include <string>
#include <limits>

namespace llmr {

// Stores a premultiplied color, with all four channels ranging from 0..1
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

enum class TranslateAnchor : uint8_t {
    Map = 1,
    Viewport = 2
};

namespace functions {

float null(float z, const std::vector<float>&);
float constant(float z, const std::vector<float>& values);
float min(float z, const std::vector<float>& values);
float max(float z, const std::vector<float>& values);
float stops(float z, const std::vector<float>& values);
float linear(float z, const std::vector<float>& values);
float exponential(float z, const std::vector<float>& values);

}

struct FunctionProperty {
    typedef float (*fn)(float z, const std::vector<float>& values);

    fn function;
    std::vector<float> values;

    inline FunctionProperty() : function(&functions::null) {}
    inline FunctionProperty(float value) : function(&functions::constant), values(1, value) {}
    template <typename T> inline T evaluate(float z) const { return function(z, values); }
};

struct PointClass {
    FunctionProperty enabled = true;
    std::array<FunctionProperty, 2> translate = {{ 0, 0 }};
    FunctionProperty size;
    Color color = {{ 0, 0, 0, 1 }};
    FunctionProperty opacity = 1;
    std::string image;
    FunctionProperty radius = 0;
    FunctionProperty blur = 0;
};

struct PointProperties {
    bool enabled = true;
    std::array<float, 2> translate = {{ 0, 0 }};
    float size = 0;
    Color color = {{ 0, 0, 0, 1 }};
    float opacity = 1.0;
    std::string image;
    float radius = 0;
    float blur = 0;
};

struct LineClass {
    FunctionProperty enabled = true;
    std::array<FunctionProperty, 2> translate = {{ 0, 0 }};
    FunctionProperty width;
    FunctionProperty offset;
    Color color = {{ 0, 0, 0, 1 }};
    std::array<FunctionProperty, 2> dash_array = {{ 1, -1 }};
    FunctionProperty opacity = 1;
};

struct LineProperties {
    bool enabled = true;
    std::array<float, 2> translate = {{ 0, 0 }};
    float width = 0;
    float offset = 0;
    Color color = {{ 0, 0, 0, 1 }};
    std::array<float, 2> dash_array = {{ 1, -1 }};
    float opacity = 1.0;
};

struct FillClass {
    FunctionProperty enabled = true;
    std::array<FunctionProperty, 2> translate = {{ 0, 0 }};
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    Winding winding = Winding::NonZero;
    FunctionProperty antialias = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, std::numeric_limits<float>::infinity() }};
    FunctionProperty opacity = 1;
    std::string image;
};

struct FillProperties {
    bool enabled = true;
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    Winding winding = Winding::NonZero;
    bool antialias = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, 1 }};
    float opacity = 1.0;
    std::string image;
};

struct TextClass {
    FunctionProperty enabled = true;
    std::array<FunctionProperty, 2> translate = {{ 0, 0 }};
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    Color color = {{ 0, 0, 0, 1 }};
    Color halo = {{ 1, 1, 1, 0.75 }};
    FunctionProperty haloRadius = 0.25f;
    FunctionProperty size = 12.0f;
    FunctionProperty rotate = 0.0f;
    FunctionProperty alwaysVisible = false;
    FunctionProperty opacity = 1;
};

struct TextProperties {
    bool enabled = true;
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    Color color = {{ 0, 0, 0, 1 }};
    Color halo = {{ 1, 1, 1, 0.75 }};
    float haloRadius = 0.25f;
    float size = 12.0f;
    float rotate = 0.0f;
    bool alwaysVisible = false;
    float opacity = 1.0;
};

struct BackgroundClass {
    Color color = {{ 1, 1, 1, 1 }};
    FunctionProperty opacity = 1;
};

struct BackgroundProperties {
    Color color = {{ 1, 1, 1, 1 }};
    float opacity = 1.0;
};

struct RasterClass {
    FunctionProperty enabled = true;
    std::array<FunctionProperty, 2> translate = {{ 0, 0 }};
    FunctionProperty opacity = 1;
};

struct RasterProperties {
    bool enabled = true;
    std::array<float, 2> translate = {{ 0, 0 }};
    float opacity = 1.0;
};

}

#endif
