#ifndef LLMR_STYLE_PROPERTY_VALUE
#define LLMR_STYLE_PROPERTY_VALUE

#include <llmr/util/variant.hpp>
#include <llmr/style/function_properties.hpp>
#include <llmr/style/types.hpp>

namespace llmr {

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
