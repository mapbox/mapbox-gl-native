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

#include <llmr/style/types.hpp>

namespace llmr {

// Stores a premultiplied color, with all four channels ranging from 0..1
typedef std::array<float, 4> Color;

struct ClassPropertyTransition {
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

enum class Property {
    Null = 1,
    Constant = 2,
    Stops = 3,
    Linear = 4
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

enum class ClassPropertyKey {
    FillEnabled,
    FillAntialias,
    FillOpacity,
    FillColor,
    FillOutlineColor,
    FillTranslate, // for transitions only
    FillTranslateX,
    FillTranslateY,
    FillTranslateAnchor,
    FillImage,

    LineEnabled,
    LineOpacity,
    LineColor,
    LineTranslate, // for transitions only
    LineTranslateX,
    LineTranslateY,
    LineTranslateAnchor,
    LineWidth,
    LineOffset,
    LineBlur,
    LineDashArray, // for transitions only
    LineDashLand,
    LineDashGap,
    LineImage,

    IconEnabled,
    IconOpacity,
    IconRotate,
    IconRotateAnchor,

    TextEnabled,
    TextOpacity,
    TextSize,
    TextColor,
    TextHaloColor,
    TextHaloWidth,
    TextHaloBlur,

    CompositeEnabled,
    CompositeOpacity,

    RasterSpin,
    RasterBrightnessLow,
    RasterBrightnessHigh,
    RasterSaturation,
    RasterContrast,
    RasterFade,

    BackgroundColor
};

typedef util::variant<
    FunctionProperty,
    TranslateAnchorType,
    RotateAnchorType,
    Color,
    std::string,
    bool
> ClassPropertyValue;

class ClassProperties {
public:
    inline ClassProperties() {}
    inline ClassProperties(ClassProperties &&properties)
        : properties(std::move(properties.properties)) {}

    template <typename T>
    inline const T *get(ClassPropertyKey key) const {
        const auto it = properties.find(key);
        if (it != properties.end() && it->second.is<T>()) {
            return ::std::addressof(it->second.get<T>());
        } else {
            return nullptr;
        }
    }
    inline const ClassPropertyTransition *getTransition(ClassPropertyKey key) const {
        const auto it = transitions.find(key);
        if (it != transitions.end()) {
            return ::std::addressof(it->second);
        } else {
            return nullptr;
        }
    }

    template <typename ...Args>
    inline void set(Args&& ...args) {
        properties.emplace(::std::forward<Args>(args)...);
    }
    template <typename ...Args>
    inline void setTransition(Args&& ...args) {
        transitions.emplace(::std::forward<Args>(args)...);
    }

public:
    std::map<ClassPropertyKey, ClassPropertyValue> properties;
    std::map<ClassPropertyKey, ClassPropertyTransition> transitions;
};

struct PrerenderProperties {
    float buffer = 1.0f / 32.0f;
    uint16_t size = 256;
    uint16_t blur = 0;
};
//
//struct GenericProperties {
//    bool enabled = true;
//    std::array<float, 2> translate = {{ 0, 0 }};
//    TranslateAnchorType translateAnchor = parseTranslateAnchorType();
//    float opacity = 1.0;
//
//    // These are unresolved properties because the value here is per tile, so it might differ.
//    boost::optional<FunctionProperty> prerender;
//    boost::optional<FunctionProperty> prerenderBuffer;
//    boost::optional<FunctionProperty> prerenderSize;
//    boost::optional<FunctionProperty> prerenderBlur;
//
//    inline bool getPrerender(int8_t z) const {
//        return prerender && prerender.get().evaluate<bool>(z);
//    }
//
//    // Obtains prerender properties by integer zoom level.
//    inline PrerenderProperties getPrerenderProperties(int8_t z) const {
//        PrerenderProperties props;
//        if (prerenderBuffer) props.buffer = prerenderBuffer.get().evaluate<float>(z);
//        if (prerenderSize) props.size = prerenderSize.get().evaluate<uint16_t>(z);
//        if (prerenderBlur) props.blur = prerenderBlur.get().evaluate<uint16_t>(z);
//        return props;
//    }
//
//    virtual bool isVisible() const {
//        if (!enabled) { return false; }
//        if (opacity <= 0) { return false; }
//        return true;
//    }
//};

struct FillProperties {
    FillProperties() {}
    bool enabled = true;
    bool antialias = true;
    float opacity = 1.0f;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, 1 }};
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchorType translateAnchor = TranslateAnchorType::Default;
    std::string image;

    inline bool isVisible() const {
        return enabled && opacity > 0 && (fill_color[3] > 0 || stroke_color[3] > 0);
    }
};

struct LineProperties {
    inline LineProperties() {}
    bool enabled = true;
    float opacity = 1.0f;
    Color color = {{ 0, 0, 0, 1 }};
    std::array<float, 2> translate = {{ 0, 0 }};
    TranslateAnchorType translateAnchor = TranslateAnchorType::Default;
    float width = 0;
    float offset = 0;
    float blur = 0;
    std::array<float, 2> dash_array = {{ 1, -1 }};
    std::string image;
    
    inline bool isVisible() const {
        return enabled && opacity > 0 && color[3] > 0 && width > 0;
    }
};

struct IconProperties {
    inline IconProperties() {}
    bool enabled = true;
    float opacity = 1.0f;
    float rotate = 0.0f;
    RotateAnchorType rotate_anchor = RotateAnchorType::Default;

    inline bool isVisible() const {
        return enabled && opacity > 0;
    }
};

struct TextProperties {
    inline TextProperties() {}
    bool enabled = true;
    float opacity = 1.0f;
    float size = 12.0f;
    Color color = {{ 0, 0, 0, 1 }};
    Color halo_color = {{ 1, 1, 1, 0.75 }};
    float halo_width = 0.25f;
    float halo_blur = 1.0f;

    inline bool isVisible() const {
        return enabled && opacity > 0 && (color[3] > 0 || halo_color[3] > 0) && size > 0;
    }
};

struct CompositeProperties {
    inline CompositeProperties() {}
    bool enabled = true;
    float opacity = 1.0f;

    inline bool isVisible() const {
        return enabled && opacity > 0;
    }
};

struct RasterProperties {
    inline RasterProperties() {}
    bool enabled = true;
    float opacity = 1.0f;
    float spin = 0.0f;
    std::array<float, 2> brightness = {{ 0, 1 }};
    float saturation = 0.0f;
    float contrast = 0.0f;
    float fade = 0.0f;

    inline bool isVisible() const {
        return enabled && opacity > 0;
    }
};

struct BackgroundProperties {
    inline BackgroundProperties() {}
    Color color = {{ 1, 1, 1, 1 }};
};

typedef util::variant<
    FillProperties,
    LineProperties,
    IconProperties,
    TextProperties,
    CompositeProperties,
    RasterProperties,
    BackgroundProperties,
    std::false_type
> StyleProperties;

template <typename T>
const T &defaultStyleProperties();




//class StyleSource {
//public:
//    typedef std::shared_ptr<StyleSource> Ptr;
//
//    enum { Vector, Raster } type = Vector;
//    std::string url;
//    float tileSize = 512.0f;
//};

class RasterizeProperties {
public:
    boost::optional<FunctionProperty> enabled;
    boost::optional<FunctionProperty> buffer;
    boost::optional<FunctionProperty> size;
    boost::optional<FunctionProperty> blur;
};

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
