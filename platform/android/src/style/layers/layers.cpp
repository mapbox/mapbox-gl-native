#include "layers.hpp"

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>

#include "background_layer.hpp"
#include "circle_layer.hpp"
#include "fill_layer.hpp"
#include "line_layer.hpp"
#include "raster_layer.hpp"
#include "symbol_layer.hpp"
#include "custom_layer.hpp"

namespace mbgl {
namespace android {

Layer* initializeLayerPeer(mbgl::Map& map, mbgl::style::Layer& coreLayer) {
    Layer* layer;
    if (coreLayer.is<mbgl::style::BackgroundLayer>()) {
        layer = new BackgroundLayer(map, *coreLayer.as<mbgl::style::BackgroundLayer>());
    } else if (coreLayer.is<mbgl::style::CircleLayer>()) {
        layer = new CircleLayer(map, *coreLayer.as<mbgl::style::CircleLayer>());
    } else if (coreLayer.is<mbgl::style::FillLayer>()) {
            layer = new FillLayer(map, *coreLayer.as<mbgl::style::FillLayer>());
    } else if (coreLayer.is<mbgl::style::LineLayer>()) {
            layer = new LineLayer(map, *coreLayer.as<mbgl::style::LineLayer>());
    } else if (coreLayer.is<mbgl::style::RasterLayer>()) {
            layer = new RasterLayer(map, *coreLayer.as<mbgl::style::RasterLayer>());
    } else if (coreLayer.is<mbgl::style::SymbolLayer>()) {
        layer = new SymbolLayer(map, *coreLayer.as<mbgl::style::SymbolLayer>());
    } else if (coreLayer.is<mbgl::style::CustomLayer>()) {
        layer = new CustomLayer(map, *coreLayer.as<mbgl::style::CustomLayer>());
    } else {
        throw new std::runtime_error("Layer type not implemented");
    }

    return layer;
}

jni::jobject* createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, mbgl::style::Layer& coreLayer) {
    std::unique_ptr<Layer> peerLayer = std::unique_ptr<Layer>(initializeLayerPeer(map, coreLayer));
    jni::jobject* result = peerLayer->createJavaPeer(env);
    peerLayer.release();
    return result;
}

void registerNativeLayers(jni::JNIEnv& env) {
    Layer::registerNative(env);
    BackgroundLayer::registerNative(env);
    CircleLayer::registerNative(env);
    FillLayer::registerNative(env);
    LineLayer::registerNative(env);
    RasterLayer::registerNative(env);
    SymbolLayer::registerNative(env);
    CustomLayer::registerNative(env);
}

} //android
} //mbgl