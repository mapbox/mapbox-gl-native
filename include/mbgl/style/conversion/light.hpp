#pragma once

#include <mbgl/style/light.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<Light> {
public:
    template <class V>
    Result<Light> operator()(const V& value) const {
        if (!isObject(value)) {
            return Error { "light must be an object" };
        }

        Light light;

        const auto anchor = objectMember(value, "anchor");
        if (anchor) {
            auto convertedAnchor = convert<LightAnchorType>(*anchor);
            if (convertedAnchor) {
                light.setAnchor(*convertedAnchor);
            } else {
                return Error{ "light anchor must be one of [\"map\", \"viewport\"]" };
            }
        }

        const auto color = objectMember(value, "color");
        if (color) {
            auto convertedColor = convert<Color>(*color);
            if (convertedColor) {
                light.setColor(*convertedColor);
            } else {
                return Error{ "light color must be a color" };
            }
        }

        const auto position = objectMember(value, "position");
        if (position) {
            auto convertedPosition = convert<std::array<float, 3>>(*position);
            if (convertedPosition) {
                light.setPosition(*convertedPosition);
            } else {
                return Error{ "light position must be an array" };
            }
        }

        const auto intensity = objectMember(value, "intensity");
        if (intensity) {
            auto convertedIntensity = toNumber(*intensity);
            if (convertedIntensity && *convertedIntensity >= 0 && *convertedIntensity <= 1) {
                light.setIntensity(static_cast<float>(*toNumber(*intensity)));
            } else {
                return Error{ "light intensity must be a float in the range of [0, 1]" };
            }
        }

        return { light };
    }

};

} // namespace conversion
} // namespace style
} // namespace mbgl
