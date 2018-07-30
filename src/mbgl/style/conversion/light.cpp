#include <mbgl/style/conversion/light.hpp>
#include <mbgl/style/conversion/position.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<Light> Converter<Light>::operator()(const Convertible& value, Error& error) const {
    if (!isObject(value)) {
        error = { "light must be an object" };
        return {};
    }

    Light light;

    const auto anchor = objectMember(value, "anchor");
    if (anchor) {
        optional<PropertyValue<LightAnchorType>> convertedAnchor =
            convert<PropertyValue<LightAnchorType>>(*anchor, error, false);

        if (convertedAnchor) {
            light.setAnchor(*convertedAnchor);
        } else {
            return {};
        }
    }

    const auto anchorTransition = objectMember(value, "anchor-transition");
    if (anchorTransition) {
        optional<TransitionOptions> transition =
            convert<TransitionOptions>(*anchorTransition, error);
        if (transition) {
            light.setAnchorTransition(*transition);
        } else {
            return {};
        }
    }

    const auto color = objectMember(value, "color");
    if (color) {
        optional<PropertyValue<Color>> convertedColor =
            convert<PropertyValue<Color>>(*color, error, false);

        if (convertedColor) {
            light.setColor(*convertedColor);
        } else {
            return {};
        }
    }

    const auto colorTransition = objectMember(value, "color-transition");
    if (colorTransition) {
        optional<TransitionOptions> transition =
            convert<TransitionOptions>(*colorTransition, error);
        if (transition) {
            light.setColorTransition(*transition);
        } else {
            return {};
        }
    }

    const auto position = objectMember(value, "position");
    if (position) {
        optional<PropertyValue<Position>> convertedPosition =
            convert<PropertyValue<Position>>(*position, error, false);

        if (convertedPosition) {
            light.setPosition(*convertedPosition);
        } else {
            return {};
        }
    }

    const auto positionTransition = objectMember(value, "position-transition");
    if (positionTransition) {
        optional<TransitionOptions> transition =
            convert<TransitionOptions>(*positionTransition, error);
        if (transition) {
            light.setPositionTransition(*transition);
        } else {
            return {};
        }
    }

    const auto intensity = objectMember(value, "intensity");
    if (intensity) {
        optional<PropertyValue<float>> convertedIntensity =
            convert<PropertyValue<float>>(*intensity, error, false);

        if (convertedIntensity) {
            light.setIntensity(*convertedIntensity);
        } else {
            return {};
        }
    }

    const auto intensityTransition = objectMember(value, "intensity-transition");
    if (intensityTransition) {
        optional<TransitionOptions> transition =
            convert<TransitionOptions>(*intensityTransition, error);
        if (transition) {
            light.setIntensityTransition(*transition);
        } else {
            return {};
        }
    }

    return { std::move(light) };
}

} // namespace conversion
} // namespace style
} // namespace mbgl
