#include <mbgl/tile/tile_cache.hpp>
#include <mbgl/tile/tile_data.hpp>

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
}

void TileCache::add(const OverscaledTileID& key, std::unique_ptr<TileData> data) {
    if (!data->isRenderable() || !size) {
        return;
    }

    // insert new or query existing data
    if (tiles.emplace(key, std::move(data)).second) {
        // remove existing data key
        orderedKeys.remove(key);
    }

    // (re-)insert data key as newest
    orderedKeys.push_back(key);

    // purge oldest key/data if necessary
    if (orderedKeys.size() > size) {
        get(orderedKeys.front());
    }

    assert(orderedKeys.size() <= size);
};

std::unique_ptr<TileData> TileCache::get(const OverscaledTileID& key) {

    std::unique_ptr<TileData> data;

    auto it = tiles.find(key);
    if (it != tiles.end()) {
        data = std::move(it->second);
        tiles.erase(it);
        orderedKeys.remove(key);
        assert(data->isRenderable());
    }

    return data;
};

bool TileCache::has(const OverscaledTileID& key) {
    return tiles.find(key) != tiles.end();
}

void TileCache::clear() {
    orderedKeys.clear();
    tiles.clear();
}

} // namespace mbgl
