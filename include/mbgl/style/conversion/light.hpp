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
    optional<Light> operator()(const V& value, Error& error) const {
        if (!isObject(value)) {
            error = { "light must be an object" };
            return {};
        }

        Light light;

        const auto anchor = objectMember(value, "anchor");
        if (anchor) {
            optional<PropertyValue<LightAnchorType>> convertedAnchor =
                convert<PropertyValue<LightAnchorType>>(*anchor, error);
            if (convertedAnchor) {
                light.set<LightAnchor>(*convertedAnchor);
            } else {
                return {};
            }
        }

        const auto color = objectMember(value, "color");
        if (color) {
            optional<PropertyValue<Color>> convertedColor = convert<PropertyValue<Color>>(*color, error);
            if (convertedColor) {
                light.set<LightColor>(*convertedColor);
            } else {
                return {};
            }
        }

        const auto position = objectMember(value, "position");
        if (position) {
            optional<PropertyValue<Position>> convertedPosition = convert<PropertyValue<Position>>(*position, error);

            if (convertedPosition) {
                light.set<LightPosition>(*convertedPosition);
            } else {
                return {};
            }
        }

        const auto intensity = objectMember(value, "intensity");
        if (intensity) {
            optional<PropertyValue<float>> convertedIntensity =
                convert<PropertyValue<float>>(*intensity, error);
            if (convertedIntensity) {
                light.set<LightIntensity>(*convertedIntensity);
            } else {
                return {};
            }
        }

        return { light };
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
