#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/renderer/tile_mask.hpp>

#include <array>
#include <memory>

namespace mbgl {

namespace gfx {
class UploadPass;
} // namespace gfx

class Bucket;
class LayerRenderData;
class Tile;
class TransformState;
class PaintParameters;
class DebugBucket;
class SourcePrepareParameters;
class FeatureIndex;

class RenderTile final {
public:
    RenderTile(UnwrappedTileID, Tile&);
    ~RenderTile();
    RenderTile(const RenderTile&) = delete;
    RenderTile(RenderTile&&) = default;
    RenderTile& operator=(const RenderTile&) = delete;
    RenderTile& operator=(RenderTile&&) = default;

    UnwrappedTileID id;
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

    const OverscaledTileID& getOverscaledTileID() const;
    bool holdForFade() const;

    Bucket* getBucket(const style::Layer::Impl&) const;
    const LayerRenderData* getLayerRenderData(const style::Layer::Impl&) const;
    optional<ImagePosition> getPattern(const std::string& pattern) const;
    const optional<gfx::Texture>& getGlyphAtlasTexture() const;
    const optional<gfx::Texture>& getIconAtlasTexture() const;
    std::shared_ptr<FeatureIndex> getFeatureIndex() const;

    void setMask(TileMask&&);
    void upload(gfx::UploadPass&);
    void prepare(const SourcePrepareParameters&);
    void finishRender(PaintParameters&);

    mat4 translateVtxMatrix(const mat4& tileMatrix,
                            const std::array<float, 2>& translation,
                            style::TranslateAnchorType anchor,
                            const TransformState& state,
                            const bool inViewportPixelUnits) const;
private:
    friend class TilePyramid;
    Tile& tile;
};

} // namespace mbgl
