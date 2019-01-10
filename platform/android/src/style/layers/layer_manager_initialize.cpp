#include "layer_manager.hpp"

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

void LayerManagerAndroid::initialize() {
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

} // namespace android

const bool LayerManager::annotationsEnabled = true;

} // namespace mbgl
