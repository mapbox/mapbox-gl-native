#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/style/style.hpp>

#include <utility>

namespace mbgl {

AnnotationTile::AnnotationTile(const OverscaledTileID& overscaledTileID,
                               const TileParameters& parameters)
    : GeometryTile(overscaledTileID, AnnotationManager::SourceID, parameters,
                   *parameters.style.glyphAtlas,
                   parameters.annotationManager.spriteAtlas),
      annotationManager(parameters.annotationManager) {
    annotationManager.addTile(*this);
}

AnnotationTile::~AnnotationTile() {
    annotationManager.removeTile(*this);
}

void AnnotationTile::setNecessity(Necessity) {}

AnnotationTileFeature::AnnotationTileFeature(const AnnotationID id_,
                                             FeatureType type_, GeometryCollection geometries_,
                                             std::unordered_map<std::string, std::string> properties_)
    : id(id_),
      type(type_),
      properties(std::move(properties_)),
      geometries(std::move(geometries_)) {}

optional<Value> AnnotationTileFeature::getValue(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return optional<Value>(it->second);
    }
    return optional<Value>();
}

AnnotationTileLayer::AnnotationTileLayer(std::string name_)
    : name(std::move(name_)) {}

std::unique_ptr<GeometryTileData> AnnotationTileData::clone() const {
    return std::make_unique<AnnotationTileData>(*this);
}

const GeometryTileLayer* AnnotationTileData::getLayer(const std::string& name) const {
    auto it = layers.find(name);
    if (it != layers.end()) {
        return &it->second;
    }
    return nullptr;
}

} // namespace mbgl
