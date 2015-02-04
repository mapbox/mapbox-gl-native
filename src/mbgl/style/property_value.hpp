#ifndef MBGL_STYLE_PROPERTY_VALUE
#define MBGL_STYLE_PROPERTY_VALUE

#include <mbgl/util/variant.hpp>
#include <mbgl/style/function_properties.hpp>
#include <mbgl/style/piecewisefunction_properties.hpp>
#include <mbgl/style/types.hpp>

#include <vector>

namespace mbgl {

typedef mapbox::util::variant<
    std::string,
    TranslateAnchorType,
    RotateAnchorType,
    bool,
    CapType,
    JoinType,
    VisibilityType,
    PlacementType,
    RotationAlignmentType,
    TextTransformType,
    TextJustifyType,
    TextAnchorType,
    Function<bool>,
    Function<float>,
    Function<Color>,
    PiecewiseConstantFunction<Faded<std::vector<float>>>,
    PiecewiseConstantFunction<Faded<std::string>>
> PropertyValue;

}

#endif
