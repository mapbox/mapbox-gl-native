#import "MGLStyleLayerManager.h"

#import "MGLBackgroundStyleLayer_Private.h"
#import "MGLCircleStyleLayer_Private.h"
#import "MGLFillExtrusionStyleLayer_Private.h"
#import "MGLFillStyleLayer_Private.h"
#import "MGLHeatmapStyleLayer_Private.h"
#import "MGLHillshadeStyleLayer_Private.h"
#import "MGLLineStyleLayer_Private.h"
#import "MGLRasterStyleLayer_Private.h"
#import "MGLSymbolStyleLayer_Private.h"
#import "MGLOpenGLStyleLayer_Private.h"

#include <vector>

namespace mbgl {

LayerManagerDarwin::LayerManagerDarwin() {
#if defined(MBGL_LAYER_FILL_DISABLE_RUNTIME)
    addLayerTypeCoreOnly(std::make_unique<FillLayerFactory>());
#elif !defined(MBGL_LAYER_FILL_DISABLE_ALL)
    addLayerType(std::make_unique<FillStyleLayerPeerFactory>());
#endif
#if defined(MBGL_LAYER_LINE_DISABLE_RUNTIME)
    addLayerTypeCoreOnly(std::make_unique<LineLayerFactory>());
#elif !defined(MBGL_LAYER_LINE_DISABLE_ALL)
    addLayerType(std::make_unique<LineStyleLayerPeerFactory>());
#endif
#if defined(MBGL_LAYER_CIRCLE_DISABLE_RUNTIME)
    addLayerTypeCoreOnly(std::make_unique<CircleLayerFactory>());
#elif !defined(MBGL_LAYER_CIRCLE_DISABLE_ALL)
    addLayerType(std::make_unique<CircleStyleLayerPeerFactory>());
#endif
#if defined(MBGL_LAYER_SYMBOL_DISABLE_RUNTIME)
    addLayerTypeCoreOnly(std::make_unique<SymbolLayerFactory>());
#elif !defined(MBGL_LAYER_SYMBOL_DISABLE_ALL)
    addLayerType(std::make_unique<SymbolStyleLayerPeerFactory>());
#endif
#if defined(MBGL_LAYER_RASTER_DISABLE_RUNTIME)
    addLayerTypeCoreOnly(std::make_unique<RasterLayerFactory>());
#elif !defined(MBGL_LAYER_RASTER_DISABLE_ALL)
    addLayerType(std::make_unique<RasterStyleLayerPeerFactory>());
#endif
#if defined(MBGL_LAYER_BACKGROUND_DISABLE_RUNTIME)
    addLayerTypeCoreOnly(std::make_unique<BackgroundLayerFactory>());
#elif !defined(MBGL_LAYER_BACKGROUND_DISABLE_ALL)
    addLayerType(std::make_unique<BackgroundStyleLayerPeerFactory>());
#endif
#if defined(MBGL_LAYER_HILLSHADE_DISABLE_RUNTIME)
    addLayerTypeCoreOnly(std::make_unique<HillshadeLayerFactory>());
#elif !defined(MBGL_LAYER_HILLSHADE_DISABLE_ALL)
    addLayerType(std::make_unique<HillshadeStyleLayerPeerFactory>());
#endif
#if defined(MBGL_LAYER_FILL_EXTRUSION_DISABLE_RUNTIME)
    addLayerTypeCoreOnly(std::make_unique<FillExtrusionLayerFactory>());
#elif !defined(MBGL_LAYER_FILL_EXTRUSION_DISABLE_ALL)
    addLayerType(std::make_unique<FillExtrusionStyleLayerPeerFactory>());
#endif
#if defined(MBGL_LAYER_HEATMAP_DISABLE_RUNTIME)
    addLayerTypeCoreOnly(std::make_unique<HeatmapLayerFactory>());
#elif !defined(MBGL_LAYER_HEATMAP_DISABLE_ALL)
    addLayerType(std::make_unique<HeatmapStyleLayerPeerFactory>());
#endif
#if defined(MBGL_LAYER_CUSTOM_DISABLE_RUNTIME)
    addLayerTypeCoreOnly(std::make_unique<CustomLayerFactory>());
#elif !defined(MBGL_LAYER_CUSTOM_DISABLE_ALL)
    addLayerType(std::make_unique<OpenGLStyleLayerPeerFactory>());
#endif
}

LayerManagerDarwin::~LayerManagerDarwin() = default;

MGLStyleLayer* LayerManagerDarwin::createPeer(style::Layer* layer) {
    if (auto* factory = getPeerFactory(layer->getTypeInfo())) {
        return factory->createPeer(layer);
    }
    return nullptr;
}

void LayerManagerDarwin::addLayerType(std::unique_ptr<LayerPeerFactory> factory) {
    NSCAssert(getFactory(factory->getCoreLayerFactory()->getTypeInfo()) == nullptr,
              @"A layer factory with the given info is already added.");
    registerCoreFactory(factory->getCoreLayerFactory());
    peerFactories.emplace_back(std::move(factory));
}

void LayerManagerDarwin::addLayerTypeCoreOnly(std::unique_ptr<LayerFactory> factory) {
    NSCAssert(getFactory(factory->getTypeInfo()) == nullptr,
              @"A layer factory with the given info is already added.");
    registerCoreFactory(factory.get());
    coreFactories.emplace_back(std::move(factory));
}

void LayerManagerDarwin::registerCoreFactory(LayerFactory* factory) {
    std::string type{factory->getTypeInfo()->type};
    if (!type.empty()) {
        NSCAssert(typeToFactory.find(type) == typeToFactory.end(), @"A layer type can be registered only once.");
        typeToFactory.emplace(std::make_pair(std::move(type), factory));
    }
}

LayerPeerFactory* LayerManagerDarwin::getPeerFactory(const mbgl::style::LayerTypeInfo* typeInfo) {
    for (const auto& factory: peerFactories) {
        if (factory->getCoreLayerFactory()->getTypeInfo() == typeInfo) {
            return factory.get();
        }
    }
    return nullptr;
}

LayerFactory* LayerManagerDarwin::getFactory(const std::string& type) noexcept {
    auto search = typeToFactory.find(type);
    return (search != typeToFactory.end()) ? search->second : nullptr;
}

LayerFactory* LayerManagerDarwin::getFactory(const mbgl::style::LayerTypeInfo* info) noexcept {
    if (LayerPeerFactory* peerFactory = getPeerFactory(info)) {
        return peerFactory->getCoreLayerFactory();
    }

    for (const auto& factory: coreFactories) {
        if (factory->getTypeInfo() == info) {
            return factory.get();
        }
    }

    return nullptr;
}

// static
LayerManagerDarwin* LayerManagerDarwin::get() noexcept {
    static LayerManagerDarwin impl;
    return &impl;
}

// static
LayerManager* LayerManager::get() noexcept {
    return LayerManagerDarwin::get();
}

#if defined(MBGL_LAYER_LINE_DISABLE_ALL) || defined(MBGL_LAYER_SYMBOL_DISABLE_ALL) || defined(MBGL_LAYER_FILL_DISABLE_ALL)
const bool LayerManager::annotationsEnabled = false;
#else
const bool LayerManager::annotationsEnabled = true;
#endif

} // namespace mbgl
