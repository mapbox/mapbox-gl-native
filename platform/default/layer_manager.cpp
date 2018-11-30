#include <mbgl/layermanager/layer_manager.hpp>
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
 
class LayerManagerDefault final : public LayerManager {
public:
    LayerManagerDefault();

private:
    void addLayerType(std::unique_ptr<LayerFactory>);
    // LayerManager overrides.
    LayerFactory* getFactory(const std::string& type) noexcept final;
    LayerFactory* getFactory(const style::LayerTypeInfo*) noexcept final;

    std::vector<std::unique_ptr<LayerFactory>> factories;
    std::map<std::string, LayerFactory*> typeToFactory;
};

LayerManagerDefault::LayerManagerDefault() {
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

void LayerManagerDefault::addLayerType(std::unique_ptr<LayerFactory> factory) {
    std::string type{factory->getTypeInfo()->type};
    if (!type.empty()) {
        typeToFactory.emplace(std::make_pair(std::move(type), factory.get()));
    }
    factories.emplace_back(std::move(factory));
}

LayerFactory* LayerManagerDefault::getFactory(const mbgl::style::LayerTypeInfo* typeInfo) noexcept {
    assert(typeInfo);
    for (const auto& factory: factories) {
        if (factory->getTypeInfo() == typeInfo) {
            return factory.get();
        }
    }
    assert(false);
    return nullptr;
}

LayerFactory* LayerManagerDefault::getFactory(const std::string& type) noexcept {
    auto search = typeToFactory.find(type);
    return (search != typeToFactory.end()) ? search->second : nullptr;
}

// static 
LayerManager* LayerManager::get() noexcept {
    static LayerManagerDefault instance;
    return &instance;
}

const bool LayerManager::annotationsEnabled = true;

} // namespace mbgl
