#include <mbgl/map/tile_cache.hpp>

#include <cassert>

namespace mbgl {

void TileCache::setSize(size_t size_) {
    size = size_;

    while (orderedKeys.size() > size) {
        auto key = orderedKeys.front();
        orderedKeys.pop_front();
        tiles.erase(key);
    }

    assert(orderedKeys.size() <= size);

    tiles.reserve(size);
}

void TileCache::add(uint64_t key, std::shared_ptr<TileData> data) {

    assert(tiles.find(key) == tiles.end());

    tiles.emplace(key, data);
    orderedKeys.push_back(key);

    if (orderedKeys.size() > size) {
        get(orderedKeys.front());
    }

    assert(orderedKeys.size() <= size);
};

std::shared_ptr<TileData> TileCache::get(uint64_t key) {

    std::shared_ptr<TileData> data;

    auto it = tiles.find(key);
    if (it != tiles.end()) {
        data = it->second;
        tiles.erase(it);
        orderedKeys.remove(key);
        assert(data->ready());
    }

    return data;
};

bool TileCache::has(uint64_t key) {
    return tiles.find(key) != tiles.end();
}

void TileCache::clear() {
    orderedKeys.clear();
    tiles.clear();
}

};
