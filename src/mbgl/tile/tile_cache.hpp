#ifndef MBGL_MAP_TILE_CACHE
#define MBGL_MAP_TILE_CACHE

#include <mbgl/tile/tile_id.hpp>

#include <list>
#include <memory>
#include <map>

namespace mbgl {

class TileData;

class TileCache {
public:
    TileCache(size_t size_ = 0) : size(size_) {}

    void setSize(size_t);
    size_t getSize() const { return size; };
    void add(const OverscaledTileID& key, std::unique_ptr<TileData> data);
    std::unique_ptr<TileData> get(const OverscaledTileID& key);
    bool has(const OverscaledTileID& key);
    void clear();

private:
    std::map<OverscaledTileID, std::unique_ptr<TileData>> tiles;
    std::list<OverscaledTileID> orderedKeys;

    size_t size;
};

} // namespace mbgl

#endif
