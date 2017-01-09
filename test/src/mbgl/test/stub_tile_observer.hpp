#pragma once

#include <mbgl/tile/tile_observer.hpp>

using namespace mbgl;

/**
 * An implementation of TileObserver that forwards all methods to dynamically-settable lambas.
 */
class StubTileObserver : public TileObserver {
public:
    void onTileChanged(Tile& tile) override {
        if (tileChanged) tileChanged(tile);
    }

    void onTileError(Tile& tile, std::exception_ptr error) override {
        if (tileError) tileError(tile, error);
    }

    std::function<void (Tile&)> tileChanged;
    std::function<void (Tile&, std::exception_ptr)> tileError;
};
