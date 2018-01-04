#pragma once

namespace mbgl {
namespace style {

class Light;

class LightObserver {
public:
    virtual ~LightObserver() = default;

    virtual void onLightChanged(const Light&) {}
};

} // namespace style
} // namespace mbgl
