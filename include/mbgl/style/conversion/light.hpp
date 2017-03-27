#pragma once

#include <mbgl/style/light.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<Light> {
public:
    template <class V>
    Result<Light> operator()(const V& value) const {
        if (!isObject(value)) {
            return Error{ "light must be an object" };
        }

        Light light;

        const auto anchor = objectMember(value, "anchor");
        if (anchor) {
            Result<PropertyValue<LightAnchorType>> convertedAnchor =
                convert<PropertyValue<LightAnchorType>>(*anchor);
            if (convertedAnchor) {
                light.set<LightAnchor>(*convertedAnchor);
            } else {
                return convertedAnchor.error();
            }
        }

        const auto color = objectMember(value, "color");
        if (color) {
            Result<PropertyValue<Color>> convertedColor = convert<PropertyValue<Color>>(*color);
            if (convertedColor) {
                light.set<LightColor>(*convertedColor);
            } else {
                return convertedColor.error();
            }
        }

        const auto position = objectMember(value, "position");
        if (position) {
            auto convertedPosition = convert<PropertyValue<Position>>(*position);

            if (convertedPosition) {
                light.set<LightPosition>(*convertedPosition);
            } else {
                return convertedPosition.error();
            }
        }

        const auto intensity = objectMember(value, "intensity");
        if (intensity) {
            Result<PropertyValue<float>> convertedIntensity =
                convert<PropertyValue<float>>(*intensity);
            if (convertedIntensity) {
                light.set<LightIntensity>(*convertedIntensity);
            } else {
                return convertedIntensity.error();
            }
        }

        return { light };
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
