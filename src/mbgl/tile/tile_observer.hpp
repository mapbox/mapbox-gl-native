#pragma once

#include <exception>

namespace mbgl {

class Tile;

class TileObserver {
public:
    virtual ~TileObserver() = default;

    virtual void onTileChanged(Tile&) {}
    virtual void onTileError(Tile&, std::exception_ptr) {}
};

} // namespace mbgl
