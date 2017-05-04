#pragma once

#include <mbgl/style/light_properties.hpp>
#include <mbgl/style/light_observer.hpp>

namespace mbgl {
namespace style {

class Light::Impl {
public:

    LightObserver nullObserver;
    LightObserver* observer = &nullObserver;
    void setObserver(LightObserver*);

    IndexedTuple<LightProperties, LightProperties> properties;
};

} // namespace style
} // namespace mbgl
