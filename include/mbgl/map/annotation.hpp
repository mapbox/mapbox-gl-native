#ifndef MBGL_MAP_ANNOTATIONS
#define MBGL_MAP_ANNOTATIONS

#include <mbgl/map/tile.hpp>
#include <mbgl/map/live_tile.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/vec.hpp>

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <unordered_map>

namespace mbgl {

class Annotation;
class Map;

typedef std::vector<LatLng> AnnotationSegment;

enum class AnnotationType : uint8_t {
    Point,
    Shape
};

class AnnotationManager : private util::noncopyable {
public:
    AnnotationManager();

    void setDefaultPointAnnotationSymbol(std::string& symbol) { defaultPointAnnotationSymbol = symbol; }
    std::pair<std::vector<Tile::ID>, std::vector<uint32_t>> addPointAnnotations(std::vector<LatLng>, std::vector<std::string>& symbols, const Map&);
    std::vector<Tile::ID> removeAnnotations(std::vector<uint32_t>, const Map&);
    std::vector<uint32_t> getAnnotationsInBounds(LatLngBounds, const Map&) const;
    LatLngBounds getBoundsForAnnotations(std::vector<uint32_t>) const;

    const std::unique_ptr<LiveTile>& getTile(Tile::ID const& id);

private:
    uint32_t nextID() { return nextID_++; }
    static vec2<double> projectPoint(LatLng& point);

private:
    std::mutex mtx;
    std::string defaultPointAnnotationSymbol;
    std::unordered_map<uint32_t, std::unique_ptr<Annotation>> annotations;
    std::unordered_map<Tile::ID, std::pair<std::vector<uint32_t>, std::unique_ptr<LiveTile>>, Tile::ID::Hash> tiles;
    std::unique_ptr<LiveTile> nullTile;
    uint32_t nextID_ = 0;
};

class Annotation : private util::noncopyable {
    friend class AnnotationManager;
public:
    Annotation(AnnotationType, std::vector<AnnotationSegment>);

private:
    LatLng getPoint() const;
    LatLngBounds getBounds() const { return bounds; }

private:
    const AnnotationType type = AnnotationType::Point;
    const std::vector<AnnotationSegment> geometry;
    std::unordered_map<Tile::ID, std::vector<std::size_t>, Tile::ID::Hash> tileFeatures;
    LatLngBounds bounds;
};

}

#endif
