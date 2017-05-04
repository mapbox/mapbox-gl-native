#pragma once

#include <mbgl/style/light.hpp>

namespace mbgl {
namespace style {

class LightObserver {
public:
    virtual ~LightObserver() = default;

    virtual void onLightChanged(const Light&) {}
};

} // namespace style
} // namespace mbgl
