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
class Style;

using GeoJSONVT = mapbox::util::geojsonvt::GeoJSONVT;

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager();
    ~AnnotationManager();

    void setDefaultPointAnnotationSymbol(const std::string& symbol);

    AnnotationIDs addPointAnnotations(const std::vector<PointAnnotation>&, const uint8_t maxZoom);
    AnnotationIDs addShapeAnnotations(const std::vector<ShapeAnnotation>&, const uint8_t maxZoom);
    void removeAnnotations(const AnnotationIDs&, const uint8_t maxZoom);

    AnnotationIDs getAnnotationsInBounds(const LatLngBounds&, const uint8_t maxZoom, const AnnotationType& = AnnotationType::Any) const;
    LatLngBounds getBoundsForAnnotations(const AnnotationIDs&) const;

    void updateStyle(Style&);
    const LiveTile* getTile(const TileID& id);

    static const std::string PointSourceID;
    static const std::string ShapeSourceID;

private:
    inline uint32_t nextID();
    static vec2<double> projectPoint(const LatLng& point);

    uint32_t addShapeAnnotation(const ShapeAnnotation&, const uint8_t maxZoom);
    uint32_t addPointAnnotation(const PointAnnotation&, const uint8_t maxZoom);

    std::string defaultPointAnnotationSymbol;
    std::unordered_map<uint32_t, std::unique_ptr<Annotation>> annotations;
    std::vector<uint32_t> orderedShapeAnnotations;
    std::unordered_map<TileID, std::pair<std::unordered_set<uint32_t>, std::unique_ptr<LiveTile>>, TileID::Hash> tiles;
    std::unordered_map<uint32_t, std::unique_ptr<GeoJSONVT>> shapeTilers;
    std::unordered_set<TileID, TileID::Hash> stalePointTileIDs;
    uint32_t nextID_ = 0;
};

}

#endif
