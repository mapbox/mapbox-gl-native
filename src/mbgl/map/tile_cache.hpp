#ifndef MBGL_MAP_TILE_CACHE
#define MBGL_MAP_TILE_CACHE

#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/tile_data.hpp>

#include <list>
#include <unordered_map>

namespace mbgl{


class TileCache {
    public:
        TileCache() : size(20) {}

        void add(uint64_t key, std::shared_ptr<TileData> data);
        std::shared_ptr<TileData> get(uint64_t key);
        bool has(uint64_t key);
        void clear();
    private:
        std::unordered_map<uint64_t, std::shared_ptr<TileData>> tiles;
        std::list<uint64_t> orderedKeys;

        const unsigned int size;
};

};
#endif
