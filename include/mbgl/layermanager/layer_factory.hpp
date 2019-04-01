#pragma once

#include <mbgl/style/layer.hpp>

#include <vector>

namespace mbgl {

namespace style {
class LayerProperties;
}  // namespace style
class Bucket;
class BucketParameters;
class GeometryTileLayer;
class Layout;
class LayoutParameters;
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
    /// Returns a new Layer instance on success call; returns `nullptr` otherwise. 
    virtual std::unique_ptr<style::Layer> createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept = 0;
    /// Returns a new RenderLayer instance.
    virtual std::unique_ptr<RenderLayer> createRenderLayer(Immutable<style::Layer::Impl>) noexcept = 0;
    /// Returns a new Bucket instance on success call; returns `nullptr` otherwise. 
    virtual std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<Immutable<style::LayerProperties>>&) noexcept;
    /// Returns a new Layout instance on success call; returns `nullptr` otherwise. 
    virtual std::unique_ptr<Layout> createLayout(const LayoutParameters&, std::unique_ptr<GeometryTileLayer>, const std::vector<Immutable<style::LayerProperties>>&) noexcept;

protected:
    optional<std::string> getSource(const style::conversion::Convertible& value) const noexcept;
    bool initSourceLayerAndFilter(style::Layer*, const style::conversion::Convertible& value) const noexcept;
};

} // namespace mbgl
