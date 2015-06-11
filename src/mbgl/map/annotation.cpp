#include <mbgl/map/annotation.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/live_tile.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/annotation/point_annotation_layer.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/ptr.hpp>

#include <algorithm>
#include <set>
#include <memory>

namespace mbgl {

Annotation::Annotation(AnnotationID id_, AnnotationType type_, const std::string& symbol_, const LatLng& point_)
    : id(id_),
      type(type_),
      symbol(symbol_),
      point(point_)
       {
}

AnnotationManager::AnnotationManager() : annotations(std::make_unique<PointAnnotationLayer>()) {}

AnnotationManager::~AnnotationManager() {
    // leave this here because the header file doesn't have a definition of
    // Annotation so we can't destruct the object with just the header file.
}

void AnnotationManager::setDefaultPointAnnotationSymbol(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(mtx);
    defaultPointAnnotationSymbol = symbol;
}

uint32_t AnnotationManager::nextID() {
    return nextID_++;
}

vec2<double> AnnotationManager::projectPoint(const LatLng& point) {
    // Project a coordinate into unit space in a square map.
    const double sine = std::sin(point.latitude * M_PI / 180.0);
    const double x = point.longitude / 360.0 + 0.5;
    const double y = 0.5 - 0.25 * std::log((1.0 + sine) / (1.0 - sine)) / M_PI;
    return { x, y };
}

std::pair<std::vector<TileID>, AnnotationIDs>
AnnotationManager::addPointAnnotations(const std::vector<LatLng>& points,
                                       const std::vector<std::string>& symbols,
                                       const MapData& data) {
    std::lock_guard<std::mutex> lock(mtx);

    std::vector<std::shared_ptr<const Annotation>> annotationObjs;
    annotationObjs.reserve(points.size());
    AnnotationIDs annotationIDs;
    annotationIDs.reserve(points.size());

    std::set<TileID> affectedTiles;

    const int8_t maxZoom = data.transform.getMaxZoom();

    for (size_t i = 0; i < points.size(); ++i) {
        const auto annotationID = nextID();

        const std::string& symbol =
            symbols.size() > i && symbols[i].length() ? symbols[i] : defaultPointAnnotationSymbol;

        auto annotation = std::make_shared<const Annotation>(annotationID, AnnotationType::Point, symbol, points[i]);
        // track the annotation global ID and its geometry
        annotationIndex.emplace(annotationID, annotation);
        annotationObjs.push_back(annotation);
        annotationIDs.push_back(annotationID);

        const TileID id { maxZoom, points[i] };
        int32_t x = id.x, y = id.y;
        for (int8_t z = id.z; z >= 0; z--, x /= 2, y /= 2) {
            affectedTiles.emplace(z, x, y, z);
        }
    }

    annotations->add(annotationObjs);

    // TileIDs that need refreshed and the annotation identifiers held onto by the client.
    return std::make_pair(std::vector<TileID>{ affectedTiles.begin(), affectedTiles.end() },
                          annotationIDs);
}

std::vector<TileID> AnnotationManager::removeAnnotations(const AnnotationIDs& ids,
                                                         const MapData& data) {
    std::lock_guard<std::mutex> lock(mtx);

    std::set<TileID> affectedTiles;

    const uint8_t zoomCount = data.transform.getMaxZoom() + 1;

    // iterate annotation id's passed
    for (const auto& annotationID : ids) {
        // grab annotation object
        const auto& annotation_it = annotationIndex.find(annotationID);
        if (annotation_it != annotationIndex.end()) {
            const auto& annotation = annotation_it->second;
            annotations->remove(annotation);
            // calculate annotation's affected tile for each zoom
            for (int8_t z = 0; z < zoomCount; ++z) {
                affectedTiles.emplace(z, annotation->point);
            }
            annotationIndex.erase(annotationID);
        }
    }

    // TileIDs for tiles that need refreshed.
    return { affectedTiles.begin(), affectedTiles.end() };
}

std::vector<uint32_t> AnnotationManager::getAnnotationsInBounds(const LatLngBounds& queryBounds,
                                                                const MapData&) const {
    std::lock_guard<std::mutex> lock(mtx);

    std::vector<uint32_t> matchingAnnotations;
    const auto result = annotations->getInBounds(queryBounds);
    for (const auto& annotation : result) {
        matchingAnnotations.emplace_back(annotation->id);
    }

    return matchingAnnotations;
}

std::vector<std::shared_ptr<const Annotation>>
AnnotationManager::getAnnotations(const std::string&, const TileID& id) const {
    std::lock_guard<std::mutex> lock(mtx);
    return annotations->getInBounds(id);
}

LatLngBounds AnnotationManager::getBoundsForAnnotations(const AnnotationIDs& ids) const {
    std::lock_guard<std::mutex> lock(mtx);

    LatLngBounds bounds;
    for (auto id : ids) {
        const auto annotation_it = annotationIndex.find(id);
        if (annotation_it != annotationIndex.end()) {
            bounds.extend(annotation_it->second->point);
        }
    }

    return bounds;
}

std::shared_ptr<const LiveTile> AnnotationManager::getTile(const TileID& id) {
    return std::make_shared<const LiveTile>(*this, id);
}

const std::string AnnotationManager::layerID = "com.mapbox.annotations.points";

}
