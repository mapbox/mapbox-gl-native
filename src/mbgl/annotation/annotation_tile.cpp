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

class AnnotationTileLayerData {
public:
    AnnotationTileLayerData(const std::string& name_) : name(name_) {
    }

    const std::string name;
    std::vector<std::shared_ptr<const AnnotationTileFeatureData>> features;
};

AnnotationTileLayer::AnnotationTileLayer(std::shared_ptr<AnnotationTileLayerData> layer_) : layer(std::move(layer_)) {
}

std::size_t AnnotationTileLayer::featureCount() const {
    return layer->features.size();
}

std::unique_ptr<GeometryTileFeature> AnnotationTileLayer::getFeature(std::size_t i) const {
    return std::make_unique<AnnotationTileFeature>(layer->features.at(i));
}

std::string AnnotationTileLayer::getName() const {
    return layer->name;
}

void AnnotationTileLayer::addFeature(const AnnotationID id,
                                     FeatureType type,
                                     GeometryCollection geometries,
                                     std::unordered_map<std::string, std::string> properties) {

    layer->features.emplace_back(std::make_shared<AnnotationTileFeatureData>(
        id, type, std::move(geometries), std::move(properties)));
}

std::unique_ptr<GeometryTileData> AnnotationTileData::clone() const {
    return std::make_unique<AnnotationTileData>(*this);
}

std::unique_ptr<GeometryTileLayer> AnnotationTileData::getLayer(const std::string& name) const {
    auto it = layers.find(name);
    if (it != layers.end()) {
        return std::make_unique<AnnotationTileLayer>(it->second);
    }
    return nullptr;
}

std::unique_ptr<AnnotationTileLayer> AnnotationTileData::addLayer(const std::string& name) {
    // Only constructs a new layer if it doesn't yet exist, otherwise, we'll use the existing one.
    auto it = layers.find(name);
    if (it == layers.end()) {
        it = layers.emplace(name, std::make_shared<AnnotationTileLayerData>(name)).first;
    }
    return std::make_unique<AnnotationTileLayer>(it->second);
}

} // namespace mbgl
