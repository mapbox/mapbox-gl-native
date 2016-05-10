#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/storage/file_source.hpp>

#include <utility>

namespace mbgl {

AnnotationTileFeature::AnnotationTileFeature(FeatureType type_, GeometryCollection geometries_,
                                 std::unordered_map<std::string, std::string> properties_)
    : type(type_),
      properties(std::move(properties_)),
      geometries(std::move(geometries_)) {}

optional<Value> AnnotationTileFeature::getValue(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return optional<Value>(it->second);
    }
    return optional<Value>();
}

util::ptr<GeometryTileLayer> AnnotationTile::getLayer(const std::string& name) const {
    auto it = layers.find(name);
    if (it != layers.end()) {
        return it->second;
    }
    return nullptr;
}

AnnotationTileMonitor::AnnotationTileMonitor(const OverscaledTileID& tileID_, AnnotationManager& annotationManager_)
    : tileID(tileID_),
      annotationManager(annotationManager_) {
}

AnnotationTileMonitor::~AnnotationTileMonitor() {
    annotationManager.removeTileMonitor(*this);
}

std::unique_ptr<AsyncRequest> AnnotationTileMonitor::monitorTile(const GeometryTileMonitor::Callback& callback_) {
    callback = callback_;
    annotationManager.addTileMonitor(*this);
    return nullptr;
}

void AnnotationTileMonitor::update(std::unique_ptr<GeometryTile> tile) {
    callback(nullptr, std::move(tile), {}, {});
}

} // namespace mbgl
