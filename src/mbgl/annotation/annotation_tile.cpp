#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/map/map_data.hpp>

namespace mbgl {

AnnotationTileFeature::AnnotationTileFeature(FeatureType type_, GeometryCollection geometries_,
                                 std::unordered_map<std::string, std::string> properties_)
    : type(type_),
      properties(properties_),
      geometries(geometries_) {}

mapbox::util::optional<Value> AnnotationTileFeature::getValue(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return mapbox::util::optional<Value>(it->second);
    }
    return mapbox::util::optional<Value>();
}

util::ptr<GeometryTileLayer> AnnotationTile::getLayer(const std::string& name) const {
    auto it = layers.find(name);
    if (it != layers.end()) {
        return it->second;
    }
    return nullptr;
}

AnnotationTileMonitor::AnnotationTileMonitor(const TileID& tileID_, MapData& data_)
    : tileID(tileID_),
      data(data_) {
}

AnnotationTileMonitor::~AnnotationTileMonitor() {
    data.getAnnotationManager()->removeTileMonitor(*this);
}

Request* AnnotationTileMonitor::monitorTile(std::function<void (std::exception_ptr, std::unique_ptr<GeometryTile>)> callback_) {
    callback = callback_;
    data.getAnnotationManager()->addTileMonitor(*this);
    return nullptr;
}

void AnnotationTileMonitor::update(std::unique_ptr<GeometryTile> tile) {
    callback(nullptr, std::move(tile));
}

}
