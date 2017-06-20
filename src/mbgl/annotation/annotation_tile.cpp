#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>

#include <utility>

namespace mbgl {

AnnotationTile::AnnotationTile(const OverscaledTileID& overscaledTileID,
                               const TileParameters& parameters)
    : GeometryTile(overscaledTileID, AnnotationManager::SourceID, parameters),
      annotationManager(parameters.annotationManager) {
    annotationManager.addTile(*this);
}

AnnotationTile::~AnnotationTile() {
    annotationManager.removeTile(*this);
}

void AnnotationTile::setNecessity(Necessity) {
}

class AnnotationTileFeatureData {
public:
    AnnotationTileFeatureData(const AnnotationID id_,
                              FeatureType type_,
                              GeometryCollection&& geometries_,
                              std::unordered_map<std::string, std::string>&& properties_)
        : id(id_),
          type(type_),
          geometries(std::move(geometries_)),
          properties(std::move(properties_)) {
    }

    AnnotationID id;
    FeatureType type;
    GeometryCollection geometries;
    std::unordered_map<std::string, std::string> properties;
};

AnnotationTileFeature::AnnotationTileFeature(std::shared_ptr<const AnnotationTileFeatureData> data_)
    : data(std::move(data_)) {
}

AnnotationTileFeature::~AnnotationTileFeature() = default;

FeatureType AnnotationTileFeature::getType() const {
    return data->type;
}

optional<Value> AnnotationTileFeature::getValue(const std::string& key) const {
    auto it = data->properties.find(key);
    if (it != data->properties.end()) {
        return optional<Value>(it->second);
    }
    return optional<Value>();
}

optional<FeatureIdentifier> AnnotationTileFeature::getID() const {
    return { static_cast<uint64_t>(data->id) };
}

GeometryCollection AnnotationTileFeature::getGeometries() const {
    return data->geometries;
}

AnnotationTileLayer::AnnotationTileLayer(std::string name_) : name(std::move(name_)) {
}

void AnnotationTileLayer::addFeature(const AnnotationID id,
                                     FeatureType type,
                                     GeometryCollection geometries,
                                     std::unordered_map<std::string, std::string> properties) {

    features.emplace_back(std::make_shared<AnnotationTileFeatureData>(
        id, type, std::move(geometries), std::move(properties)));
}

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

AnnotationTileLayer& AnnotationTileData::addLayer(const std::string& name) {
    // Only constructs a new layer if it doesn't yet exist, otherwise, we'll use the existing one.
    return layers.emplace(name, name).first->second;
}

} // namespace mbgl
