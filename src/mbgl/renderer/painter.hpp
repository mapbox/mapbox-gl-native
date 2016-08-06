#pragma once

#include <mbgl/map/transform_state.hpp>

#include <mbgl/tile/tile_id.hpp>

#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/renderer/render_item.hpp>
#include <mbgl/renderer/bucket.hpp>

#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/static_vertex_buffer.hpp>

#include <mbgl/gl/gl_config.hpp>
#include <mbgl/gl/gl.hpp>

#include <mbgl/style/style.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/constants.hpp>

#include <array>
#include <vector>
#include <set>
#include <map>

namespace mbgl {

class RenderTile;
class SpriteAtlas;
class GlyphAtlas;
class LineAtlas;
struct FrameData;
class Tile;

class DebugBucket;
class FillBucket;
class LineBucket;
class CircleBucket;
class SymbolBucket;
class RasterBucket;

class Shaders;
class SDFShader;
class PaintParameters;

struct ClipID;

namespace util {
class ObjectStore;
} // namespace util

namespace style {
class Style;
class Source;
class FillLayer;
class LineLayer;
class CircleLayer;
class SymbolLayer;
class RasterLayer;
class BackgroundLayer;
} // namespace style

struct FrameData {
    std::array<uint16_t, 2> framebufferSize;
    TimePoint timePoint;
    float pixelRatio;
    MapMode mapMode;
    GLContextMode contextMode;
    MapDebugOptions debugOptions;
};

class Painter : private util::noncopyable {
public:
    Painter(const TransformState&, gl::ObjectStore&);
    ~Painter();

    void render(const style::Style&,
                const FrameData&,
                SpriteAtlas& annotationSpriteAtlas);

    // Renders debug information for a tile.
    void renderTileDebug(const RenderTile&);

    // Renders the red debug frame around a tile, visualizing its perimeter.
    void renderDebugFrame(const mat4 &matrix);

#ifndef NDEBUG
    // Renders tile clip boundaries, using stencil buffer to calculate fill color.
    void renderClipMasks();
#endif

    void renderDebugText(Tile&, const mat4&);
    void renderFill(PaintParameters&, FillBucket&, const style::FillLayer&, const RenderTile&);
    void renderLine(PaintParameters&, LineBucket&, const style::LineLayer&, const RenderTile&);
    void renderCircle(PaintParameters&, CircleBucket&, const style::CircleLayer&, const RenderTile&);
    void renderSymbol(PaintParameters&, SymbolBucket&, const style::SymbolLayer&, const RenderTile&);
    void renderRaster(PaintParameters&, RasterBucket&, const style::RasterLayer&, const RenderTile&);
    void renderBackground(PaintParameters&, const style::BackgroundLayer&);

    float saturationFactor(float saturation);
    float contrastFactor(float contrast);
    std::array<float, 3> spinWeights(float spin_value);

    void drawClippingMasks(PaintParameters&, const std::map<UnwrappedTileID, ClipID>&);

    bool needsAnimation() const;

private:
    std::vector<RenderItem> determineRenderOrder(const style::Style&);

    template <class Iterator>
    void renderPass(PaintParameters&,
                    RenderPass,
                    Iterator it, Iterator end,
                    GLsizei i, int8_t increment);

    void setClipping(const ClipID&);

    void renderSDF(SymbolBucket&,
                   const RenderTile&,
                   float scaleDivisor,
                   std::array<float, 2> texsize,
                   SDFShader& sdfShader,
                   void (SymbolBucket::*drawSDF)(SDFShader&, gl::ObjectStore&, bool),

                   // Layout
                   style::AlignmentType rotationAlignment,
                   style::AlignmentType pitchAlignment,
                   float layoutSize,

                   // Paint
                   float opacity,
                   Color color,
                   Color haloColor,
                   float haloWidth,
                   float haloBlur,
                   std::array<float, 2> translate,
                   style::TranslateAnchorType translateAnchor,
                   float paintSize);

    void setDepthSublayer(int n);

#ifndef NDEBUG
    bool isOverdraw() const { return frame.debugOptions & MapDebugOptions::Overdraw; }
#else
    bool isOverdraw() const { return false; }
#endif

    mat4 projMatrix;

    std::array<float, 2> pixelsToGLUnits;

    const mat4 identityMatrix = []{
        mat4 identity;
        matrix::identity(identity);
        return identity;
    }();

    const TransformState& state;
    gl::ObjectStore& store;

    FrameData frame;

    int indent = 0;

    gl::Config config;

    RenderPass pass = RenderPass::Opaque;

    int numSublayers = 3;
    GLsizei currentLayer;
    float depthRangeSize;
    const float depthEpsilon = 1.0f / (1 << 16);

    SpriteAtlas* spriteAtlas = nullptr;
    GlyphAtlas* glyphAtlas = nullptr;
    LineAtlas* lineAtlas = nullptr;

    FrameHistory frameHistory;

    std::unique_ptr<Shaders> shaders;
#ifndef NDEBUG
    std::unique_ptr<Shaders> overdrawShaders;
#endif

    // Set up the stencil quad we're using to generate the stencil mask.
    StaticVertexBuffer tileStencilBuffer {
        // top left triangle
        {{ 0, 0 }},
        {{ util::EXTENT, 0 }},
        {{ 0, util::EXTENT }},

        // bottom right triangle
        {{ util::EXTENT, 0 }},
        {{ 0, util::EXTENT }},
        {{ util::EXTENT, util::EXTENT }},
    };

    StaticRasterVertexBuffer rasterBoundsBuffer {
        {{ 0, 0, 0, 0 }},
        {{ util::EXTENT, 0, 32767, 0 }},
        {{ 0, util::EXTENT, 0, 32767 }},
        {{ util::EXTENT, util::EXTENT, 32767, 32767 }},
    };

    // Set up the tile boundary lines we're using to draw the tile outlines.
    StaticVertexBuffer tileBorderBuffer {
        {{ 0, 0 }},
        {{ util::EXTENT, 0 }},
        {{ util::EXTENT, util::EXTENT }},
        {{ 0, util::EXTENT }},
        {{ 0, 0 }},
    };

    VertexArrayObject tileBorderArray;
};

} // namespace mbgl
