#ifndef LLMR_STYLE_PROPERTIES
#define LLMR_STYLE_PROPERTIES

#include <array>
#include <vector>
#include <string>
#include <limits>
#include <set>

namespace llmr {

// Stores a premultiplied color, with all four channels ranging from 0..1
typedef std::array<float, 4> Color;

struct PropertyTransition {
    uint16_t duration = 0;
    uint16_t delay = 0;
};

enum class TransitionablePropertyKey {
    Translate = 1,
    FillColor = 2,
    StrokeColor = 3,
    Opacity = 4,
    Width = 5,
    Offset = 6,
    Color = 7,
    DashArray = 8,
    Radius = 9,
    Blur = 10,
    Halo = 11,
    HaloRadius = 12,
    HaloBlur = 13,
};

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

struct IconClass {
    FunctionProperty enabled = true;
    std::array<FunctionProperty, 2> translate = {{ 0, 0 }};
    PropertyTransition translate_transition;
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    FunctionProperty size;
    Color color = {{ 1, 1, 1, 1 }};
    PropertyTransition color_transition;
    FunctionProperty opacity = 1;
    PropertyTransition opacity_transition;
    std::string image;
    FunctionProperty radius = 0;
    PropertyTransition radius_transition;
    FunctionProperty blur = 0;
    PropertyTransition blur_transition;
    std::set<std::string> specifiers;
};

struct IconProperties {
    bool enabled = true;
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    float size = 0;
    Color color = {{ 1, 1, 1, 1 }};
    float opacity = 1.0;
    std::string image;
    float radius = 0;
    float blur = 0;
};

struct LineClass {
    FunctionProperty enabled = true;
    std::array<FunctionProperty, 2> translate = {{ 0, 0 }};
    PropertyTransition translate_transition;
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    FunctionProperty width;
    PropertyTransition width_transition;
    FunctionProperty offset;
    PropertyTransition offset_transition;
    Color color = {{ 0, 0, 0, 1 }};
    PropertyTransition color_transition;
    std::array<FunctionProperty, 2> dash_array = {{ 1, -1 }};
    PropertyTransition dash_array_transition;
    FunctionProperty opacity = 1;
    PropertyTransition opacity_transition;
    std::set<std::string> specifiers;
};

struct LineProperties {
    bool enabled = true;
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    float width = 0;
    float offset = 0;
    Color color = {{ 0, 0, 0, 1 }};
    std::array<float, 2> dash_array = {{ 1, -1 }};
    float opacity = 1.0;
};

struct FillClass {
    FunctionProperty enabled = true;
    std::array<FunctionProperty, 2> translate = {{ 0, 0 }};
    PropertyTransition translate_transition;
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    Winding winding = Winding::NonZero;
    FunctionProperty antialias = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    PropertyTransition fill_color_transition;
    Color stroke_color = {{ 0, 0, 0, std::numeric_limits<float>::infinity() }};
    PropertyTransition stroke_color_transition;
    FunctionProperty opacity = 1;
    PropertyTransition opacity_transition;
    std::string image;
    std::set<std::string> specifiers;
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
    PropertyTransition translate_transition;
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    Color color = {{ 0, 0, 0, 1 }};
    PropertyTransition color_transition;
    Color halo = {{ 1, 1, 1, 0.75 }};
    PropertyTransition halo_transition;
    FunctionProperty halo_radius = 0.25f;
    PropertyTransition halo_radius_transition;
    FunctionProperty halo_blur = 1.0f;
    PropertyTransition halo_blur_transition;
    FunctionProperty size = 12.0f;
    FunctionProperty rotate = 0.0f;
    FunctionProperty always_visible = false;
    FunctionProperty opacity = 1;
    PropertyTransition opacity_transition;
    std::set<std::string> specifiers;
};

struct TextProperties {
    bool enabled = true;
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    Color color = {{ 0, 0, 0, 1 }};
    Color halo = {{ 1, 1, 1, 0.75 }};
    float halo_radius = 0.25f;
    float halo_blur = 1.0f;
    float size = 12.0f;
    float rotate = 0.0f;
    bool always_visible = false;
    float opacity = 1.0;
};

struct BackgroundClass {
    Color color = {{ 1, 1, 1, 1 }};
    PropertyTransition color_transition;
    FunctionProperty opacity = 1;
    PropertyTransition opacity_transition;
    std::set<std::string> specifiers;
};

struct BackgroundProperties {
    Color color = {{ 1, 1, 1, 1 }};
    float opacity = 1.0;
};

struct RasterClass {
    FunctionProperty enabled = true;
    std::array<FunctionProperty, 2> translate = {{ 0, 0 }};
    PropertyTransition translate_transition;
    FunctionProperty opacity = 1;
    PropertyTransition opacity_transition;
    std::set<std::string> specifiers;
};

struct RasterProperties {
    bool enabled = true;
    std::array<float, 2> translate = {{ 0, 0 }};
    float opacity = 1.0;
};

struct CompositeClass {
    FunctionProperty enabled = true;
    FunctionProperty opacity = 1;
    PropertyTransition opacity_transition;
    std::set<std::string> specifiers;
};

struct CompositeProperties {
    inline CompositeProperties() {}
    bool enabled = true;
    float opacity = 1.0;
};


const CompositeProperties defaultCompositeProperties;

}

#endif
