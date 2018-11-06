#include "layers.hpp"

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
    factories.emplace_back(std::unique_ptr<JavaLayerPeerFactory>(new FillJavaLayerPeerFactory));
    factories.emplace_back(std::unique_ptr<JavaLayerPeerFactory>(new LineJavaLayerPeerFactory));
    factories.emplace_back(std::unique_ptr<JavaLayerPeerFactory>(new CircleJavaLayerPeerFactory));
    factories.emplace_back(std::unique_ptr<JavaLayerPeerFactory>(new SymbolJavaLayerPeerFactory));
    factories.emplace_back(std::unique_ptr<JavaLayerPeerFactory>(new RasterJavaLayerPeerFactory));
    factories.emplace_back(std::unique_ptr<JavaLayerPeerFactory>(new BackgroundJavaLayerPeerFactory));
    factories.emplace_back(std::unique_ptr<JavaLayerPeerFactory>(new HillshadeJavaLayerPeerFactory));
    factories.emplace_back(std::unique_ptr<JavaLayerPeerFactory>(new FillExtrusionJavaLayerPeerFactory));
    factories.emplace_back(std::unique_ptr<JavaLayerPeerFactory>(new HeatmapJavaLayerPeerFactory));
    factories.emplace_back(std::unique_ptr<JavaLayerPeerFactory>(new CustomJavaLayerPeerFactory));
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

JavaLayerPeerFactory* LayerManagerAndroid::getPeerFactory(mbgl::style::Layer* layer) {
    auto* layerFactory = layer->baseImpl->getLayerFactory();
    assert(layerFactory);
    for (const auto& factory: factories) {
        if (factory->getLayerFactory() == layerFactory) {
            return factory.get();
        }
    }
    assert(false);
    return nullptr;
}

std::unique_ptr<style::Layer> LayerManagerAndroid::createLayer(const std::string& type, const std::string& id, const style::conversion::Convertible& value, style::conversion::Error& error) {
    for (const auto& factory: factories) {
        auto* layerFactory = factory->getLayerFactory();
        if (layerFactory->supportsType(type)) {
            if (auto layer = layerFactory->createLayer(id, value)) {
                return layer;
            }
            error.message = "Error parsing a layer of type: " + type;
            return nullptr;
        }
    }
    error.message = "Unsupported layer type: " + type;
    return nullptr;
}

// static 
LayerManagerAndroid* LayerManagerAndroid::get() {
    static LayerManagerAndroid impl;
    return &impl;
}

} // namespace android

namespace style {
// static 
LayerManager* LayerManager::get() {
    return android::LayerManagerAndroid::get();
}

} // style
} // namespace mbgl
