#pragma once

#include <mbgl/style/light.hpp>
#include <mbgl/style/light_properties.hpp>

namespace mbgl {
namespace style {

class Light::Impl {
public:
    IndexedTuple<LightProperties, LightProperties> properties;
};

} // namespace style
} // namespace mbgl
