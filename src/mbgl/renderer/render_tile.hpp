#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/renderer/tile_mask.hpp>

#include <array>
#include <memory>

namespace mbgl {

namespace gfx {
class UploadPass;
} // namespace gfx

class Tile;
class TransformState;
class PaintParameters;
class DebugBucket;

class RenderTile final {
public:
    RenderTile(UnwrappedTileID, Tile&);
    ~RenderTile();
    RenderTile(const RenderTile&) = delete;
    RenderTile(RenderTile&&) = default;
    RenderTile& operator=(const RenderTile&) = delete;
    RenderTile& operator=(RenderTile&&) = default;

    UnwrappedTileID id;
    Tile& tile;
    mat4 matrix;
    mat4 nearClippedMatrix;
    bool used = false;
    // Contains the tile ID string for painting debug information.
    std::unique_ptr<DebugBucket> debugBucket;

    mat4 translatedMatrix(const std::array<float, 2>& translate,
                          style::TranslateAnchorType anchor,
                          const TransformState&) const;

    mat4 translatedClipMatrix(const std::array<float, 2>& translate,
                              style::TranslateAnchorType anchor,
                              const TransformState&) const;

    void setMask(TileMask&&);
    void upload(gfx::UploadPass&);
    void prepare(PaintParameters&);
    void finishRender(PaintParameters&);

    mat4 translateVtxMatrix(const mat4& tileMatrix,
                            const std::array<float, 2>& translation,
                            style::TranslateAnchorType anchor,
                            const TransformState& state,
                            const bool inViewportPixelUnits) const;
};

} // namespace mbgl
