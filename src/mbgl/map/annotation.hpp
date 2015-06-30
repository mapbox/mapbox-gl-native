#ifndef MBGL_MAP_ANNOTATIONS
#define MBGL_MAP_ANNOTATIONS

#include <mbgl/map/map.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/style/style_properties.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geojsonvt/geojsonvt.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/vec.hpp>

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace mbgl {

class Annotation;
class PointAnnotation;
class ShapeAnnotation;
class LiveTile;
class LiveTileFeature;

using GeoJSONVT = mapbox::util::geojsonvt::GeoJSONVT;

class Annotation : private util::noncopyable {
    friend class AnnotationManager;
public:
    Annotation(AnnotationType, const AnnotationSegments&, const StyleProperties&);

public:
    const StyleProperties styleProperties;

private:
    LatLng getPoint() const;
    LatLngBounds getBounds() const { return bounds; }

private:
    const AnnotationType type = AnnotationType::Point;
    const AnnotationSegments geometry;
    std::unordered_map<TileID, std::weak_ptr<const LiveTileFeature>, TileID::Hash> tilePointFeatures;
    const LatLngBounds bounds;
};

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager();
    ~AnnotationManager();

    void markStaleTiles(std::unordered_set<TileID, TileID::Hash>);
    std::unordered_set<TileID, TileID::Hash> resetStaleTiles();

    void setDefaultPointAnnotationSymbol(const std::string& symbol);

    std::pair<std::unordered_set<TileID, TileID::Hash>, AnnotationIDs>
    addPointAnnotations(const std::vector<PointAnnotation>&,
                        const uint8_t maxZoom);

    std::pair<std::unordered_set<TileID, TileID::Hash>, AnnotationIDs>
    addShapeAnnotations(const std::vector<ShapeAnnotation>&,
                        const uint8_t maxZoom);

    std::unordered_set<TileID, TileID::Hash> removeAnnotations(const AnnotationIDs&, const uint8_t maxZoom);
    AnnotationIDs getOrderedShapeAnnotations() const { return orderedShapeAnnotations; }
    const StyleProperties getAnnotationStyleProperties(uint32_t) const;

    AnnotationIDs getAnnotationsInBounds(const LatLngBounds&, const uint8_t maxZoom, const AnnotationType& = AnnotationType::Any) const;
    LatLngBounds getBoundsForAnnotations(const AnnotationIDs&) const;

    const LiveTile* getTile(const TileID& id);

    static const std::string PointLayerID;
    static const std::string ShapeLayerID;

private:
    inline uint32_t nextID();
    static vec2<double> projectPoint(const LatLng& point);
    std::unordered_set<TileID, TileID::Hash> addTileFeature(
        const uint32_t annotationID,
        const AnnotationSegments&,
        const std::vector<std::vector<vec2<double>>>& projectedFeature,
        const AnnotationType&,
        const StyleProperties&,
        const std::unordered_map<std::string, std::string>& featureProperties,
        const uint8_t maxZoom);

private:
    std::string defaultPointAnnotationSymbol;
    std::unordered_map<uint32_t, std::unique_ptr<Annotation>> annotations;
    std::vector<uint32_t> orderedShapeAnnotations;
    std::unordered_map<TileID, std::pair<std::unordered_set<uint32_t>, std::unique_ptr<LiveTile>>, TileID::Hash> tiles;
    std::unordered_map<uint32_t, std::unique_ptr<GeoJSONVT>> shapeTilers;
    std::unordered_set<TileID, TileID::Hash> staleTiles;
    uint32_t nextID_ = 0;
};

}

#endif
