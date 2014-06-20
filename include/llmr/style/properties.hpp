#ifndef LLMR_STYLE_PROPERTIES
#define LLMR_STYLE_PROPERTIES

#include <array>
#include <vector>
#include <string>
#include <limits>
#include <set>
#include <map>
#include <type_traits>
#include <memory>

#include <boost/optional.hpp>
#include <llmr/util/variant.hpp>

namespace llmr {

// Stores a premultiplied color, with all four channels ranging from 0..1
typedef std::array<float, 4> Color;

struct PropertyTransition {
    inline PropertyTransition(uint16_t duration, uint16_t delay = 0)
        : duration(duration), delay(delay) {}
    inline PropertyTransition(const PropertyTransition &transition)
        : duration(transition.duration), delay(transition.delay) {}
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

    inline FunctionProperty(const FunctionProperty &property)
        : function(property.function), values(property.values) {}
    inline FunctionProperty(FunctionProperty &&property)
        : function(property.function), values(std::move(property.values)) {}


    inline void operator=(const FunctionProperty &rhs) {
        function = rhs.function;
        values = rhs.values;
    }

    inline FunctionProperty(float value) : function(&functions::constant), values(1, value) {}
    template <typename T> inline T evaluate(float z) const { return function(z, values); }
};


enum class RenderType {
    Fill,
    Line,
    Icon,
    Text,
    Raster,
    Composite,
    Background
};

enum class ClassPropertyKey {
    Enabled,
    TranslateX,
    TranslateY,
    TranslateTransition,
    TranslateAnchor,
    Opacity,
    OpacityTransition,
    Prerender,
    PrerenderBuffer,
    PrerenderSize,
    PrerenderBlur,

    FillWinding,
    FillAntialias,
    FillColor,
    FillColorTransition,
    FillStrokeColor,
    FillStrokeColorTransition,
    FillImage,

    LineWidth,
    LineWidthTransition,
    LineOffset,
    LineOffsetTransition,
    LineColor,
    LineColorTransition,
    LineDashLand,
    LineDashGap,
    LineDashTransition,

    IconSize,
    IconColor,
    IconColorTransition,
    IconImage,
    IconRadius,
    IconRadiusTransition,
    IconBlur,
    IconBlurTransition,

    TextColor,
    TextColorTransition,
    TextHaloColor,
    TextHaloColorTransition,
    TextHaloRadius,
    TextHaloRadiusTransition,
    TextHaloBlur,
    TextHaloBlurTransition,
    TextSize,
    TextRotate,
    TextAlwaysVisible,

    BackgroundColor,
    BackgroundColorTransition,
};

typedef util::variant<
    FunctionProperty,
    PropertyTransition,
    TranslateAnchor,
    Color,
    Winding,
    std::string,
    bool
> ClassPropertyValue;

class ClassProperties {
public:
    inline ClassProperties(RenderType type) : type(type) {}
    inline ClassProperties(ClassProperties &&properties)
        : type(properties.type), properties(std::move(properties.properties)) {}

    template <typename T>
    inline const T *get(ClassPropertyKey key) const {
        const auto it = properties.find(key);
        if (it != properties.end() && it->second.is<T>()) {
            return ::std::addressof(it->second.get<T>());
        } else {
            return nullptr;
        }
    }

    template <typename ...Args>
    inline void emplace(Args&& ...args) {
        properties.emplace(::std::forward<Args>(args)...);
    }

    const RenderType type;

private:
    std::map<ClassPropertyKey, ClassPropertyValue> properties;
};

struct PrerenderProperties {
    float buffer = 1.0f / 32.0f;
    uint16_t size = 256;
    uint16_t blur = 0;
};

struct GenericProperties {
    bool enabled = true;
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchor translateAnchor = TranslateAnchor::Map;
    float opacity = 1.0;

    // These are unresolved properties because the value here is per tile, so it might differ.
    boost::optional<FunctionProperty> prerender;
    boost::optional<FunctionProperty> prerenderBuffer;
    boost::optional<FunctionProperty> prerenderSize;
    boost::optional<FunctionProperty> prerenderBlur;

    inline bool getPrerender(int8_t z) const {
        return prerender && prerender.get().evaluate<bool>(z);
    }

    // Obtains prerender properties by integer zoom level.
    inline PrerenderProperties getPrerenderProperties(int8_t z) const {
        PrerenderProperties props;
        if (prerenderBuffer) props.buffer = prerenderBuffer.get().evaluate<float>(z);
        if (prerenderSize) props.size = prerenderSize.get().evaluate<uint16_t>(z);
        if (prerenderBlur) props.blur = prerenderBlur.get().evaluate<uint16_t>(z);
        return props;
    }

    virtual bool isVisible() const {
        if (!enabled) { return false; }
        if (opacity <= 0) { return false; }
        return true;
    }
};

struct IconProperties : public GenericProperties {
    inline IconProperties() : GenericProperties() {}
    float size = 0;
    Color color = {{ 1, 1, 1, 1 }};
    std::string image;
    float radius = 0;
    float blur = 0;
};

struct LineProperties : public GenericProperties {
    inline LineProperties() : GenericProperties() {}
    float width = 0;
    float offset = 0;
    Color color = {{ 0, 0, 0, 1 }};
    std::array<float, 2> dash_array = {{ 1, -1 }};
};

struct FillProperties : public GenericProperties {
    inline FillProperties() : GenericProperties() {}
    Winding winding = Winding::NonZero;
    bool antialias = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, 1 }};
    float blur = 0.0f;
    std::string image;
};

struct TextProperties : public GenericProperties {
    inline TextProperties() : GenericProperties() {}
    Color color = {{ 0, 0, 0, 1 }};
    Color halo = {{ 1, 1, 1, 0.75 }};
    float halo_radius = 0.25f;
    float halo_blur = 1.0f;
    float size = 12.0f;
    float rotate = 0.0f;
    bool always_visible = false;
};

struct BackgroundProperties : public GenericProperties {
    inline BackgroundProperties() : GenericProperties() {}
    Color color = {{ 1, 1, 1, 1 }};
};

struct RasterProperties : public GenericProperties {
    inline RasterProperties() : GenericProperties() {}
};

struct CompositeProperties : public GenericProperties {
    inline CompositeProperties() : GenericProperties() {}
};


const IconProperties defaultIconProperties;
const LineProperties defaultLineProperties;
const FillProperties defaultFillProperties;
const TextProperties defaultTextProperties;
const BackgroundProperties defaultBackgroundProperties;
const RasterProperties defaultRasterProperties;
const CompositeProperties defaultCompositeProperties;

}


namespace std {

template <> struct hash<llmr::TransitionablePropertyKey> {
public:
    inline size_t operator()(llmr::TransitionablePropertyKey prop) const {
        typedef typename std::underlying_type<llmr::TransitionablePropertyKey>::type type;
        return std::hash<type>()(static_cast<type>(prop));
    }
};

}

#endif
