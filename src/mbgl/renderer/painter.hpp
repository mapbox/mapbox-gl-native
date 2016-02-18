#ifndef MBGL_RENDERER_PAINTER
#define MBGL_RENDERER_PAINTER

#include <mbgl/map/transform_state.hpp>

#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/renderer/bucket.hpp>

#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/static_vertex_buffer.hpp>

#include <mbgl/gl/gl_config.hpp>

#include <mbgl/style/types.hpp>
#include <mbgl/style/style.hpp>

#include <mbgl/gl/gl.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/constants.hpp>

#include <array>
#include <vector>
#include <set>
#include <map>

namespace mbgl {

class MapData;
class Style;
class StyleLayer;
class Tile;
class SpriteAtlas;
class GlyphAtlas;
class LineAtlas;
class Source;
struct FrameData;
class TileData;

class DebugBucket;
class FillBucket;
class FillLayer;
class LineBucket;
class LineLayer;
class CircleBucket;
class CircleLayer;
class SymbolBucket;
class SymbolLayer;
class RasterBucket;
class RasterLayer;
class BackgroundLayer;

class SDFShader;
class PlainShader;
class OutlineShader;
class LineShader;
class LinejoinShader;
class LineSDFShader;
class LinepatternShader;
class CircleShader;
class PatternShader;
class IconShader;
class RasterShader;
class SDFGlyphShader;
class SDFIconShader;
class DotShader;
class CollisionBoxShader;

struct ClipID;

namespace util {
class GLObjectStore;
}

struct FrameData {
    std::array<uint16_t, 2> framebufferSize;
    TimePoint timePoint;
};

class Painter : private util::noncopyable {
public:
    Painter(MapData&, TransformState&, gl::GLObjectStore&);
    ~Painter();

    void render(const Style& style,
                const FrameData& frame,
                SpriteAtlas& annotationSpriteAtlas);

    // Renders debug information for a tile.
    void renderTileDebug(const Tile& tile);

    // Renders the red debug frame around a tile, visualizing its perimeter.
    void renderDebugFrame(const mat4 &matrix);

    void renderDebugText(TileData&, const mat4&);
    void renderFill(FillBucket&, const FillLayer&, const TileID&, const mat4&);
    void renderLine(LineBucket&, const LineLayer&, const TileID&, const mat4&);
    void renderCircle(CircleBucket&, const CircleLayer&, const TileID&, const mat4&);
    void renderSymbol(SymbolBucket&, const SymbolLayer&, const TileID&, const mat4&);
    void renderRaster(RasterBucket&, const RasterLayer&, const TileID&, const mat4&);
    void renderBackground(const BackgroundLayer&);

    float saturationFactor(float saturation);
    float contrastFactor(float contrast);
    std::array<float, 3> spinWeights(float spin_value);

    void drawClippingMasks(const std::map<TileID, ClipID>&);

    bool needsAnimation() const;

private:
    mat4 translatedMatrix(const mat4& matrix, const std::array<float, 2> &translation, const TileID &id, TranslateAnchorType anchor);

    std::vector<RenderItem> determineRenderOrder(const Style& style);

    template <class Iterator>
    void renderPass(RenderPass,
                    Iterator it, Iterator end,
                    GLsizei i, int8_t increment);

    void setClipping(const ClipID&);

    template <typename BucketProperties, typename StyleProperties>
    void renderSDF(SymbolBucket &bucket,
                   const TileID &id,
                   const mat4 &matrixSymbol,
                   const BucketProperties& bucketProperties,
                   const StyleProperties& styleProperties,
                   float scaleDivisor,
                   std::array<float, 2> texsize,
                   SDFShader& sdfShader,
                   void (SymbolBucket::*drawSDF)(SDFShader&, gl::GLObjectStore&));

    void setDepthSublayer(int n);

    mat4 projMatrix;
    mat4 nativeMatrix;
    mat4 extrudeMatrix;

    // used to composite images and flips the geometry upside down
    const mat4 flipMatrix = []{
        mat4 flip;
        matrix::ortho(flip, 0, util::EXTENT, -util::EXTENT, 0, 0, 1);
        matrix::translate(flip, flip, 0, -util::EXTENT, 0);
        return flip;
    }();

    const mat4 identityMatrix = []{
        mat4 identity;
        matrix::identity(identity);
        return identity;
    }();

    MapData& data;
    TransformState& state;
    gl::GLObjectStore& glObjectStore;

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

    std::unique_ptr<PlainShader> plainShader;
    std::unique_ptr<OutlineShader> outlineShader;
    std::unique_ptr<LineShader> lineShader;
    std::unique_ptr<LineSDFShader> linesdfShader;
    std::unique_ptr<LinepatternShader> linepatternShader;
    std::unique_ptr<PatternShader> patternShader;
    std::unique_ptr<IconShader> iconShader;
    std::unique_ptr<RasterShader> rasterShader;
    std::unique_ptr<SDFGlyphShader> sdfGlyphShader;
    std::unique_ptr<SDFIconShader> sdfIconShader;
    std::unique_ptr<DotShader> dotShader;
    std::unique_ptr<CollisionBoxShader> collisionBoxShader;
    std::unique_ptr<CircleShader> circleShader;

    // Set up the stencil quad we're using to generate the stencil mask.
    StaticVertexBuffer tileStencilBuffer = {
        // top left triangle
        { 0, 0 },
        { util::EXTENT, 0 },
        { 0, util::EXTENT },

        // bottom right triangle
        { util::EXTENT, 0 },
        { 0, util::EXTENT },
        { util::EXTENT, util::EXTENT },
    };

    VertexArrayObject coveringPlainArray;
    VertexArrayObject coveringRasterArray;
    VertexArrayObject backgroundPatternArray;
    VertexArrayObject backgroundArray;

    // Set up the tile boundary lines we're using to draw the tile outlines.
    StaticVertexBuffer tileBorderBuffer = {
        { 0, 0 },
        { util::EXTENT, 0 },
        { util::EXTENT, util::EXTENT },
        { 0, util::EXTENT },
        { 0, 0 },
    };

    VertexArrayObject tileBorderArray;
};

} // namespace mbgl

#endif
