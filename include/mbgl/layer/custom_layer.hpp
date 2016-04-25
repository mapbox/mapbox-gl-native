#pragma once

#include <mbgl/style/layer.hpp>

namespace mbgl {

class CustomLayer : public Layer {
public:
    CustomLayer(const std::string& id,
                CustomLayerInitializeFunction,
                CustomLayerRenderFunction,
                CustomLayerDeinitializeFunction,
                void* context);
    ~CustomLayer() final;

    // Private implementation

    class Impl;
    Impl* impl;

    CustomLayer(const Impl&);
    CustomLayer(const CustomLayer&) = delete;
};

template <>
inline bool Layer::is<CustomLayer>() const {
    return type == Type::Custom;
}

} // namespace mbgl
