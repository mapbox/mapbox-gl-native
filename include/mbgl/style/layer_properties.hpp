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
    // Returns constants mask for the data-driven properties.
    virtual unsigned long constantsMask() const { return 0u; }
    Immutable<Layer::Impl> baseImpl;
    // Contains render passes used by the renderer, see `mbgl::RenderPass`.
    uint8_t renderPasses = 0u;

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
