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
#include <map>
#include <unordered_set>

namespace mbgl {

class LiveTile;
class MapData;
class PointAnnotationLayer;

using AnnotationID = uint32_t;
using AnnotationIDs = std::vector<AnnotationID>;

enum class AnnotationType : uint8_t {
    Point,
    Shape
};

class Annotation : private util::noncopyable {
public:
    Annotation(AnnotationID, AnnotationType, const std::string& symbol, const LatLng&);

public:
    inline AnnotationType getType() const { return type; }
    inline const std::string& getSymbol() const { return symbol; }

public:
    const AnnotationID id;
    const AnnotationType type = AnnotationType::Point;
    const std::string symbol;
    const LatLng point;
};


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

    // Returns a list of all annotations for the given layer that are in the bounds of the tile.
    std::vector<std::shared_ptr<const Annotation>> getAnnotations(const std::string& layer, const TileID& id) const;

    std::shared_ptr<const LiveTile> getTile(const TileID& id);

    static const std::string layerID;

private:
    inline uint32_t nextID();
    static vec2<double> projectPoint(const LatLng& point);

private:
    mutable std::mutex mtx;
    std::string defaultPointAnnotationSymbol;
    std::map<uint32_t, std::shared_ptr<const Annotation>> annotationIndex;
    const std::unique_ptr<PointAnnotationLayer> annotations;
    uint32_t nextID_ = 0;
};

}

#endif
