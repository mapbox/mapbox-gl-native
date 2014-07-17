#ifndef MBGL_STYLE_PROPERTY_VALUE
#define MBGL_STYLE_PROPERTY_VALUE

#include <mbgl/util/variant.hpp>
#include <mbgl/style/function_properties.hpp>
#include <mbgl/style/types.hpp>

namespace mbgl {

typedef util::variant<
    std::string,
    TranslateAnchorType,
    RotateAnchorType,
    Function<bool>,
    Function<float>,
    Function<Color>
> PropertyValue;

}

#endif
