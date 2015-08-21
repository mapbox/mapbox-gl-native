#ifndef MBGL_MAP_TILE_CACHE
#define MBGL_MAP_TILE_CACHE

#include <mbgl/map/tile_data.hpp>

#include <list>
#include <unordered_map>

namespace mbgl {

class TileCache {
public:
    TileCache(size_t size_ = 0) : size(size_) {}

    void setSize(size_t);
    size_t getSize() const { return size; };
    void add(uint64_t key, std::shared_ptr<TileData> data);
    std::shared_ptr<TileData> get(uint64_t key);
    bool has(uint64_t key);
    void clear();
private:
    std::unordered_map<uint64_t, std::shared_ptr<TileData>> tiles;
    std::list<uint64_t> orderedKeys;

    size_t size;
};

};

#endif
