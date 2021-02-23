#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/annotation/fill_annotation_impl.hpp>
#include <mbgl/annotation/line_annotation_impl.hpp>
#include <mbgl/annotation/symbol_annotation_impl.hpp>
#include <mbgl/layermanager/layer_manager.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/expression/image.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_impl.hpp>

#include <boost/function_output_iterator.hpp>

// Note: LayerManager::annotationsEnabled is defined
// at compile time, so that linker (with LTO on) is able
// to optimize out the unreachable code.
#define CHECK_ANNOTATIONS_ENABLED_AND_RETURN(result) \
if (!LayerManager::annotationsEnabled) {             \
    assert(false);                                   \
    return result;                                   \
}

namespace mbgl {

using namespace style;

const std::string AnnotationManager::SourceID = "com.mapbox.annotations";
const std::string AnnotationManager::PointLayerID = "com.mapbox.annotations.points";
const std::string AnnotationManager::ShapeLayerID = "com.mapbox.annotations.shape.";

AnnotationManager::AnnotationManager(Style& style_)
        : style(style_) {
};

AnnotationManager::~AnnotationManager() = default;

void AnnotationManager::setStyle(Style& style_) {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN();
    style = style_;
}

void AnnotationManager::onStyleLoaded() {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN();
    updateStyle();
}

AnnotationID AnnotationManager::addAnnotation(const Annotation& annotation) {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN(nextID++);
    std::lock_guard<std::mutex> lock(mutex);
    AnnotationID id = nextID++;
    Annotation::visit(annotation, [&] (const auto& annotation_) {
        this->add(id, annotation_);
    });
    dirty = true;
    return id;
}

bool AnnotationManager::updateAnnotation(const AnnotationID& id, const Annotation& annotation) {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN(true);
    std::lock_guard<std::mutex> lock(mutex);
    Annotation::visit(annotation, [&] (const auto& annotation_) {
        this->update(id, annotation_);
    });
    return dirty;
}

void AnnotationManager::removeAnnotation(const AnnotationID& id) {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN();
    std::lock_guard<std::mutex> lock(mutex);
    remove(id);
    dirty = true;
}

void AnnotationManager::add(const AnnotationID& id, const SymbolAnnotation& annotation) {
    auto impl = std::make_shared<SymbolAnnotationImpl>(id, annotation);
    symbolTree.insert(impl);
    symbolAnnotations.emplace(id, impl);
}

void AnnotationManager::add(const AnnotationID& id, const LineAnnotation& annotation) {
    ShapeAnnotationImpl& impl = *shapeAnnotations.emplace(id,
        std::make_unique<LineAnnotationImpl>(id, annotation)).first->second;
    impl.updateStyle(*style.get().impl);
}

void AnnotationManager::add(const AnnotationID& id, const FillAnnotation& annotation) {
    ShapeAnnotationImpl& impl = *shapeAnnotations.emplace(id,
        std::make_unique<FillAnnotationImpl>(id, annotation)).first->second;
    impl.updateStyle(*style.get().impl);
}

void AnnotationManager::update(const AnnotationID& id, const SymbolAnnotation& annotation) {
    auto it = symbolAnnotations.find(id);
    if (it == symbolAnnotations.end()) {
        assert(false); // Attempt to update a non-existent symbol annotation
        return;
    }

    const SymbolAnnotation& existing = it->second->annotation;

    if (existing.geometry != annotation.geometry || existing.icon != annotation.icon) {
        dirty = true;

        remove(id);
        add(id, annotation);
    }
}

void AnnotationManager::update(const AnnotationID& id, const LineAnnotation& annotation) {
    auto it = shapeAnnotations.find(id);
    if (it == shapeAnnotations.end()) {
        assert(false); // Attempt to update a non-existent shape annotation
        return;
    }

    shapeAnnotations.erase(it);
    add(id, annotation);
    dirty = true;
}

void AnnotationManager::update(const AnnotationID& id, const FillAnnotation& annotation) {
    auto it = shapeAnnotations.find(id);
    if (it == shapeAnnotations.end()) {
        assert(false); // Attempt to update a non-existent shape annotation
        return;
    }

    shapeAnnotations.erase(it);
    add(id, annotation);
    dirty = true;
}

void AnnotationManager::remove(const AnnotationID& id) {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN();
    if (symbolAnnotations.find(id) != symbolAnnotations.end()) {
        symbolTree.remove(symbolAnnotations.at(id));
        symbolAnnotations.erase(id);
    } else if (shapeAnnotations.find(id) != shapeAnnotations.end()) {
        auto it = shapeAnnotations.find(id);
        *style.get().impl->removeLayer(it->second->layerID);
        shapeAnnotations.erase(it);
    } else {
        assert(false); // Should never happen
    }
}

std::unique_ptr<AnnotationTileData> AnnotationManager::getTileData(const CanonicalTileID& tileID) {
    if (symbolAnnotations.empty() && shapeAnnotations.empty())
        return nullptr;

    auto tileData = std::make_unique<AnnotationTileData>();

    auto pointLayer = tileData->addLayer(PointLayerID);

    LatLngBounds tileBounds(tileID);
    // Hack for https://github.com/mapbox/mapbox-gl-native/issues/12472
    // To handle precision issues, query a slightly larger area than the tile bounds
    // Symbols at a border can be included in vector data for both tiles
    // The rendering/querying logic will make sure the symbols show up in only one of the tiles
    tileBounds.extend(LatLng(tileBounds.south() - 0.000000001, tileBounds.west() - 0.000000001));
    tileBounds.extend(LatLng(tileBounds.north() + 0.000000001, tileBounds.east() + 0.000000001));
    
    symbolTree.query(boost::geometry::index::intersects(tileBounds),
        boost::make_function_output_iterator([&](const auto& val){
            val->updateLayer(tileID, *pointLayer);
        }));

    for (const auto& shape : shapeAnnotations) {
        shape.second->updateTileData(tileID, *tileData);
    }

    return tileData;
}

void AnnotationManager::updateStyle() {
    // Create annotation source, point layer, and point bucket. We do everything via Style::Impl
    // because we don't want annotation mutations to trigger Style::Impl::styleMutated to be set.
    if (!style.get().impl->getSource(SourceID)) {
        style.get().impl->addSource(std::make_unique<AnnotationSource>());

        std::unique_ptr<SymbolLayer> layer = std::make_unique<SymbolLayer>(PointLayerID, SourceID);

        using namespace expression::dsl;
        layer->setSourceLayer(PointLayerID);
        layer->setIconImage(PropertyExpression<expression::Image>(
            image(concat(vec(literal(SourceID + "."), toString(get("sprite")))))));
        layer->setIconAllowOverlap(true);
        layer->setIconIgnorePlacement(true);

        style.get().impl->addLayer(std::move(layer));
    }

    std::lock_guard<std::mutex> lock(mutex);

    for (const auto& shape : shapeAnnotations) {
        shape.second->updateStyle(*style.get().impl);
    }

    for (const auto& image : images) {
        // Call addImage even for images we may have previously added, because we must support
        // addAnnotationImage being used to update an existing image. Creating a new image is
        // relatively cheap, as it copies only the Immutable reference. (We can't keep track
        // of which images need to be added because we don't know if the style is the same
        // instance as in the last updateStyle call. If it's a new style, we need to add all
        // images.)
        style.get().impl->addImage(std::make_unique<style::Image>(image.second));
    }
}

void AnnotationManager::updateData() {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN();
    std::lock_guard<std::mutex> lock(mutex);
    if (dirty) {
        for (auto& tile : tiles) {
            tile->setData(getTileData(tile->id.canonical));
        }
        dirty = false;
    }
}

void AnnotationManager::addTile(AnnotationTile& tile) {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN();
    std::lock_guard<std::mutex> lock(mutex);
    tiles.insert(&tile);
    tile.setData(getTileData(tile.id.canonical));
}

void AnnotationManager::removeTile(AnnotationTile& tile) {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN();
    std::lock_guard<std::mutex> lock(mutex);
    tiles.erase(&tile);
}

// To ensure that annotation images do not collide with images from the style,
// we prefix input image IDs with "com.mapbox.annotations".
static std::string prefixedImageID(const std::string& id) {
    return AnnotationManager::SourceID + "." + id;
}

void AnnotationManager::addImage(std::unique_ptr<style::Image> image) {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN();
    std::lock_guard<std::mutex> lock(mutex);
    const std::string id = prefixedImageID(image->getID());
    images.erase(id);
    auto inserted = images.emplace(id, style::Image(id, image->getImage().clone(),
                                                    image->getPixelRatio(), image->isSdf()));
    style.get().impl->addImage(std::make_unique<style::Image>(inserted.first->second));
}

void AnnotationManager::removeImage(const std::string& id_) {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN();
    std::lock_guard<std::mutex> lock(mutex);
    const std::string id = prefixedImageID(id_);
    images.erase(id);
    style.get().impl->removeImage(id);
}

double AnnotationManager::getTopOffsetPixelsForImage(const std::string& id_) {
    CHECK_ANNOTATIONS_ENABLED_AND_RETURN(0.0);
    std::lock_guard<std::mutex> lock(mutex);
    const std::string id = prefixedImageID(id_);
    auto it = images.find(id);
    return it != images.end() ? -(it->second.getImage().size.height / it->second.getPixelRatio()) / 2 : 0.0;
}

} // namespace mbgl
