#include <mbgl/style/layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>

#include <map>
#include <memory>
#include <vector>

namespace mbgl {
namespace style {
 
class LayerManagerBase : public LayerManager {
public:
    LayerManagerBase();

private:
    void addLayerType(std::unique_ptr<LayerFactory>);
    // LayerManager overrides.
    std::unique_ptr<Layer> createLayer(const std::string& type, const std::string& id, const conversion::Convertible& value, conversion::Error& error) noexcept final;

    std::vector<std::unique_ptr<LayerFactory>> factories;
    std::map<std::string, LayerFactory*> typeToFactory;
};

LayerManagerBase::LayerManagerBase() {
    addLayerType(std::make_unique<FillLayerFactory>());
    addLayerType(std::make_unique<LineLayerFactory>());
    addLayerType(std::make_unique<CircleLayerFactory>());
    addLayerType(std::make_unique<SymbolLayerFactory>());
    addLayerType(std::make_unique<RasterLayerFactory>());
    addLayerType(std::make_unique<BackgroundLayerFactory>());
    addLayerType(std::make_unique<HillshadeLayerFactory>());
    addLayerType(std::make_unique<FillExtrusionLayerFactory>());
    addLayerType(std::make_unique<HeatmapLayerFactory>());
    addLayerType(std::make_unique<CustomLayerFactory>());
}

void LayerManagerBase::addLayerType(std::unique_ptr<LayerFactory> factory) {
    std::string type{factory->getTypeInfo()->type};
    if (!type.empty()) {
        typeToFactory.emplace(std::make_pair(std::move(type), factory.get()));
    }
    factories.emplace_back(std::move(factory));
}

std::unique_ptr<Layer> LayerManagerBase::createLayer(const std::string& type,
                                                     const std::string& id,
                                                     const conversion::Convertible& value,
                                                     conversion::Error& error) noexcept {
    auto search = typeToFactory.find(type);
    if (search != typeToFactory.end()) {
        auto layer = search->second->createLayer(id, value);
        if (!layer) {
            error.message = "Error parsing a layer of type: " + type;
        }
        return layer;
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
