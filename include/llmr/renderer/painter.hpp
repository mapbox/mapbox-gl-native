#ifndef LLMR_RENDERER_PAINTER
#define LLMR_RENDERER_PAINTER

#include <llmr/map/tile_data.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/geometry/vertex_buffer.hpp>
#include <llmr/util/mat4.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/renderer/frame_history.hpp>
#include <llmr/style/properties.hpp>

#include <llmr/shader/plain_shader.hpp>
#include <llmr/shader/outline_shader.hpp>
#include <llmr/shader/pattern_shader.hpp>
#include <llmr/shader/line_shader.hpp>
#include <llmr/shader/linejoin_shader.hpp>
#include <llmr/shader/point_shader.hpp>
#include <llmr/shader/raster_shader.hpp>
#include <llmr/shader/text_shader.hpp>
#include <llmr/shader/dot_shader.hpp>

#include <llmr/map/transform_state.hpp>

namespace llmr {

class Transform;
class Style;
class Tile;
class GlyphAtlas;

class FillBucket;
class LineBucket;
class PointBucket;
class TextBucket;
class RasterBucket;

class LayerDescription;
class RasterTileData;

class Painter : private util::noncopyable {
public:
    Painter(Map &map);


    void setup();
    void clear();

    // Updates the default matrices to the current viewport dimensions.
    void changeMatrix();

    // Renders a particular layer from a tile.
    void renderTileLayer(const Tile& tile, const LayerDescription &layer_desc);

    // Renders debug information for a tile.
    void renderTileDebug(const Tile& tile);

    // Renders the backdrop of the OpenGL view. This also paints in areas where we don't have any
    // tiles whatsoever.
    void renderMatte();

    // Renders the red debug frame around a tile, visualizing its perimeter.
    void renderDebugFrame();

    void renderDebugText(DebugBucket& bucket);
    void renderDebugText(const std::vector<std::string> &strings);
    void renderFill(FillBucket& bucket, const std::string& layer_name, const Tile::ID& id);
    void renderLine(LineBucket& bucket, const std::string& layer_name, const Tile::ID& id);
    void renderPoint(PointBucket& bucket, const std::string& layer_name, const Tile::ID& id);
    void renderText(TextBucket& bucket, const std::string& layer_name, const Tile::ID& id);
    void renderRaster(RasterBucket& bucket, const std::string& layer_name, const Tile::ID& id);

    void resize();

    // Changes whether debug information is drawn onto the map
    void setDebug(bool enabled);

    // Opaque/Translucent pass setting
    void startOpaquePass();
    void startTranslucentPass();
    void endPass();

    // Configures the painter strata that is used for early z-culling of fragments.
    void setStrata(float strata);

    void prepareClippingMask();
    void drawClippingMask(const mat4& matrix, const ClipID& clip);
    void finishClippingMask();

    bool needsAnimation() const;
private:
    void setupShaders();
    void translateLayer(const std::array<float, 2> &translation, const Tile::ID &id, bool reverse = false);
    mat4 translatedMatrix(const std::array<float, 2> &translation, const Tile::ID &id, TranslateAnchor anchor = TranslateAnchor::Map);

    void prepareTile(const Tile& tile);
    void useProgram(uint32_t program);
    void lineWidth(float lineWidth);
    void depthMask(bool value);

public:
    mat4 matrix;
    mat4 projMatrix;
    mat4 nativeMatrix;
    mat4 extrudeMatrix;

private:
    Map& map;

    FrameHistory frameHistory;

    bool debug = false;

    uint32_t gl_program = 0;
    float gl_lineWidth = 0;
    bool gl_depthMask = true;
    std::array<uint16_t, 2> gl_viewport = {{ 0, 0 }};
    float strata = 0;
    enum { Opaque, Translucent } pass = Opaque;
    const float strata_epsilon = 1.0f / (1 << 16);

    std::unique_ptr<PlainShader> plainShader;
    std::unique_ptr<OutlineShader> outlineShader;
    std::unique_ptr<LineShader> lineShader;
    std::unique_ptr<LinejoinShader> linejoinShader;
    std::unique_ptr<PatternShader> patternShader;
    std::unique_ptr<PointShader> pointShader;
    std::unique_ptr<RasterShader> rasterShader;
    std::unique_ptr<TextShader> textShader;
    std::unique_ptr<DotShader> dotShader;

    // Set up the stencil quad we're using to generate the stencil mask.
    VertexBuffer tileStencilBuffer = {
        // top left triangle
        0, 0,
        4096, 0,
        0, 4096,

        // bottom right triangle
        4096, 0,
        0, 4096,
        4096, 4096
    };

    VertexArrayObject coveringPlainArray;
    VertexArrayObject coveringPatternArray;
    VertexArrayObject coveringRasterArray;
    VertexArrayObject matteArray;

    // Set up the tile boundary lines we're using to draw the tile outlines.
    VertexBuffer tileBorderBuffer = {
        0, 0,
        4096, 0,
        4096, 4096,
        0, 4096,
        0, 0
    };

    VertexArrayObject tileBorderArray;

};

}

#endif
