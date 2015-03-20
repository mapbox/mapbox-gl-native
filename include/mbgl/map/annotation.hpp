#ifndef MBGL_MAP_ANNOTATIONS
#define MBGL_MAP_ANNOTATIONS

#include <mbgl/map/tile.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/vec.hpp>

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>

namespace mbgl {

class Annotation;
class Map;
class LiveTile;

using AnnotationIDs = std::vector<uint32_t>;

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager();
    ~AnnotationManager();

    void setDefaultPointAnnotationSymbol(const std::string& symbol);
    std::pair<std::vector<Tile::ID>, AnnotationIDs> addPointAnnotations(
        const std::vector<LatLng>&, const std::vector<std::string>& symbols, const Map&);
    std::vector<Tile::ID> removeAnnotations(const AnnotationIDs&);
    AnnotationIDs getAnnotationsInBounds(const LatLngBounds&, const Map&) const;
    LatLngBounds getBoundsForAnnotations(const AnnotationIDs&) const;

    const LiveTile* getTile(Tile::ID const& id);

    static const std::string layerID;

private:
    inline uint32_t nextID();
    static vec2<double> projectPoint(const LatLng& point);

private:
    mutable std::mutex mtx;
    std::string defaultPointAnnotationSymbol;
    std::map<uint32_t, std::unique_ptr<Annotation>> annotations;
    std::map<Tile::ID, std::pair<AnnotationIDs, std::unique_ptr<LiveTile>>> annotationTiles;
    uint32_t nextID_ = 0;
};

}

#endif
