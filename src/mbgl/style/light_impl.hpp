#pragma once

#include <mbgl/style/light.hpp>
#include <mbgl/style/light_properties.hpp>

namespace mbgl {
namespace style {

class Light::Impl {
public:
    LightProperties::Transitionable properties;
};

} // namespace style
} // namespace mbgl
