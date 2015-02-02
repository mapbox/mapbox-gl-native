#ifndef MBGL_STYLE_PROPERTY_VALUE
#define MBGL_STYLE_PROPERTY_VALUE

#include <mbgl/util/variant.hpp>
#include <mbgl/style/function_properties.hpp>
#include <mbgl/style/fadedfunction_properties.hpp>
#include <mbgl/style/types.hpp>

#include <vector>

namespace mbgl {

typedef mapbox::util::variant<
    std::string,
    TranslateAnchorType,
    RotateAnchorType,
    Function<bool>,
    Function<float>,
    Function<Color>,
    Function<std::vector<float>>,
    FadedStopsFunction<Faded<std::string>>
> PropertyValue;

}

#endif
