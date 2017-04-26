#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/annotation/symbol_annotation_impl.hpp>
#include <mbgl/annotation/line_annotation_impl.hpp>
#include <mbgl/annotation/fill_annotation_impl.hpp>
#include <mbgl/annotation/style_sourced_annotation_impl.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/storage/file_source.hpp>

#include <boost/function_output_iterator.hpp>

namespace mbgl {

using namespace style;

const std::string AnnotationManager::SourceID = "com.mapbox.annotations";
const std::string AnnotationManager::PointLayerID = "com.mapbox.annotations.points";

AnnotationManager::AnnotationManager(float pixelRatio)
    : spriteAtlas({ 1024, 1024 }, pixelRatio) {
    // This is a special atlas, holding only images added via addIcon, so we always treat it as
    // loaded.
    spriteAtlas.markAsLoaded();
}

AnnotationManager::~AnnotationManager() = default;

AnnotationID AnnotationManager::addAnnotation(const Annotation& annotation, const uint8_t maxZoom) {
    AnnotationID id = nextID++;
    Annotation::visit(annotation, [&] (const auto& annotation_) {
        this->add(id, annotation_, maxZoom);
    });
    return id;
}

Update AnnotationManager::updateAnnotation(const AnnotationID& id, const Annotation& annotation, const uint8_t maxZoom) {
    return Annotation::visit(annotation, [&] (const auto& annotation_) {
        return this->update(id, annotation_, maxZoom);
    });
}

void AnnotationManager::removeAnnotation(const AnnotationID& id) {
    if (symbolAnnotations.find(id) != symbolAnnotations.end()) {
        symbolTree.remove(symbolAnnotations.at(id));
        symbolAnnotations.erase(id);
    } else if (shapeAnnotations.find(id) != shapeAnnotations.end()) {
        obsoleteShapeAnnotationLayers.insert(shapeAnnotations.at(id)->layerID);
        shapeAnnotations.erase(id);
    } else {
        assert(false); // Should never happen
    }
}

void AnnotationManager::add(const AnnotationID& id, const SymbolAnnotation& annotation, const uint8_t) {
    auto impl = std::make_shared<SymbolAnnotationImpl>(id, annotation);
    symbolTree.insert(impl);
    symbolAnnotations.emplace(id, impl);
}

void AnnotationManager::add(const AnnotationID& id, const LineAnnotation& annotation, const uint8_t maxZoom) {
    ShapeAnnotationImpl& impl = *shapeAnnotations.emplace(id,
        std::make_unique<LineAnnotationImpl>(id, annotation, maxZoom)).first->second;
    obsoleteShapeAnnotationLayers.erase(impl.layerID);
}

void AnnotationManager::add(const AnnotationID& id, const FillAnnotation& annotation, const uint8_t maxZoom) {
    ShapeAnnotationImpl& impl = *shapeAnnotations.emplace(id,
        std::make_unique<FillAnnotationImpl>(id, annotation, maxZoom)).first->second;
    obsoleteShapeAnnotationLayers.erase(impl.layerID);
}

void AnnotationManager::add(const AnnotationID& id, const StyleSourcedAnnotation& annotation, const uint8_t maxZoom) {
    ShapeAnnotationImpl& impl = *shapeAnnotations.emplace(id,
        std::make_unique<StyleSourcedAnnotationImpl>(id, annotation, maxZoom)).first->second;
    obsoleteShapeAnnotationLayers.erase(impl.layerID);
}

Update AnnotationManager::update(const AnnotationID& id, const SymbolAnnotation& annotation, const uint8_t maxZoom) {
    Update result = Update::Nothing;

    auto it = symbolAnnotations.find(id);
    if (it == symbolAnnotations.end()) {
        assert(false); // Attempt to update a non-existent symbol annotation
        return result;
    }

    const SymbolAnnotation& existing = it->second->annotation;

    if (existing.geometry != annotation.geometry) {
        result |= Update::AnnotationData;
    }

    if (existing.icon != annotation.icon) {
        result |= Update::AnnotationData | Update::AnnotationStyle;
    }

    if (result != Update::Nothing) {
        removeAndAdd(id, annotation, maxZoom);
    }

    return result;
}

Update AnnotationManager::update(const AnnotationID& id, const LineAnnotation& annotation, const uint8_t maxZoom) {
    auto it = shapeAnnotations.find(id);
    if (it == shapeAnnotations.end()) {
        assert(false); // Attempt to update a non-existent shape annotation
        return Update::Nothing;
    }
    removeAndAdd(id, annotation, maxZoom);
    return Update::AnnotationData | Update::AnnotationStyle;
}

Update AnnotationManager::update(const AnnotationID& id, const FillAnnotation& annotation, const uint8_t maxZoom) {
    auto it = shapeAnnotations.find(id);
    if (it == shapeAnnotations.end()) {
        assert(false); // Attempt to update a non-existent shape annotation
        return Update::Nothing;
    }
    removeAndAdd(id, annotation, maxZoom);
    return Update::AnnotationData | Update::AnnotationStyle;
}

Update AnnotationManager::update(const AnnotationID& id, const StyleSourcedAnnotation& annotation, const uint8_t maxZoom) {
    auto it = shapeAnnotations.find(id);
    if (it == shapeAnnotations.end()) {
        assert(false); // Attempt to update a non-existent shape annotation
        return Update::Nothing;
    }
    removeAndAdd(id, annotation, maxZoom);
    return Update::AnnotationData | Update::AnnotationStyle;
}

void AnnotationManager::removeAndAdd(const AnnotationID& id, const Annotation& annotation, const uint8_t maxZoom) {
    removeAnnotation(id);
    Annotation::visit(annotation, [&] (const auto& annotation_) {
        this->add(id, annotation_, maxZoom);
    });
}

std::unique_ptr<AnnotationTileData> AnnotationManager::getTileData(const CanonicalTileID& tileID) {
    if (symbolAnnotations.empty() && shapeAnnotations.empty())
        return nullptr;

    auto tileData = std::make_unique<AnnotationTileData>();

    AnnotationTileLayer& pointLayer = tileData->layers.emplace(PointLayerID, PointLayerID).first->second;

    LatLngBounds tileBounds(tileID);

    symbolTree.query(boost::geometry::index::intersects(tileBounds),
        boost::make_function_output_iterator([&](const auto& val){
            val->updateLayer(tileID, pointLayer);
        }));

    for (const auto& shape : shapeAnnotations) {
        shape.second->updateTileData(tileID, *tileData);
    }

    return tileData;
}

void AnnotationManager::updateStyle(Style& style) {
    // Create annotation source, point layer, and point bucket
    if (!style.getSource(SourceID)) {
        style.addSource(std::make_unique<AnnotationSource>());

        std::unique_ptr<SymbolLayer> layer = std::make_unique<SymbolLayer>(PointLayerID, SourceID);

        layer->setSourceLayer(PointLayerID);
        layer->setIconImage({"{sprite}"});
        layer->setIconAllowOverlap(true);
        layer->setIconIgnorePlacement(true);

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
}

void AnnotationManager::updateData() {
    for (auto& tile : tiles) {
        tile->setData(getTileData(tile->id.canonical));
    }
}

void AnnotationManager::addTile(AnnotationTile& tile) {
    tiles.insert(&tile);
    tile.setData(getTileData(tile.id.canonical));
}

void AnnotationManager::removeTile(AnnotationTile& tile) {
    tiles.erase(&tile);
}

void AnnotationManager::addImage(const std::string& id, std::unique_ptr<style::Image> image) {
    spriteAtlas.addImage(id, std::move(image));
}

void AnnotationManager::removeImage(const std::string& id) {
    spriteAtlas.removeImage(id);
}

double AnnotationManager::getTopOffsetPixelsForImage(const std::string& id) {
    const style::Image* image = spriteAtlas.getImage(id);
    return image ? -(image->image.size.height / image->pixelRatio) / 2 : 0;
}

} // namespace mbgl
