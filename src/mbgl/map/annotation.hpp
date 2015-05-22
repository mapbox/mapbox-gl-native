#ifndef MBGL_MAP_ANNOTATIONS
#define MBGL_MAP_ANNOTATIONS

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/vec.hpp>

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace mbgl {

class Annotation;
class Map;
class LiveTile;
class LiveTileFeature;
class MapData;

using AnnotationIDs = std::vector<uint32_t>;
using AnnotationSegment = std::vector<LatLng>;
using AnnotationSegments = std::vector<AnnotationSegment>;
using AnnotationsProperties = std::unordered_map<std::string, std::vector<std::string>>;

enum class AnnotationType : uint8_t {
    Point,
    Shape
};

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager();
    ~AnnotationManager();

    void setDefaultPointAnnotationSymbol(const std::string& symbol);
    std::pair<std::unordered_set<TileID, TileID::Hash>, AnnotationIDs> addPointAnnotations(
        const AnnotationSegment& points,
        const AnnotationsProperties& properties,
        const MapData&);
    std::pair<std::unordered_set<TileID, TileID::Hash>, AnnotationIDs> addShapeAnnotations(
        const std::vector<AnnotationSegments>& shapes,
        const AnnotationsProperties& properties,
        const MapData&);
    std::unordered_set<TileID, TileID::Hash> removeAnnotations(const AnnotationIDs&, const MapData&);
    AnnotationIDs getAnnotationsInBounds(const LatLngBounds&, const MapData&) const;
    LatLngBounds getBoundsForAnnotations(const AnnotationIDs&) const;

    const LiveTile* getTile(const TileID& id);

    static const std::string PointLayerID;
    static const std::string ShapeLayerID;

private:
    inline uint32_t nextID();
    static vec2<double> projectPoint(const LatLng& point);
    std::pair<std::unordered_set<TileID, TileID::Hash>, AnnotationIDs> addAnnotations(
        const AnnotationType,
        const std::vector<AnnotationSegments>& segments,
        const AnnotationsProperties& properties,
        const MapData&);
    std::unordered_set<TileID, TileID::Hash> addTileFeature(
        const uint32_t annotationID,
        const AnnotationSegments&,
        const std::vector<std::vector<vec2<double>>>& projectedFeature,
        const AnnotationType&,
        const std::unordered_map<std::string, std::string>& properties,
        const uint8_t maxZoom);

private:
    mutable std::mutex mtx;
    std::string defaultPointAnnotationSymbol;
    std::unordered_map<uint32_t, std::unique_ptr<Annotation>> annotations;
    std::unordered_map<TileID, std::pair<std::unordered_set<uint32_t>, std::unique_ptr<LiveTile>>, TileID::Hash> tiles;
    uint32_t nextID_ = 0;
};

}

#endif
