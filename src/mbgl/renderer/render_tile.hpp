#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/clip_id.hpp>

namespace mbgl {

class TileData;

class RenderTile {
public:
    RenderTile(const UnwrappedTileID& id_, TileData& data_) : id(id_), data(data_) {
    }

    RenderTile(const RenderTile&) = delete;
    RenderTile(RenderTile&&) = default;
    RenderTile& operator=(const RenderTile&) = delete;
    RenderTile& operator=(RenderTile&&) = default;

    const UnwrappedTileID id;
    TileData& data;
    ClipID clip;
    mat4 matrix;
};

} // namespace mbgl
