#include <mbgl/layermanager/layer_manager.hpp>

#include <mbgl/layermanager/background_layer_factory.hpp>
#include <mbgl/layermanager/circle_layer_factory.hpp>
#include <mbgl/layermanager/custom_layer_factory.hpp>
#include <mbgl/layermanager/fill_extrusion_layer_factory.hpp>
#include <mbgl/layermanager/fill_layer_factory.hpp>
#include <mbgl/layermanager/heatmap_layer_factory.hpp>
#include <mbgl/layermanager/hillshade_layer_factory.hpp>
#include <mbgl/layermanager/line_layer_factory.hpp>
#include <mbgl/layermanager/raster_layer_factory.hpp>
#include <mbgl/layermanager/symbol_layer_factory.hpp>

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
#if !defined(MBGL_LAYER_FILL_DISABLE_ALL)
    addLayerType(std::make_unique<FillLayerFactory>());
#endif
#if !defined(MBGL_LAYER_LINE_DISABLE_ALL)
    addLayerType(std::make_unique<LineLayerFactory>());
#endif
#if !defined(MBGL_LAYER_CIRCLE_DISABLE_ALL)
    addLayerType(std::make_unique<CircleLayerFactory>());
#endif
#if !defined(MBGL_LAYER_SYMBOL_DISABLE_ALL)
    addLayerType(std::make_unique<SymbolLayerFactory>());
#endif
#if !defined(MBGL_LAYER_RASTER_DISABLE_ALL)
    addLayerType(std::make_unique<RasterLayerFactory>());
#endif
#if !defined(MBGL_LAYER_BACKGROUND_DISABLE_ALL)
    addLayerType(std::make_unique<BackgroundLayerFactory>());
#endif
#if !defined(MBGL_LAYER_HILLSHADE_DISABLE_ALL)
    addLayerType(std::make_unique<HillshadeLayerFactory>());
#endif
#if !defined(MBGL_LAYER_FILL_EXTRUSION_DISABLE_ALL)
    addLayerType(std::make_unique<FillExtrusionLayerFactory>());
#endif
#if !defined(MBGL_LAYER_HEATMAP_DISABLE_ALL)
    addLayerType(std::make_unique<HeatmapLayerFactory>());
#endif
#if !defined(MBGL_LAYER_CUSTOM_DISABLE_ALL)
    addLayerType(std::make_unique<CustomLayerFactory>());
#endif
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

#if defined(MBGL_LAYER_LINE_DISABLE_ALL) || defined(MBGL_LAYER_SYMBOL_DISABLE_ALL) || defined(MBGL_LAYER_FILL_DISABLE_ALL)
const bool LayerManager::annotationsEnabled = false;
#else
const bool LayerManager::annotationsEnabled = true;
#endif

} // namespace mbgl
