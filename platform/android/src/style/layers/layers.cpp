#include "layers.hpp"

#include <mbgl/style/layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>

#include "background_layer.hpp"
#include "circle_layer.hpp"
#include "custom_layer.hpp"
#include "fill_extrusion_layer.hpp"
#include "fill_layer.hpp"
#include "line_layer.hpp"
#include "raster_layer.hpp"
#include "symbol_layer.hpp"
#include "unknown_layer.hpp"
#include "fill_extrusion_layer.hpp"

namespace mbgl {
namespace android {

static Layer* initializeLayerPeer(mbgl::Map& map, mbgl::style::Layer& coreLayer) {
    if (coreLayer.is<mbgl::style::BackgroundLayer>()) {
        return new BackgroundLayer(map, *coreLayer.as<mbgl::style::BackgroundLayer>());
    } else if (coreLayer.is<mbgl::style::CircleLayer>()) {
        return new CircleLayer(map, *coreLayer.as<mbgl::style::CircleLayer>());
    } else if (coreLayer.is<mbgl::style::FillExtrusionLayer>()) {
        return new FillExtrusionLayer(map, *coreLayer.as<mbgl::style::FillExtrusionLayer>());
    } else if (coreLayer.is<mbgl::style::FillLayer>()) {
        return new FillLayer(map, *coreLayer.as<mbgl::style::FillLayer>());
    } else if (coreLayer.is<mbgl::style::LineLayer>()) {
        return new LineLayer(map, *coreLayer.as<mbgl::style::LineLayer>());
    } else if (coreLayer.is<mbgl::style::RasterLayer>()) {
        return new RasterLayer(map, *coreLayer.as<mbgl::style::RasterLayer>());
    } else if (coreLayer.is<mbgl::style::SymbolLayer>()) {
        return new SymbolLayer(map, *coreLayer.as<mbgl::style::SymbolLayer>());
    } else if (coreLayer.is<mbgl::style::CustomLayer>()) {
        return new CustomLayer(map, *coreLayer.as<mbgl::style::CustomLayer>());
    } else {
        return new UnknownLayer(map, coreLayer);
    }
}

template <class LayerType, class PeerType>
static PeerType* createPeer(Map& map, std::unique_ptr<mbgl::style::Layer> layer) {
    return new PeerType(map, std::move(std::unique_ptr<LayerType>(layer.release()->as<LayerType>())));
}

static Layer* initializeLayerPeer(Map& map, std::unique_ptr<mbgl::style::Layer> coreLayer) {
    if (coreLayer->is<style::BackgroundLayer>()) {
        return createPeer<style::BackgroundLayer, BackgroundLayer>(map, std::move(coreLayer));
    } else if (coreLayer->is<style::CircleLayer>()) {
        return createPeer<style::CircleLayer, CircleLayer>(map, std::move(coreLayer));
    } else if (coreLayer->is<style::FillExtrusionLayer>()) {
        return createPeer<style::FillExtrusionLayer, FillExtrusionLayer>(map, std::move(coreLayer));
    } else if (coreLayer->is<style::FillLayer>()) {
        return createPeer<style::FillLayer, FillLayer>(map, std::move(coreLayer));
    } else if (coreLayer->is<style::LineLayer>()) {
        return createPeer<style::LineLayer, LineLayer>(map, std::move(coreLayer));
    } else if (coreLayer->is<style::RasterLayer>()) {
        return createPeer<style::RasterLayer, RasterLayer>(map, std::move(coreLayer));
    } else if (coreLayer->is<style::SymbolLayer>()) {
        return createPeer<style::SymbolLayer, SymbolLayer>(map, std::move(coreLayer));
    } else if (coreLayer->is<mbgl::style::CustomLayer>()) {
        return createPeer<style::SymbolLayer, SymbolLayer>(map, std::move(coreLayer));
    } else {
        return new UnknownLayer(map, std::move(coreLayer));
    }
}

jni::jobject* createJavaLayerPeer(jni::JNIEnv& env, Map& map, style::Layer& coreLayer) {
    std::unique_ptr<Layer> peerLayer = std::unique_ptr<Layer>(initializeLayerPeer(map, coreLayer));
    jni::jobject* result = peerLayer->createJavaPeer(env);
    peerLayer.release();
    return result;
}

jni::jobject* createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> coreLayer) {
    std::unique_ptr<Layer> peerLayer = std::unique_ptr<Layer>(initializeLayerPeer(map, std::move(coreLayer)));
    jni::jobject* result = peerLayer->createJavaPeer(env);
    peerLayer.release();
    return result;
}

void registerNativeLayers(jni::JNIEnv& env) {
    Layer::registerNative(env);
    BackgroundLayer::registerNative(env);
    CircleLayer::registerNative(env);
    CustomLayer::registerNative(env);
    FillExtrusionLayer::registerNative(env);
    FillLayer::registerNative(env);
    LineLayer::registerNative(env);
    RasterLayer::registerNative(env);
    SymbolLayer::registerNative(env);
    UnknownLayer::registerNative(env);
}

} // namespace android
} // namespace mbgl
