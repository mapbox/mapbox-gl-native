#include <mbgl/layermanager/layer_manager.hpp>

#include <mbgl/layermanager/layer_factory.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {

std::unique_ptr<style::Layer> LayerManager::createLayer(
    const std::string& type, const std::string& id,
    const style::conversion::Convertible& value, style::conversion::Error& error) noexcept {
    if (LayerFactory* factory = getFactory(type)) {
        auto layer = factory->createLayer(id, value);
        if (!layer) {
            error.message = "Error parsing a layer of type: " + type;
        }
        return layer;
    }
    error.message = "Unsupported layer type: " + type;
    return nullptr;
}

std::unique_ptr<RenderLayer> LayerManager::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    LayerFactory* factory = getFactory(impl->getTypeInfo());
    assert(factory);
    return factory->createRenderLayer(std::move(impl));
}

} // namespace mbgl
