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
    struct LayerPeerIntitializer {
        mbgl::Map& map;

        Layer* operator()(style::BackgroundLayer& layer) { return new BackgroundLayer(map, layer); }
        Layer* operator()(style::CircleLayer& layer) { return new CircleLayer(map, layer); }
        Layer* operator()(style::FillExtrusionLayer& layer) { return new FillExtrusionLayer(map, layer); }
        Layer* operator()(style::FillLayer& layer) { return new FillLayer(map, layer); }
        Layer* operator()(style::LineLayer& layer) { return new LineLayer(map, layer); }
        Layer* operator()(style::RasterLayer& layer) { return new RasterLayer(map, layer); }
        Layer* operator()(style::SymbolLayer& layer) { return new SymbolLayer(map, layer); }
        Layer* operator()(style::CustomLayer& layer) { return new CustomLayer(map, layer); }
    };

    Layer* layer = coreLayer.accept(LayerPeerIntitializer {map});
    return layer ? layer : new UnknownLayer(map, coreLayer);
}

struct UniqueLayerPeerIntitializer {
    mbgl::Map& map;
    std::unique_ptr<style::Layer> layer;

    template <class LayerType>
    std::unique_ptr<LayerType> cast() {
        return std::unique_ptr<LayerType>(layer.release()->as<LayerType>());
    }

    Layer* operator()(style::BackgroundLayer&) && { return new BackgroundLayer(map, cast<style::BackgroundLayer>()); }
    Layer* operator()(style::CircleLayer&) && { return new CircleLayer(map, cast<style::CircleLayer>()); }
    Layer* operator()(style::FillExtrusionLayer&) && { return new FillExtrusionLayer(map, cast<style::FillExtrusionLayer>()); }
    Layer* operator()(style::FillLayer&) && { return new FillLayer(map, cast<style::FillLayer>()); }
    Layer* operator()(style::LineLayer&) && { return new LineLayer(map, cast<style::LineLayer>()); }
    Layer* operator()(style::RasterLayer&) && { return new RasterLayer(map, cast<style::RasterLayer>()); }
    Layer* operator()(style::SymbolLayer&) && { return new SymbolLayer(map, cast<style::SymbolLayer>()); }
    Layer* operator()(style::CustomLayer&) && { return new CustomLayer(map, cast<style::CustomLayer>()); }
};

static Layer* initializeLayerPeer(Map& map, std::unique_ptr<mbgl::style::Layer> coreLayer) {
    Layer* layer = coreLayer->accept(UniqueLayerPeerIntitializer {map, std::move(coreLayer)});
    return layer ? layer : new UnknownLayer(map, std::move(coreLayer));
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
