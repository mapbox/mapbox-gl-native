#include "layer_manager.hpp"

#include <mbgl/style/layer_impl.hpp>

#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>

#include "background_layer.hpp"
#include "circle_layer.hpp"
#include "custom_layer.hpp"
#include "fill_extrusion_layer.hpp"
#include "fill_layer.hpp"
#include "heatmap_layer.hpp"
#include "hillshade_layer.hpp"
#include "line_layer.hpp"
#include "raster_layer.hpp"
#include "symbol_layer.hpp"
#include "fill_extrusion_layer.hpp"

namespace mbgl {

namespace android {

LayerManagerAndroid::LayerManagerAndroid() {
    addLayerType(std::make_unique<FillJavaLayerPeerFactory>());
    addLayerType(std::make_unique<LineJavaLayerPeerFactory>());
    addLayerType(std::make_unique<CircleJavaLayerPeerFactory>());
    addLayerType(std::make_unique<SymbolJavaLayerPeerFactory>());
    addLayerType(std::make_unique<RasterJavaLayerPeerFactory>());
    addLayerType(std::make_unique<BackgroundJavaLayerPeerFactory>());
    addLayerType(std::make_unique<HillshadeJavaLayerPeerFactory>());
    addLayerType(std::make_unique<FillExtrusionJavaLayerPeerFactory>());
    addLayerType(std::make_unique<HeatmapJavaLayerPeerFactory>());
    addLayerType(std::make_unique<CustomJavaLayerPeerFactory>());
}

LayerManagerAndroid::~LayerManagerAndroid() = default;

jni::Local<jni::Object<Layer>> LayerManagerAndroid::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, mbgl::style::Layer& layer) {
    if (JavaLayerPeerFactory* factory = getPeerFactory(layer.getTypeInfo())) {
        return factory->createJavaLayerPeer(env, map, layer);
    }
    return jni::Local<jni::Object<Layer>>();
}

jni::Local<jni::Object<Layer>> LayerManagerAndroid::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> layer) {
    if (JavaLayerPeerFactory* factory = getPeerFactory(layer->getTypeInfo())) {
        return factory->createJavaLayerPeer(env, map, std::move(layer));
    }
    return jni::Local<jni::Object<Layer>>();
}

void LayerManagerAndroid::registerNative(jni::JNIEnv& env) {
    Layer::registerNative(env);
    for (const auto& factory: factories) {
        factory->registerNative(env);
    }
}

void LayerManagerAndroid::addLayerType(std::unique_ptr<JavaLayerPeerFactory> factory) {
    auto* coreFactory = factory->getLayerFactory();
    std::string type{coreFactory->getTypeInfo()->type};
    if (!type.empty()) {
        typeToFactory.emplace(std::make_pair(std::move(type), coreFactory));
    }
    factories.emplace_back(std::move(factory));
}

JavaLayerPeerFactory* LayerManagerAndroid::getPeerFactory(const mbgl::style::LayerTypeInfo* typeInfo) {
    assert(typeInfo);
    for (const auto& factory: factories) {
        if (factory->getLayerFactory()->getTypeInfo() == typeInfo) {
            return factory.get();
        }
    }
    assert(false);
    return nullptr;
}

LayerFactory* LayerManagerAndroid::getFactory(const std::string& type) noexcept {
    auto search = typeToFactory.find(type);
    return (search != typeToFactory.end()) ? search->second : nullptr;
}

LayerFactory* LayerManagerAndroid::getFactory(const mbgl::style::LayerTypeInfo* info) noexcept {
    JavaLayerPeerFactory* peerFactory = getPeerFactory(info);
    return (peerFactory != nullptr) ? peerFactory->getLayerFactory() : nullptr;
}

// static 
LayerManagerAndroid* LayerManagerAndroid::get() noexcept {
    static LayerManagerAndroid impl;
    return &impl;
}

} // namespace android

LayerManager* LayerManager::get() noexcept {
    return android::LayerManagerAndroid::get();
}

const bool LayerManager::annotationsEnabled = true;

} // namespace mbgl
