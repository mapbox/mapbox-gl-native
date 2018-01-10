#include <mbgl/tile/tile_cache.hpp>
#include <mbgl/tile/tile.hpp>

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

void TileCache::add(const OverscaledTileID& key, std::unique_ptr<Tile> tile) {
    if (!tile->isRenderable() || !size) {
        return;
    }

    // insert new or query existing tile
    if (tiles.emplace(key, std::move(tile)).second) {
        // remove existing tile key
        orderedKeys.remove(key);
    }

    // (re-)insert tile key as newest
    orderedKeys.push_back(key);

    // purge oldest key/tile if necessary
    if (orderedKeys.size() > size) {
        pop(orderedKeys.front());
    }

    assert(orderedKeys.size() <= size);
}

Tile* TileCache::get(const OverscaledTileID& key) {
    auto it = tiles.find(key);
    if (it != tiles.end()) {
        return it->second.get();
    } else {
        return nullptr;
    }
}

std::unique_ptr<Tile> TileCache::pop(const OverscaledTileID& key) {

    std::unique_ptr<Tile> tile;

    auto it = tiles.find(key);
    if (it != tiles.end()) {
        tile = std::move(it->second);
        tiles.erase(it);
        orderedKeys.remove(key);
        assert(tile->isRenderable());
    }

    return tile;
}

bool TileCache::has(const OverscaledTileID& key) {
    return tiles.find(key) != tiles.end();
}

void TileCache::clear() {
    orderedKeys.clear();
    tiles.clear();
}

} // namespace mbgl
