#pragma once

#include <exception>

namespace mbgl {

class Tile;

class TileObserver {
public:
    virtual ~TileObserver() = default;

    virtual void onTileLoaded(Tile&, bool /*isNewTile*/) {}
    virtual void onTileError(Tile&, std::exception_ptr) {}
    virtual void onNeedsRepaint() {}
};

} // namespace mbgl
