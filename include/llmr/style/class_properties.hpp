#ifndef LLMR_STYLE_CLASS_PROPERTIES
#define LLMR_STYLE_CLASS_PROPERTIES

#include <llmr/util/variant.hpp>
#include <llmr/style/function_properties.hpp>
#include <llmr/style/types.hpp>


#include <map>

namespace llmr {

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


struct ClassPropertyTransition {
    uint16_t duration = 0;
    uint16_t delay = 0;
};

class ClassProperties {
public:
    inline ClassProperties() {}
    inline ClassProperties(ClassProperties &&properties)
        : properties(std::move(properties.properties)) {}

    template <typename ...Args>
    inline void set(Args&& ...args) {
        properties.emplace(::std::forward<Args>(args)...);
    }
    template <typename ...Args>
    inline void setTransition(Args&& ...args) {
        transitions.emplace(::std::forward<Args>(args)...);
    }

    inline const ClassPropertyTransition &getTransition(ClassPropertyKey key, const ClassPropertyTransition &defaultTransition) const {
        auto it = transitions.find(key);
        if (it == transitions.end()) {
            return defaultTransition;
        } else {
            return it->second;
        }
    }

    // Route-through iterable interface so that you can iterate on the object as is.
    inline std::map<ClassPropertyKey, ClassPropertyValue>::const_iterator begin() const {
        return properties.begin();
    }
    inline std::map<ClassPropertyKey, ClassPropertyValue>::const_iterator end() const {
        return properties.end();
    }

public:
    std::map<ClassPropertyKey, ClassPropertyValue> properties;
    std::map<ClassPropertyKey, ClassPropertyTransition> transitions;
};

}

#endif
