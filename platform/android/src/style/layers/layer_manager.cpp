#include "layer_manager.hpp"

#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>

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
    if (JavaLayerPeerFactory* factory = getPeerFactory(&layer)) {
        return factory->createJavaLayerPeer(env, map, layer);
    }
    return jni::Local<jni::Object<Layer>>();
}

jni::Local<jni::Object<Layer>> LayerManagerAndroid::createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> layer) {
    if (JavaLayerPeerFactory* factory = getPeerFactory(layer.get())) {
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

JavaLayerPeerFactory* LayerManagerAndroid::getPeerFactory(mbgl::style::Layer* layer) {
    auto* typeInfo = layer->baseImpl->getTypeInfo();
    assert(typeInfo);
    for (const auto& factory: factories) {
        if (factory->getLayerFactory()->getTypeInfo() == typeInfo) {
            return factory.get();
        }
    }
    assert(false);
    return nullptr;
}

std::unique_ptr<style::Layer> LayerManagerAndroid::createLayer(const std::string& type,
                                                               const std::string& id,
                                                               const style::conversion::Convertible& value,
                                                               style::conversion::Error& error) noexcept {
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
LayerManagerAndroid* LayerManagerAndroid::get() noexcept {
    static LayerManagerAndroid impl;
    return &impl;
}

} // namespace android

namespace style {
// static 
LayerManager* LayerManager::get() noexcept {
    return android::LayerManagerAndroid::get();
}

} // style
} // namespace mbgl
