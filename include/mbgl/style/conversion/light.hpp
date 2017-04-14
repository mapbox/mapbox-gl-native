#pragma once

#include <mbgl/style/light.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/position.hpp>
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
                light.get<LightAnchor>().value = *convertedAnchor;
            } else {
                return {};
            }
        }

        const auto anchorTransition = objectMember(value, "anchor-transition");
        if (anchorTransition) {
            optional<TransitionOptions> transition =
                convert<TransitionOptions>(*anchorTransition, error);
            if (transition) {
                light.get<LightAnchor>().transition = *transition;
            } else {
                return {};
            }
        }

        const auto color = objectMember(value, "color");
        if (color) {
            optional<PropertyValue<Color>> convertedColor =
                convert<PropertyValue<Color>>(*color, error);

            if (convertedColor) {
                light.get<LightColor>().value = *convertedColor;
            } else {
                return {};
            }
        }

        const auto colorTransition = objectMember(value, "color-transition");
        if (colorTransition) {
            optional<TransitionOptions> transition =
                convert<TransitionOptions>(*colorTransition, error);
            if (transition) {
                light.get<LightColor>().transition = *transition;
            } else {
                return {};
            }
        }

        const auto position = objectMember(value, "position");
        if (position) {
            optional<PropertyValue<Position>> convertedPosition =
                convert<PropertyValue<Position>>(*position, error);

            if (convertedPosition) {
                light.get<LightPosition>().value = *convertedPosition;
            } else {
                return {};
            }
        }

        const auto positionTransition = objectMember(value, "position-transition");
        if (positionTransition) {
            optional<TransitionOptions> transition =
                convert<TransitionOptions>(*positionTransition, error);
            if (transition) {
                light.get<LightPosition>().transition = *transition;
            } else {
                return {};
            }
        }

        const auto intensity = objectMember(value, "intensity");
        if (intensity) {
            optional<PropertyValue<float>> convertedIntensity =
                convert<PropertyValue<float>>(*intensity, error);

            if (convertedIntensity) {
                light.get<LightIntensity>().value = *convertedIntensity;
            } else {
                return {};
            }
        }

        const auto intensityTransition = objectMember(value, "intensity-transition");
        if (intensityTransition) {
            optional<TransitionOptions> transition =
                convert<TransitionOptions>(*intensityTransition, error);
            if (transition) {
                light.get<LightIntensity>().transition = *transition;
            } else {
                return {};
            }
        }
        return { light };
    };
};

} // namespace conversion
} // namespace style
} // namespace mbgl
