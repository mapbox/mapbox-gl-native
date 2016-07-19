#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/annotation/symbol_annotation_impl.hpp>
#include <mbgl/annotation/line_annotation_impl.hpp>
#include <mbgl/annotation/fill_annotation_impl.hpp>
#include <mbgl/annotation/style_sourced_annotation_impl.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>

#include <boost/function_output_iterator.hpp>

namespace mbgl {

using namespace style;

const std::string AnnotationManager::SourceID = "com.mapbox.annotations";
const std::string AnnotationManager::PointLayerID = "com.mapbox.annotations.points";

AnnotationManager::AnnotationManager(float pixelRatio)
    : spriteStore(pixelRatio),
      spriteAtlas(1024, 1024, pixelRatio, spriteStore) {
}

AnnotationManager::~AnnotationManager() = default;

AnnotationID AnnotationManager::addAnnotation(const Annotation& annotation, const uint8_t maxZoom) {
    AnnotationID id = nextID++;
    updateAnnotation(id, annotation, maxZoom);
    return id;
}

void AnnotationManager::updateAnnotation(const AnnotationID& id, const Annotation& annotation, const uint8_t maxZoom) {
    removeAnnotation(id);
    Annotation::visit(annotation, [&] (const auto& annotation_) {
        this->add(id, annotation_, maxZoom);
    });
}

void AnnotationManager::removeAnnotation(const AnnotationID& id) {
    if (symbolAnnotations.find(id) != symbolAnnotations.end()) {
        symbolTree.remove(symbolAnnotations.at(id));
        symbolAnnotations.erase(id);
    } else if (shapeAnnotations.find(id) != shapeAnnotations.end()) {
        obsoleteShapeAnnotationLayers.push_back(shapeAnnotations.at(id)->layerID);
        shapeAnnotations.erase(id);
    }
}

void AnnotationManager::add(const AnnotationID& id, const SymbolAnnotation& annotation, const uint8_t) {
    auto impl = std::make_shared<SymbolAnnotationImpl>(id, annotation);
    symbolTree.insert(impl);
    symbolAnnotations.emplace(id, impl);
}

void AnnotationManager::add(const AnnotationID& id, const LineAnnotation& annotation, const uint8_t maxZoom) {
    shapeAnnotations.emplace(id,
        std::make_unique<LineAnnotationImpl>(id, annotation, maxZoom));
}

void AnnotationManager::add(const AnnotationID& id, const FillAnnotation& annotation, const uint8_t maxZoom) {
    shapeAnnotations.emplace(id,
        std::make_unique<FillAnnotationImpl>(id, annotation, maxZoom));
}

void AnnotationManager::add(const AnnotationID& id, const StyleSourcedAnnotation& annotation, const uint8_t maxZoom) {
    shapeAnnotations.emplace(id,
        std::make_unique<StyleSourcedAnnotationImpl>(id, annotation, maxZoom));
}

AnnotationIDs AnnotationManager::getPointAnnotationsInBounds(const LatLngBounds& bounds) const {
    AnnotationIDs result;

    symbolTree.query(boost::geometry::index::intersects(bounds),
        boost::make_function_output_iterator([&](const auto& val){
            result.push_back(val->id);
        }));

    return result;
}

std::unique_ptr<AnnotationTile> AnnotationManager::getTile(const CanonicalTileID& tileID) {
    if (symbolAnnotations.empty() && shapeAnnotations.empty())
        return nullptr;

    auto tile = std::make_unique<AnnotationTile>();

    AnnotationTileLayer& pointLayer = *tile->layers.emplace(
        PointLayerID,
        std::make_unique<AnnotationTileLayer>(PointLayerID)).first->second;

    LatLngBounds tileBounds(tileID);

    symbolTree.query(boost::geometry::index::intersects(tileBounds),
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
        auto tileset = std::make_unique<Tileset>();
        tileset->maxZoom = 18;
        std::unique_ptr<Source> source = std::make_unique<Source>(SourceType::Annotations, SourceID, "", util::tileSize, std::move(tileset), nullptr);
        source->enabled = true;
        style.addSource(std::move(source));

        std::unique_ptr<SymbolLayer> layer = std::make_unique<SymbolLayer>(PointLayerID);

        layer->setSource(SourceID, PointLayerID);
        layer->setIconImage({"{sprite}"});
        layer->setIconAllowOverlap(true);

        layer->impl->spriteAtlas = &spriteAtlas;

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
        monitor->update(getTile(monitor->tileID.canonical));
    }
}

void AnnotationManager::addTileMonitor(AnnotationTileMonitor& monitor) {
    monitors.insert(&monitor);
    monitor.update(getTile(monitor.tileID.canonical));
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
