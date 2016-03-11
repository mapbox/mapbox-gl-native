#ifndef MBGL_MAP_TILE
#define MBGL_MAP_TILE

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/clip_id.hpp>

namespace mbgl {

class TileData;
struct box;

class Tile : private util::noncopyable {
public:
    Tile(const UnwrappedTileID& id_) : id(id_) {
    }

    const UnwrappedTileID id;
    ClipID clip;
    mat4 matrix;
    util::ptr<TileData> data;
};

} // namespace mbgl

#endif
