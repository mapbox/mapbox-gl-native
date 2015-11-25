#ifndef MBGL_MAP_TILE_WORKER
#define MBGL_MAP_TILE_WORKER

#include <mapbox/variant.hpp>

#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/text/placement_config.hpp>

#include <string>
#include <memory>
#include <mutex>
#include <list>
#include <unordered_map>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wshadow"
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-register"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#pragma GCC diagnostic pop

namespace mbgl {

class CollisionTile;
class GeometryTile;
class Style;
class Bucket;
class StyleLayer;

namespace FeatureBG = boost::geometry;
namespace FeatureBGM = FeatureBG::model;
namespace FeatureBGI = FeatureBG::index;
typedef FeatureBGM::point<int16_t, 2, FeatureBG::cs::cartesian> FeaturePoint;
typedef FeatureBGM::box<FeaturePoint> FeatureBox;
typedef std::pair<FeatureBox, std::string> Feature;
typedef FeatureBGI::rtree<Feature, FeatureBGI::linear<16, 4>> FeatureTree;

// We're using this class to shuttle the resulting buckets from the worker thread to the MapContext
// thread. This class is movable-only because the vector contains movable-only value elements.
class TileParseResultBuckets {
public:
    TileData::State state = TileData::State::invalid;
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
    FeatureTree featureTree;
};

using TileParseResult = mapbox::util::variant<TileParseResultBuckets, // success
                                              std::string>;           // error

class TileWorker : public util::noncopyable {
public:
    TileWorker(TileID,
               std::string sourceID,
               Style&,
               const std::atomic<TileData::State>&);
    ~TileWorker();

    TileParseResult parseAllLayers(std::vector<util::ptr<StyleLayer>>,
                                   const GeometryTile&,
                                   PlacementConfig);

    TileParseResult parsePendingLayers();

    void redoPlacement(std::vector<util::ptr<StyleLayer>>,
                       const std::unordered_map<std::string, std::unique_ptr<Bucket>>*,
                       PlacementConfig);

private:
    void parseLayer(const StyleLayer&, const GeometryTile&);
    void insertBucket(const std::string& name, std::unique_ptr<Bucket>);

    const TileID id;
    const std::string sourceID;

    Style& style;
    const std::atomic<TileData::State>& state;

    bool partialParse = false;

    std::unique_ptr<CollisionTile> collisionTile;

    // Contains buckets that we couldn't parse so far due to missing resources.
    // They will be attempted on subsequent parses.
    std::list<std::pair<const StyleLayer&, std::unique_ptr<Bucket>>> pending;

    // Temporary holder
    TileParseResultBuckets result;
};

} // namespace mbgl

#endif
