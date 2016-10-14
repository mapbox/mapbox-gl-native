// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/terrain_layer.hpp>
#include <mbgl/style/layers/terrain_layer_impl.hpp>

namespace mbgl {
namespace style {

TerrainLayer::TerrainLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(Type::Terrain, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

TerrainLayer::TerrainLayer(const Impl& other)
    : Layer(Type::Terrain, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

TerrainLayer::~TerrainLayer() = default;

std::unique_ptr<Layer> TerrainLayer::Impl::clone() const {
    return std::make_unique<TerrainLayer>(*this);
}

std::unique_ptr<Layer> TerrainLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<TerrainLayer>(*this);
    result->impl->id = id_;
    result->impl->ref = this->id;
    result->impl->paint = TerrainPaintProperties();
    return std::move(result);
}

// Source

const std::string& TerrainLayer::getSourceID() const {
    return impl->source;
}

void TerrainLayer::setSourceLayer(const std::string& sourceLayer) {
    impl->sourceLayer = sourceLayer;
}

const std::string& TerrainLayer::getSourceLayer() const {
    return impl->sourceLayer;
}

// Filter

void TerrainLayer::setFilter(const Filter& filter) {
    impl->filter = filter;
    impl->observer->onLayerFilterChanged(*this);
}

const Filter& TerrainLayer::getFilter() const {
    return impl->filter;
}

// Layout properties


// Paint properties

PropertyValue<Color> TerrainLayer::getDefaultTerrainShadowColor() {
    return { { 0, 0, 1, 1 } };
}

PropertyValue<Color> TerrainLayer::getTerrainShadowColor(const optional<std::string>& klass) const {
    return impl->paint.terrainShadowColor.get(klass);
}

void TerrainLayer::setTerrainShadowColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getTerrainShadowColor(klass))
        return;
    impl->paint.terrainShadowColor.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> TerrainLayer::getDefaultTerrainHighlightColor() {
    return { { 1, 1, 0, 1 } };
}

PropertyValue<Color> TerrainLayer::getTerrainHighlightColor(const optional<std::string>& klass) const {
    return impl->paint.terrainHighlightColor.get(klass);
}

void TerrainLayer::setTerrainHighlightColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getTerrainHighlightColor(klass))
        return;
    impl->paint.terrainHighlightColor.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> TerrainLayer::getDefaultTerrainAccentColor() {
    return { Color::black() };
}

PropertyValue<Color> TerrainLayer::getTerrainAccentColor(const optional<std::string>& klass) const {
    return impl->paint.terrainAccentColor.get(klass);
}

void TerrainLayer::setTerrainAccentColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getTerrainAccentColor(klass))
        return;
    impl->paint.terrainAccentColor.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> TerrainLayer::getDefaultTerrainIlluminationDirection() {
    return { 135 };
}

PropertyValue<float> TerrainLayer::getTerrainIlluminationDirection(const optional<std::string>& klass) const {
    return impl->paint.terrainIlluminationDirection.get(klass);
}

void TerrainLayer::setTerrainIlluminationDirection(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTerrainIlluminationDirection(klass))
        return;
    impl->paint.terrainIlluminationDirection.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<AlignmentType> TerrainLayer::getDefaultTerrainIlluminationAlignment() {
    return { AlignmentType::Viewport };
}

PropertyValue<AlignmentType> TerrainLayer::getTerrainIlluminationAlignment(const optional<std::string>& klass) const {
    return impl->paint.terrainIlluminationAlignment.get(klass);
}

void TerrainLayer::setTerrainIlluminationAlignment(PropertyValue<AlignmentType> value, const optional<std::string>& klass) {
    if (value == getTerrainIlluminationAlignment(klass))
        return;
    impl->paint.terrainIlluminationAlignment.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> TerrainLayer::getDefaultTerrainExaggeration() {
    return { 1 };
}

PropertyValue<float> TerrainLayer::getTerrainExaggeration(const optional<std::string>& klass) const {
    return impl->paint.terrainExaggeration.get(klass);
}

void TerrainLayer::setTerrainExaggeration(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTerrainExaggeration(klass))
        return;
    impl->paint.terrainExaggeration.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

} // namespace style
} // namespace mbgl
