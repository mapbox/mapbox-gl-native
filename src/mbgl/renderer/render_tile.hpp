#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/clip_id.hpp>
#include <mbgl/style/types.hpp>

#include <array>

namespace mbgl {

class Tile;
class TransformState;

class RenderTile {
public:
    RenderTile(UnwrappedTileID id_, Tile& tile_) : id(std::move(id_)), tile(tile_) {}
    RenderTile(const RenderTile&) = delete;
    RenderTile(RenderTile&&) = default;
    RenderTile& operator=(const RenderTile&) = delete;
    RenderTile& operator=(RenderTile&&) = default;

    const UnwrappedTileID id;
    Tile& tile;
    ClipID clip;
    mat4 matrix;
    bool used = false;

    mat4 translatedMatrix(const std::array<float, 2>& translate,
                          style::TranslateAnchorType anchor,
                          const TransformState&) const;
};

} // namespace mbgl
