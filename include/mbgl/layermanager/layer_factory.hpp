#pragma once

#include <mbgl/style/layer.hpp>

namespace mbgl {

class RenderLayer;

/**
 * @brief The LayerFactory abstract class
 * 
 * This class is responsible for creation of the layer objects that belong to a concrete layer type.
 */
class LayerFactory {
public:
    virtual ~LayerFactory() = default;
    /// Returns the layer type data.
    virtual const style::LayerTypeInfo* getTypeInfo() const noexcept = 0;
    /// Returns a new Layer instance on success call; returns `nulltptr` otherwise. 
    virtual std::unique_ptr<style::Layer> createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept = 0;
    /// Returns a new RenderLayer instance on success call; returns `nulltptr` otherwise.
    virtual std::unique_ptr<RenderLayer> createRenderLayer(Immutable<style::Layer::Impl>) noexcept = 0;

protected:
    optional<std::string> getSource(const style::conversion::Convertible& value) const noexcept;
    bool initSourceLayerAndFilter(style::Layer*, const style::conversion::Convertible& value) const noexcept;
};

} // namespace mbgl
