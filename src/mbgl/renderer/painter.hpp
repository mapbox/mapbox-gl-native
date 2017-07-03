#pragma once

#include <mbgl/map/transform_state.hpp>

#include <mbgl/tile/tile_id.hpp>

#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/renderer/render_item.hpp>
#include <mbgl/renderer/bucket.hpp>

#include <mbgl/gl/context.hpp>
#include <mbgl/programs/debug_program.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/programs/extrusion_texture_program.hpp>
#include <mbgl/programs/raster_program.hpp>

#include <mbgl/style/style.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/offscreen_texture.hpp>

#include <array>
#include <vector>
#include <set>
#include <map>

namespace mbgl {

class RenderTile;
class SpriteAtlas;
class View;
class GlyphAtlas;
class LineAtlas;
struct FrameData;
class Tile;

class DebugBucket;
class FillBucket;
class FillExtrusionBucket;
class LineBucket;
class CircleBucket;
class SymbolBucket;
class RasterBucket;

class RenderFillLayer;
class RenderFillExtrusionLayer;
class RenderLineLayer;
class RenderCircleLayer;
class RenderSymbolLayer;
class RenderRasterLayer;
class RenderBackgroundLayer;

class Programs;
class PaintParameters;
class TilePyramid;

struct ClipID;

namespace style {
class Style;
class Source;
} // namespace style

struct FrameData {
    TimePoint timePoint;
    float pixelRatio;
    MapMode mapMode;
    GLContextMode contextMode;
    MapDebugOptions debugOptions;
};

class Painter : private util::noncopyable {
public:
    Painter(gl::Context&, const TransformState&, float pixelRatio, const optional<std::string>& programCacheDir);
    ~Painter();

    void render(const style::Style&,
                const FrameData&,
                View&,
                SpriteAtlas& annotationSpriteAtlas);

    void cleanup();

    void renderClippingMask(const UnwrappedTileID&, const ClipID&);
    void renderTileDebug(const RenderTile&);
    void renderFill(PaintParameters&, FillBucket&, const RenderFillLayer&, const RenderTile&);
    void renderFillExtrusion(PaintParameters&, FillExtrusionBucket&, const RenderFillExtrusionLayer&, const RenderTile&);
    void renderLine(PaintParameters&, LineBucket&, const RenderLineLayer&, const RenderTile&);
    void renderCircle(PaintParameters&, CircleBucket&, const RenderCircleLayer&, const RenderTile&);
    void renderSymbol(PaintParameters&, SymbolBucket&, const RenderSymbolLayer&, const RenderTile&);
    void renderRaster(PaintParameters&, RasterBucket&, const RenderRasterLayer&, const RenderTile&);
    void renderBackground(PaintParameters&, const RenderBackgroundLayer&);

#ifndef NDEBUG
    // Renders tile clip boundaries, using stencil buffer to calculate fill color.
    void renderClipMasks(PaintParameters&);
    // Renders the depth buffer.
    void renderDepthBuffer(PaintParameters&);
#endif

    bool needsAnimation() const;

private:
    std::vector<RenderItem> determineRenderOrder(const style::Style&);

    template <class Iterator>
    void renderPass(PaintParameters&,
                    RenderPass,
                    Iterator it, Iterator end,
                    uint32_t i, int8_t increment);

    mat4 matrixForTile(const UnwrappedTileID&);
    gl::DepthMode depthModeForSublayer(uint8_t n, gl::DepthMode::Mask) const;
    gl::StencilMode stencilModeForClipping(const ClipID&) const;
    gl::ColorMode colorModeForRenderPass() const;

#ifndef NDEBUG
    PaintMode paintMode() const {
        return frame.debugOptions & MapDebugOptions::Overdraw ? PaintMode::Overdraw
                                                              : PaintMode::Regular;
    }
#else
    PaintMode paintMode() const {
        return PaintMode::Regular;
    }
#endif

private:
    gl::Context& context;

    mat4 projMatrix;
    mat4 nearClippedProjMatrix;

    std::array<float, 2> pixelsToGLUnits;

    const mat4 identityMatrix = []{
        mat4 identity;
        matrix::identity(identity);
        return identity;
    }();

    const TransformState& state;

    FrameData frame;

    int indent = 0;

    RenderPass pass = RenderPass::Opaque;

    int numSublayers = 3;
    uint32_t currentLayer;
    float depthRangeSize;
    const float depthEpsilon = 1.0f / (1 << 16);

    SpriteAtlas* spriteAtlas = nullptr;
    GlyphAtlas* glyphAtlas = nullptr;
    LineAtlas* lineAtlas = nullptr;

    optional<OffscreenTexture> extrusionTexture;

    EvaluatedLight evaluatedLight;

    FrameHistory frameHistory;

    std::unique_ptr<Programs> programs;
#ifndef NDEBUG
    std::unique_ptr<Programs> overdrawPrograms;
#endif

    gl::VertexBuffer<FillLayoutVertex> tileVertexBuffer;
    gl::VertexBuffer<RasterLayoutVertex> rasterVertexBuffer;
    gl::VertexBuffer<ExtrusionTextureLayoutVertex> extrusionTextureVertexBuffer;

    gl::IndexBuffer<gl::Triangles> quadTriangleIndexBuffer;
    gl::IndexBuffer<gl::LineStrip> tileBorderIndexBuffer;

    SegmentVector<FillAttributes> tileTriangleSegments;
    SegmentVector<DebugAttributes> tileBorderSegments;
    SegmentVector<RasterAttributes> rasterSegments;
    SegmentVector<ExtrusionTextureAttributes> extrusionTextureSegments;
};

} // namespace mbgl
