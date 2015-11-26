#include <mbgl/style/style_layer.hpp>
#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/layer/line_layer.hpp>
#include <mbgl/layer/circle_layer.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/layer/background_layer.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> StyleLayer::create(StyleLayerType type) {
    switch (type) {
    case StyleLayerType::Fill:
        return std::make_unique<FillLayer>();
    case StyleLayerType::Line:
        return std::make_unique<LineLayer>();
    case StyleLayerType::Circle:
        return std::make_unique<CircleLayer>();
    case StyleLayerType::Symbol:
        return std::make_unique<SymbolLayer>();
    case StyleLayerType::Raster:
        return std::make_unique<RasterLayer>();
    case StyleLayerType::Background:
        return std::make_unique<BackgroundLayer>();
    default:
        return nullptr;
    }
}

const std::string& StyleLayer::bucketName() const {
    return ref.empty() ? id : ref;
}

bool StyleLayer::hasRenderPass(RenderPass pass) const {
    return bool(passes & pass);
}

void StyleLayer::copy(const StyleLayer& src) {
    type = src.type;
    source = src.source;
    sourceLayer = src.sourceLayer;
    filter = src.filter;
    minZoom = src.minZoom;
    maxZoom = src.maxZoom;
    visibility = src.visibility;
}

}
