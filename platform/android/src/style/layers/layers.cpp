#include "layers.hpp"

#include <mbgl/style/layer.hpp>
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
#include "unknown_layer.hpp"
#include "fill_extrusion_layer.hpp"

namespace mbgl {

namespace android {

template <typename T>
inline std::unique_ptr<T> to(std::unique_ptr<style::Layer> layer) {
    return std::unique_ptr<T>(static_cast<T*>(layer.release()));
}

template <typename T>
inline T& to(style::Layer& layer) {
    return static_cast<T&>(layer);
}

template <typename T>
std::unique_ptr<Layer> initializeLayerPeer(Map& map, style::LayerType type, T&& layer) {
    switch (type) {
    case style::LayerType::Fill:
        return std::unique_ptr<Layer>(new FillLayer(map, to<style::FillLayer>(std::forward<T>(layer))));
    case style::LayerType::Line:
        return std::unique_ptr<Layer>(new LineLayer(map, to<style::LineLayer>(std::forward<T>(layer))));
    case style::LayerType::Circle:
        return std::unique_ptr<Layer>(new CircleLayer(map, to<style::CircleLayer>(std::forward<T>(layer))));
    case style::LayerType::Symbol:
        return std::unique_ptr<Layer>(new SymbolLayer(map, to<style::SymbolLayer>(std::forward<T>(layer))));
    case style::LayerType::Raster:
        return std::unique_ptr<Layer>(new RasterLayer(map, to<style::RasterLayer>(std::forward<T>(layer))));
    case style::LayerType::Background:
        return std::unique_ptr<Layer>(new BackgroundLayer(map, to<style::BackgroundLayer>(std::forward<T>(layer))));
    case style::LayerType::Hillshade:
        return std::unique_ptr<Layer>(new HillshadeLayer(map, to<style::HillshadeLayer>(std::forward<T>(layer))));
    case style::LayerType::Custom:
        return std::unique_ptr<Layer>(new CustomLayer(map, to<style::CustomLayer>(std::forward<T>(layer))));
    case style::LayerType::FillExtrusion:
        return std::unique_ptr<Layer>(new FillExtrusionLayer(map, to<style::FillExtrusionLayer>(std::forward<T>(layer))));
    case style::LayerType::Heatmap:
        return std::unique_ptr<Layer>(new HeatmapLayer(map, to<style::HeatmapLayer>(std::forward<T>(layer))));
    }
    // Not reachable, but placate GCC.
    assert(false);
    return std::unique_ptr<Layer>(new UnknownLayer(map, std::forward<T>(layer)));
}

jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, Map& map, style::Layer& coreLayer) {
    std::unique_ptr<Layer> peerLayer = initializeLayerPeer(map, coreLayer.getType(), coreLayer);
    jni::Local<jni::Object<Layer>> result = peerLayer->createJavaPeer(env);
    peerLayer.release();
    return result;
}

jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer> coreLayer) {
    auto type = coreLayer->getType();
    std::unique_ptr<Layer> peerLayer = initializeLayerPeer(map, type, std::move(coreLayer));
    jni::Local<jni::Object<Layer>> result = peerLayer->createJavaPeer(env);
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
    HeatmapLayer::registerNative(env);
    HillshadeLayer::registerNative(env);
    LineLayer::registerNative(env);
    RasterLayer::registerNative(env);
    SymbolLayer::registerNative(env);
    UnknownLayer::registerNative(env);
}

} // namespace android
} // namespace mbgl
