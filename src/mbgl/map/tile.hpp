#ifndef MBGL_MAP_TILE
#define MBGL_MAP_TILE

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/clip_id.hpp>
#include <mbgl/map/tile_id.hpp>

namespace mbgl {

class TileData;
struct box;

class Tile : private util::noncopyable {
public:
    explicit Tile(const TileID& id_)
        : id(id_) {}

    const TileID id;
    ClipID clip;
    mat4 matrix;
    util::ptr<TileData> data;
};

}

#endif
