#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/layer/symbol_layer.hpp>

#include <boost/function_output_iterator.hpp>

namespace mbgl {

const std::string AnnotationManager::SourceID = "com.mapbox.annotations";
const std::string AnnotationManager::PointLayerID = "com.mapbox.annotations.points";

AnnotationManager::AnnotationManager(float pixelRatio)
    : spriteStore(pixelRatio),
      spriteAtlas(1024, 1024, pixelRatio, spriteStore) {
}

AnnotationManager::~AnnotationManager() = default;

AnnotationIDs
AnnotationManager::addPointAnnotations(const std::vector<PointAnnotation>& points, const uint8_t) {
    AnnotationIDs annotationIDs;
    annotationIDs.reserve(points.size());

    for (const auto& point : points) {
        const uint32_t annotationID = nextID++;
        auto annotation = std::make_shared<PointAnnotationImpl>(annotationID, point);
        pointTree.insert(annotation);
        pointAnnotations.emplace(annotationID, annotation);
        annotationIDs.push_back(annotationID);
    }

    return annotationIDs;
}

AnnotationIDs
AnnotationManager::addShapeAnnotations(const std::vector<ShapeAnnotation>& shapes, const uint8_t maxZoom) {
    AnnotationIDs annotationIDs;
    annotationIDs.reserve(shapes.size());

    for (const auto& shape : shapes) {
        const uint32_t annotationID = nextID++;
        shapeAnnotations.emplace(annotationID,
            std::make_unique<ShapeAnnotationImpl>(annotationID, shape, maxZoom));
        annotationIDs.push_back(annotationID);
    }

    return annotationIDs;
}

void AnnotationManager::updatePointAnnotation(const AnnotationID& id, const PointAnnotation& point, const uint8_t) {
    auto foundAnnotation = pointAnnotations.find(id);
    if (foundAnnotation != pointAnnotations.end()) {
        auto updatedAnnotation = std::make_shared<PointAnnotationImpl>(id, point);
        pointTree.remove(foundAnnotation->second);
        pointTree.insert(updatedAnnotation);
        foundAnnotation->second = updatedAnnotation;
    }
}

void AnnotationManager::removeAnnotations(const AnnotationIDs& ids) {
    for (const auto& id : ids) {
        if (pointAnnotations.find(id) != pointAnnotations.end()) {
            pointTree.remove(pointAnnotations.at(id));
            pointAnnotations.erase(id);
        } else if (shapeAnnotations.find(id) != shapeAnnotations.end()) {
            obsoleteShapeAnnotationLayers.push_back(shapeAnnotations.at(id)->layerID);
            shapeAnnotations.erase(id);
        }
    }
}

AnnotationIDs AnnotationManager::getPointAnnotationsInBounds(const LatLngBounds& bounds) const {
    AnnotationIDs result;

    pointTree.query(boost::geometry::index::intersects(bounds),
        boost::make_function_output_iterator([&](const auto& val){
            result.push_back(val->id);
        }));

    return result;
}

std::unique_ptr<AnnotationTile> AnnotationManager::getTile(const TileID& tileID) {
    if (pointAnnotations.empty() && shapeAnnotations.empty())
        return nullptr;

    auto tile = std::make_unique<AnnotationTile>();

    AnnotationTileLayer& pointLayer = *tile->layers.emplace(
        PointLayerID,
        std::make_unique<AnnotationTileLayer>()).first->second;

    LatLngBounds tileBounds(tileID);

    pointTree.query(boost::geometry::index::intersects(tileBounds),
        boost::make_function_output_iterator([&](const auto& val){
            val->updateLayer(tileID, pointLayer);
        }));

    for (const auto& shape : shapeAnnotations) {
        shape.second->updateTile(tileID, *tile);
    }

    return tile;
}

void AnnotationManager::updateStyle(Style& style) {
    // Create annotation source, point layer, and point bucket
    if (!style.getSource(SourceID)) {
        std::unique_ptr<Source> source = std::make_unique<Source>(SourceType::Annotations, SourceID, "", util::tileSize, std::make_unique<SourceInfo>(), nullptr);
        source->enabled = true;
        style.addSource(std::move(source));

        std::unique_ptr<SymbolLayer> layer = std::make_unique<SymbolLayer>();
        layer->id = PointLayerID;
        layer->source = SourceID;
        layer->sourceLayer = PointLayerID;
        layer->layout.iconImage = std::string("{sprite}");
        layer->layout.iconAllowOverlap = true;
        layer->spriteAtlas = &spriteAtlas;

        style.addLayer(std::move(layer));
    }

    for (const auto& shape : shapeAnnotations) {
        shape.second->updateStyle(style);
    }

    for (const auto& layer : obsoleteShapeAnnotationLayers) {
        if (style.getLayer(layer)) {
            style.removeLayer(layer);
        }
    }

    obsoleteShapeAnnotationLayers.clear();

    for (auto& monitor : monitors) {
        monitor->update(getTile(monitor->tileID));
    }
}

void AnnotationManager::addTileMonitor(AnnotationTileMonitor& monitor) {
    monitors.insert(&monitor);
    monitor.update(getTile(monitor.tileID));
}

void AnnotationManager::removeTileMonitor(AnnotationTileMonitor& monitor) {
    monitors.erase(&monitor);
}

void AnnotationManager::addIcon(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    spriteStore.setSprite(name, sprite);
    spriteAtlas.updateDirty();
}
    
void AnnotationManager::removeIcon(const std::string& name) {
    spriteStore.removeSprite(name);
    spriteAtlas.updateDirty();
}

double AnnotationManager::getTopOffsetPixelsForIcon(const std::string& name) {
    auto sprite = spriteStore.getSprite(name);
    return sprite ? -(sprite->image.height / sprite->pixelRatio) / 2 : 0;
}

} // namespace mbgl
