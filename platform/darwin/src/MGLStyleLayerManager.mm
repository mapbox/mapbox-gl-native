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
    addLayerType(std::make_unique<FillStyleLayerPeerFactory>());
    addLayerType(std::make_unique<LineStyleLayerPeerFactory>());
    addLayerType(std::make_unique<CircleStyleLayerPeerFactory>());
    addLayerType(std::make_unique<SymbolStyleLayerPeerFactory>());
    addLayerType(std::make_unique<RasterStyleLayerPeerFactory>());
    addLayerType(std::make_unique<BackgroundStyleLayerPeerFactory>());
    addLayerType(std::make_unique<HillshadeStyleLayerPeerFactory>());
    addLayerType(std::make_unique<FillExtrusionStyleLayerPeerFactory>());
    addLayerType(std::make_unique<HeatmapStyleLayerPeerFactory>());
    addLayerType(std::make_unique<OpenGLStyleLayerPeerFactory>());
}

LayerManagerDarwin::~LayerManagerDarwin() = default;

MGLStyleLayer* LayerManagerDarwin::createPeer(style::Layer* layer) {
    if (auto* factory = getPeerFactory(layer->getTypeInfo())) {
        return factory->createPeer(layer);
    }
    return nullptr;
}

void LayerManagerDarwin::addLayerType(std::unique_ptr<LayerPeerFactory> factory) {
    auto* coreFactory = factory->getCoreLayerFactory();
    std::string type{coreFactory->getTypeInfo()->type};
    if (!type.empty()) {
        typeToFactory.emplace(std::make_pair(std::move(type), coreFactory));
    }
    factories.emplace_back(std::move(factory));
}

LayerPeerFactory* LayerManagerDarwin::getPeerFactory(const mbgl::style::LayerTypeInfo* typeInfo) {
    assert(typeInfo);
    for (const auto& factory: factories) {
        if (factory->getCoreLayerFactory()->getTypeInfo() == typeInfo) {
            return factory.get();
        }
    }
    assert(false);
    return nullptr;
}

LayerFactory* LayerManagerDarwin::getFactory(const std::string& type) noexcept {
    auto search = typeToFactory.find(type);
    return (search != typeToFactory.end()) ? search->second : nullptr;
}

LayerFactory* LayerManagerDarwin::getFactory(const mbgl::style::LayerTypeInfo* info) noexcept {
    LayerPeerFactory* peerFactory = getPeerFactory(info);
    return (peerFactory != nullptr) ? peerFactory->getCoreLayerFactory() : nullptr;
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

const bool LayerManager::annotationsEnabled = true;

} // namespace mbgl
