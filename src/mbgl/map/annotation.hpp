#ifndef MBGL_MAP_ANNOTATIONS
#define MBGL_MAP_ANNOTATIONS

#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>
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
class MapData;

using AnnotationIDs = std::vector<uint32_t>;

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager();
    ~AnnotationManager();

    void setDefaultPointAnnotationSymbol(const std::string& symbol);
    std::pair<std::vector<TileID>, AnnotationIDs> addPointAnnotations(
        const std::vector<LatLng>&, const std::vector<std::string>& symbols, const MapData&);
    std::vector<TileID> removeAnnotations(const AnnotationIDs&, const MapData&);
    AnnotationIDs getAnnotationsInBounds(const LatLngBounds&, const MapData&) const;
    LatLngBounds getBoundsForAnnotations(const AnnotationIDs&) const;

    const LiveTile* getTile(const TileID& id);

    static const std::string layerID;

private:
    inline uint32_t nextID();
    static vec2<double> projectPoint(const LatLng& point);

private:
    mutable std::mutex mtx;
    std::string defaultPointAnnotationSymbol;
    std::unordered_map<uint32_t, std::unique_ptr<Annotation>> annotations;
    std::unordered_map<TileID, std::pair<std::unordered_set<uint32_t>, std::unique_ptr<LiveTile>>, TileID::Hash> tiles;
    uint32_t nextID_ = 0;
};

}

#endif
