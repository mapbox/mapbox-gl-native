#include <mbgl/style/layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>

#include <memory>
#include <vector>

namespace mbgl {
namespace style {
 
class LayerManagerBase : public LayerManager {
public:
    LayerManagerBase();
private:
    // LayerManager overrides.
    std::unique_ptr<Layer> createLayer(const std::string& type, const std::string& id, const conversion::Convertible& value, conversion::Error& error) noexcept final;
    std::vector<std::unique_ptr<LayerFactory>> factories;
};

LayerManagerBase::LayerManagerBase() {
    factories.emplace_back(std::unique_ptr<LayerFactory>(new FillLayerFactory));
    factories.emplace_back(std::unique_ptr<LayerFactory>(new LineLayerFactory));
    factories.emplace_back(std::unique_ptr<LayerFactory>(new CircleLayerFactory));
    factories.emplace_back(std::unique_ptr<LayerFactory>(new SymbolLayerFactory));
    factories.emplace_back(std::unique_ptr<LayerFactory>(new RasterLayerFactory));
    factories.emplace_back(std::unique_ptr<LayerFactory>(new BackgroundLayerFactory));
    factories.emplace_back(std::unique_ptr<LayerFactory>(new HillshadeLayerFactory));
    factories.emplace_back(std::unique_ptr<LayerFactory>(new FillExtrusionLayerFactory));
    factories.emplace_back(std::unique_ptr<LayerFactory>(new HeatmapLayerFactory));
}

std::unique_ptr<Layer> LayerManagerBase::createLayer(const std::string& type,
                                                     const std::string& id,
                                                     const conversion::Convertible& value,
                                                     conversion::Error& error) noexcept {
    for (const auto& factory: factories) {
        if (factory->supportsType(type)) {
            auto layer = factory->createLayer(id, value);
            if (!layer) {
                error.message = "Error parsing a layer of type: " + type;
            }
            return layer;
        }
    }
    error.message = "Unsupported layer type: " + type;
    return nullptr;
}

// static 
LayerManager* LayerManager::get() noexcept {
    static LayerManagerBase impl;
    return &impl;
}

} // namespace style
} // namespace mbgl
