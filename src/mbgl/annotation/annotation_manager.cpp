#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/layer/symbol_layer.hpp>

#include <boost/function_output_iterator.hpp>

namespace mbgl {

const std::string AnnotationManager::SourceID = "com.mapbox.annotations";
const std::string AnnotationManager::PointLayerID = "com.mapbox.annotations.points";

AnnotationManager::AnnotationManager() = default;
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

LatLngBounds AnnotationManager::getBoundsForAnnotations(const AnnotationIDs& ids) const {
    LatLngBounds result;

    for (const auto& id : ids) {
        if (pointAnnotations.find(id) != pointAnnotations.end()) {
            result.extend(pointAnnotations.at(id)->bounds());
        } else if (shapeAnnotations.find(id) != shapeAnnotations.end()) {
            result.extend(shapeAnnotations.at(id)->bounds());
        }
    }

    return result;
}

std::unique_ptr<AnnotationTile> AnnotationManager::getTile(const TileID& tileID) {
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
        std::unique_ptr<Source> source = std::make_unique<Source>();
        source->info.type = SourceType::Annotations;
        source->info.source_id = SourceID;
        source->enabled = true;
        style.addSource(std::move(source));

        std::unique_ptr<SymbolLayer> layer = std::make_unique<SymbolLayer>();
        layer->id = PointLayerID;
        layer->type = StyleLayerType::Symbol;

        layer->bucket = std::make_shared<StyleBucket>(layer->type);
        layer->bucket->name = layer->id;
        layer->bucket->source = SourceID;
        layer->bucket->source_layer = PointLayerID;
        layer->bucket->layout.set(PropertyKey::IconImage, ConstantFunction<std::string>("{sprite}"));
        layer->bucket->layout.set(PropertyKey::IconAllowOverlap, ConstantFunction<bool>(true));

        style.addLayer(std::move(layer));
    }

    for (const auto& shape : shapeAnnotations) {
        shape.second->updateStyle(style);
    }

    for (const auto& layer : obsoleteShapeAnnotationLayers) {
        style.removeLayer(layer);
    }

    obsoleteShapeAnnotationLayers.clear();
    style.getSource(SourceID)->invalidateTiles();
}

}
