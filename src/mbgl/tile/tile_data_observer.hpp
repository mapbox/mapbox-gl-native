#pragma once

#include <exception>

namespace mbgl {

class TileData;

class TileDataObserver {
public:
    virtual ~TileDataObserver() = default;

    virtual void onTileLoaded(TileData&, bool /*isNewTile*/) {}
    virtual void onTileError(TileData&, std::exception_ptr) {}
    virtual void onPlacementRedone(TileData&) {}
};

} // namespace mbgl
