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

// Mapping from style layers to peer classes
template <class> struct PeerType {};
template <> struct PeerType<style::BackgroundLayer> { using Type = android::BackgroundLayer; };
template <> struct PeerType<style::CircleLayer> { using Type = android::CircleLayer; };
template <> struct PeerType<style::FillExtrusionLayer> { using Type = android::FillExtrusionLayer; };
template <> struct PeerType<style::FillLayer> { using Type = android::FillLayer; };
template <> struct PeerType<style::HeatmapLayer> { using Type = android::HeatmapLayer; };
template <> struct PeerType<style::HillshadeLayer> { using Type = android::HillshadeLayer; };
template <> struct PeerType<style::LineLayer> { using Type = android::LineLayer; };
template <> struct PeerType<style::RasterLayer> { using Type = android::RasterLayer; };
template <> struct PeerType<style::SymbolLayer> { using Type = android::SymbolLayer; };
template <> struct PeerType<style::CustomLayer> { using Type = android::CustomLayer; };

// Inititalizes a non-owning peer
struct LayerPeerIntitializer {
    mbgl::Map& map;

    template <class LayerType>
    Layer* operator()(LayerType& layer) {
        return new typename PeerType<LayerType>::Type(map, layer);
    }
};

static Layer* initializeLayerPeer(mbgl::Map& map, mbgl::style::Layer& coreLayer) {
    Layer* layer = coreLayer.accept(LayerPeerIntitializer {map});
    return layer ? layer : new UnknownLayer(map, coreLayer);
}

// Initializes an owning peer
// Only usable once since it needs to pass on ownership
// of the given layer and thus enforced to be an rvalue
struct UniqueLayerPeerIntitializer {
    mbgl::Map& map;
    std::unique_ptr<style::Layer> layer;

    template <class LayerType>
    Layer* operator()(LayerType&) && {
        return new typename PeerType<LayerType>::Type(
                map,
                std::unique_ptr<LayerType>(layer.release()->as<LayerType>())
        );
    }
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
    HeatmapLayer::registerNative(env);
    HillshadeLayer::registerNative(env);
    LineLayer::registerNative(env);
    RasterLayer::registerNative(env);
    SymbolLayer::registerNative(env);
    UnknownLayer::registerNative(env);
}

} // namespace android
} // namespace mbgl
