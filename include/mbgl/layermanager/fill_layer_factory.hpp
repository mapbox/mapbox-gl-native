#pragma once

#include <mbgl/layermanager/layer_factory.hpp>

namespace mbgl {

class FillLayerFactory : public LayerFactory {
protected:
    const style::LayerTypeInfo* getTypeInfo() const noexcept final;
    std::unique_ptr<style::Layer> createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept final;
    std::unique_ptr<Layout> createLayout(const LayoutParameters&, std::unique_ptr<GeometryTileLayer>, const std::vector<Immutable<style::LayerProperties>>&) noexcept final;
    std::unique_ptr<RenderLayer> createRenderLayer(Immutable<style::Layer::Impl>) noexcept final;
};

} // namespace mbgl
