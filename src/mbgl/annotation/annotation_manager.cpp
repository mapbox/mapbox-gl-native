#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/annotation_source_impl.hpp>
#include <mbgl/annotation/line_annotation_impl.hpp>
#include <mbgl/annotation/fill_annotation_impl.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>

namespace mbgl {

using namespace style;

const std::string AnnotationManager::SourceID = "com.mapbox.annotations";
const std::string AnnotationManager::PointLayerID = "com.mapbox.annotations.points";

AnnotationManager::AnnotationManager()
        : ownedSource(std::make_unique<AnnotationSource>()), source(ownedSource.get()) {
};

AnnotationManager::~AnnotationManager() = default;

AnnotationID AnnotationManager::addAnnotation(const Annotation &annotation, const uint8_t maxZoom) {
    AnnotationID id = nextID++;
    Annotation::visit(annotation, [&](const auto &annotation_) {
        this->add(id, annotation_, maxZoom);
    });
    return id;
}

void AnnotationManager::updateAnnotation(const AnnotationID &id, const Annotation &annotation,
                                         const uint8_t maxZoom) {
    return Annotation::visit(annotation, [&](const auto &annotation_) {
        return this->update(id, annotation_, maxZoom);
    });
}

void AnnotationManager::removeAnnotation(const AnnotationID &id) {
    // Remove from source
    sourceImpl = source->removeAnnotation(id);

    // Remove from shapes
    auto it = shapeAnnotations.find(id);
    if (it != shapeAnnotations.end()) {
        auto removedLayer = it->second->layerID;
        shapeAnnotations.erase(it);
        if (style) {
            style->removeLayer(removedLayer);
        }
    }
}

void AnnotationManager::add(const AnnotationID &id,
                            const SymbolAnnotation &annotation,
                            const uint8_t) {
    sourceImpl = source->addAnnotation(id, annotation);
}

void AnnotationManager::add(const AnnotationID &id, const LineAnnotation &annotation,
                            const uint8_t maxZoom) {
    // Add to source
    sourceImpl = source->addAnnotation(id, annotation.geometry, maxZoom);

    // Add to collection
    ShapeAnnotationImpl &impl = *shapeAnnotations
            .emplace(id,
                     std::make_unique<LineAnnotationImpl>(id, annotation, maxZoom)).first->second;

    if (style) {
        impl.updateStyle(*style);
    }
}

void AnnotationManager::add(const AnnotationID &id, const FillAnnotation &annotation,
                            const uint8_t maxZoom) {
    // Add to source
    sourceImpl = source->addAnnotation(id, annotation.geometry, maxZoom);

    // Add to collection
    ShapeAnnotationImpl &impl = *shapeAnnotations
            .emplace(id,
                     std::make_unique<FillAnnotationImpl>(id, annotation, maxZoom)).first->second;

    if (style) {
        impl.updateStyle(*style);
    }
}

void AnnotationManager::update(const AnnotationID &id,
                               const SymbolAnnotation &annotation,
                               const uint8_t) {
    sourceImpl = source->updateAnnotation(id, annotation);
}

void AnnotationManager::update(const AnnotationID &id,
                               const LineAnnotation &annotation,
                               const uint8_t maxZoom) {
    auto it = shapeAnnotations.find(id);
    if (it == shapeAnnotations.end()) {
        assert(false); // Attempt to update a non-existent shape annotation
    }

    // Update in collection
    shapeAnnotations.erase(it);
    ShapeAnnotationImpl &impl = *shapeAnnotations
            .emplace(id,
                     std::make_unique<LineAnnotationImpl>(id, annotation, maxZoom)).first->second;

    // Update in source
    sourceImpl = source->updateAnnotation(id, annotation.geometry, maxZoom);

    // Update layer
    if (style) {
        impl.updateStyle(*style);
    }
}

void AnnotationManager::update(const AnnotationID &id,
                               const FillAnnotation &annotation,
                               const uint8_t maxZoom) {
    auto it = shapeAnnotations.find(id);
    if (it == shapeAnnotations.end()) {
        assert(false); // Attempt to update a non-existent shape annotation
    }

    // Update in collection
    shapeAnnotations.erase(it);
    ShapeAnnotationImpl &impl = *shapeAnnotations
            .emplace(id,
                     std::make_unique<FillAnnotationImpl>(id, annotation, maxZoom)).first->second;

    // Update in source
    sourceImpl = source->updateAnnotation(id, annotation.geometry, maxZoom);

    // Update layer
    if (style) {
        impl.updateStyle(*style);
    }
}

// To ensure that annotation images do not collide with images from the style,
// we prefix input image IDs with "com.mapbox.annotations".
static std::string prefixedImageID(const std::string &id) {
    return AnnotationManager::SourceID + "." + id;
}

void AnnotationManager::addImage(std::unique_ptr<style::Image> image) {
    const std::string id = prefixedImageID(image->getID());
    images.erase(id);
    auto it = images.emplace(
            id,
            style::Image(id, image->getImage().clone(), image->getPixelRatio(), image->isSdf())
    );

    if (style) {
        style->addImage(std::make_unique<style::Image>(it.first->second));
    }
}

void AnnotationManager::removeImage(const std::string &id_) {
    const std::string id = prefixedImageID(id_);
    images.erase(id);

    if (style) {
        style->removeImage(id);
    }
}

double AnnotationManager::getTopOffsetPixelsForImage(const std::string &id_) {
    const std::string id = prefixedImageID(id_);
    auto it = images.find(id);
    return it != images.end() ?
           -(it->second.getImage().size.height / it->second.getPixelRatio()) / 2 :
           0;
}

void AnnotationManager::onStyleLoaded(style::Style &style_) {
    style = &style_;

    // Add the annotation source
    if (ownedSource) {
        style->addSource(std::move(ownedSource));
    } else {
        style->addSource(sourceImpl ?
                         std::make_unique<AnnotationSource>(*sourceImpl) :
                         std::make_unique<AnnotationSource>());
    }

    source = style->getSource(SourceID)->as<AnnotationSource>();

    // Add the Symbol Layer
    std::unique_ptr<SymbolLayer> layer = std::make_unique<SymbolLayer>(PointLayerID, SourceID);
    layer->setSourceLayer(PointLayerID);
    layer->setIconImage({SourceID + ".{sprite}"});
    layer->setIconAllowOverlap(true);
    layer->setIconIgnorePlacement(true);
    style->addLayer(std::move(layer));

    // Add all images
    for (const auto &image : images) {
        // Call addImage even for images we may have previously added, because we must support
        // addAnnotationImage being used to update an existing image. Creating a new image is
        // relatively cheap, as it copies only the Immutable reference. (We can't keep track
        // of which images need to be added because we don't know if the style is the same
        // instance as in the last updateStyle call. If it's a new style, we need to add all
        // images.)
        style->addImage(std::make_unique<style::Image>(image.second));
    }

    // Add all layers for shape annotations
    for (auto &entry : shapeAnnotations) {
        entry.second->updateStyle(*style);
    }
}

} // namespace mbgl
