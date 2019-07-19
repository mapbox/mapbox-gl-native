#pragma once
#include <mbgl/style/layer_impl.hpp>

namespace mbgl {

namespace style {

/**
 * @brief An interface, wrapping evaluated layer properties.
 * 
 *  It is an abstract base class; concrete derived classes that hold the actual data are provided for each layer type. 
 */
class LayerProperties {
public:
    virtual ~LayerProperties() = default;

    Immutable<Layer::Impl> baseImpl;

protected:
    LayerProperties(Immutable<Layer::Impl> impl) : baseImpl(std::move(impl)) {}
};

template <class Derived>
inline const auto& getEvaluated(const Immutable<LayerProperties>& properties) {
    return static_cast<const Derived&>(*properties).evaluated;
}

template <class Derived>
inline const auto& getCrossfade(const Immutable<LayerProperties>& properties) {
    return static_cast<const Derived&>(*properties).crossfade;
}

} // namespace style
} // namespace mbgl
