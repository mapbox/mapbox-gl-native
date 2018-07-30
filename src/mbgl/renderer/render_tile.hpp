#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/clip_id.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/renderer/tile_mask.hpp>

#include <array>

namespace mbgl {

class Tile;
class TransformState;
class PaintParameters;

class RenderTile final {
public:
    RenderTile(UnwrappedTileID id_, Tile& tile_) : id(std::move(id_)), tile(tile_) {}
    RenderTile(const RenderTile&) = delete;
    RenderTile(RenderTile&&) = default;
    RenderTile& operator=(const RenderTile&) = delete;
    RenderTile& operator=(RenderTile&&) = default;

    UnwrappedTileID id;
    Tile& tile;
    ClipID clip;
    mat4 matrix;
    mat4 nearClippedMatrix;
    bool used = false;
    bool needsClipping = false;

    mat4 translatedMatrix(const std::array<float, 2>& translate,
                          style::TranslateAnchorType anchor,
                          const TransformState&) const;

    mat4 translatedClipMatrix(const std::array<float, 2>& translate,
                              style::TranslateAnchorType anchor,
                              const TransformState&) const;

    void setMask(TileMask&&);
    void startRender(PaintParameters&);
    void finishRender(PaintParameters&);

    mat4 translateVtxMatrix(const mat4& tileMatrix,
                            const std::array<float, 2>& translation,
                            style::TranslateAnchorType anchor,
                            const TransformState& state,
                            const bool inViewportPixelUnits) const;
};

} // namespace mbgl
