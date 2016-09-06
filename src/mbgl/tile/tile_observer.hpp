#pragma once

#include <exception>

namespace mbgl {

class Tile;

enum class TileLoadState : bool {
    First = true,
    Subsequent = false,
};

class TileObserver {
public:
    virtual ~TileObserver() = default;

    virtual void onTileLoaded(Tile&, TileLoadState) {}
    virtual void onTileError(Tile&, std::exception_ptr) {}
    virtual void onTileUpdated(Tile&) {}
};

} // namespace mbgl
