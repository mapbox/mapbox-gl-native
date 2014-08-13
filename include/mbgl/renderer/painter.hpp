#ifndef MBGL_RENDERER_PAINTER
#define MBGL_RENDERER_PAINTER

#include <mbgl/map/tile_data.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/static_vertex_buffer.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/style/types.hpp>

#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/linejoin_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/text_shader.hpp>
#include <mbgl/shader/dot_shader.hpp>
#include <mbgl/shader/gaussian_shader.hpp>

#include <mbgl/map/transform_state.hpp>

#include <map>
#include <unordered_map>
#include <set>

namespace mbgl {

enum class RenderPass : bool { Opaque, Translucent };

class Transform;
class Style;
class Tile;
class GlyphAtlas;
class Source;
class StyleSource;

class FillBucket;
class LineBucket;
class SymbolBucket;
class RasterBucket;
class PrerenderedTexture;

struct FillProperties;
struct RasterProperties;

class LayerDescription;
class RasterTileData;

class Painter : private util::noncopyable {
public:
    Painter(Map &map);
    ~Painter();


    void setup();

    // Perform cleanup tasks that prepare shutting down the app. This doesn't mean that the
    // app will be shut down. That means all operations must be automatically be reversed (e.g. through
    // lazy initialization) in case rendering continues.
    void cleanup();


    // Renders the backdrop of the OpenGL view. This also paints in areas where we don't have any
    // tiles whatsoever.
    void clear();

    // Updates the default matrices to the current viewport dimensions.
    void changeMatrix();

    // Renders a particular layer from a tile.
    void renderTileLayer(const Tile& tile, std::shared_ptr<StyleLayer> layer_desc, const mat4 &matrix);

    // Renders debug information for a tile.
    void renderTileDebug(const Tile& tile);

    // Renders the red debug frame around a tile, visualizing its perimeter.
    void renderDebugFrame(const mat4 &matrix);

    void renderDebugText(DebugBucket& bucket, const mat4 &matrix);
    void renderDebugText(const std::vector<std::string> &strings);
    void renderFill(FillBucket& bucket, const FillProperties& properties, const Tile::ID& id, const mat4 &matrix);
    void renderFill(FillBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix);
    void renderLine(LineBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix);
    void renderSymbol(SymbolBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix);
    void renderRaster(RasterBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix);
    std::array<float, 3> spinWeights(float spin_value);

    void preparePrerender(RasterBucket &bucket);

    void renderPrerenderedTexture(RasterBucket &bucket, const mat4 &matrix, const RasterProperties& properties);

    void createPrerendered(RasterBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id);

    void resize();

    // Changes whether debug information is drawn onto the map
    void setDebug(bool enabled);

    // Opaque/Translucent pass setting
    void setOpaque();
    void setTranslucent();

    // Configures the painter strata that is used for early z-culling of fragments.
    void setStrata(float strata);

    void drawClippingMasks(const std::set<std::shared_ptr<StyleSource>> &sources);
    void drawClippingMask(const mat4& matrix, const ClipID& clip);

    void resetFramebuffer();
    void bindFramebuffer();
    void pushFramebuffer();
    GLuint popFramebuffer();
    void discardFramebuffers();

    bool needsAnimation() const;
private:
    void setupShaders();
    const mat4 &translatedMatrix(const mat4& matrix, const std::array<float, 2> &translation, const Tile::ID &id, TranslateAnchorType anchor = TranslateAnchorType::Map);

    void prepareTile(const Tile& tile);

public:
    void useProgram(uint32_t program);
    void lineWidth(float lineWidth);
    void depthMask(bool value);
    void depthRange(float near, float far);

public:
    mat4 vtxMatrix;
    mat4 projMatrix;
    mat4 nativeMatrix;
    mat4 extrudeMatrix;

    // used to composite images and flips the geometry upside down
    const mat4 flipMatrix = []{
        mat4 flipMatrix;
        matrix::ortho(flipMatrix, 0, 4096, -4096, 0, 0, 1);
        matrix::translate(flipMatrix, flipMatrix, 0, -4096, 0);
        return flipMatrix;
    }();

private:
    Map& map;

    FrameHistory frameHistory;

    bool debug = false;

    uint32_t gl_program = 0;
    float gl_lineWidth = 0;
    bool gl_depthMask = true;
    std::array<uint16_t, 2> gl_viewport = {{ 0, 0 }};
    std::array<float, 2> gl_depthRange = {{ 0, 1 }};
    float strata = 0;
    RenderPass pass = RenderPass::Opaque;
    const float strata_epsilon = 1.0f / (1 << 16);

public:
    std::unique_ptr<PlainShader> plainShader;
    std::unique_ptr<OutlineShader> outlineShader;
    std::unique_ptr<LineShader> lineShader;
    std::unique_ptr<LinejoinShader> linejoinShader;
    std::unique_ptr<PatternShader> patternShader;
    std::unique_ptr<IconShader> iconShader;
    std::unique_ptr<RasterShader> rasterShader;
    std::unique_ptr<TextShader> textShader;
    std::unique_ptr<DotShader> dotShader;
    std::unique_ptr<GaussianShader> gaussianShader;

    // Set up the stencil quad we're using to generate the stencil mask.
    StaticVertexBuffer tileStencilBuffer = {
        // top left triangle
        { 0, 0 },
        { 4096, 0 },
        { 0, 4096 },

        // bottom right triangle
        { 4096, 0 },
        { 0, 4096 },
        { 4096, 4096 },
    };

    VertexArrayObject coveringPlainArray;
    VertexArrayObject coveringPatternArray;
    VertexArrayObject coveringRasterArray;
    VertexArrayObject coveringGaussianArray;

    VertexArrayObject matteArray;

    // Set up the tile boundary lines we're using to draw the tile outlines.
    StaticVertexBuffer tileBorderBuffer = {
        { 0, 0 },
        { 4096, 0 },
        { 4096, 4096 },
        { 0, 4096 },
        { 0, 0 },
    };

    VertexArrayObject tileBorderArray;

    // Framebuffer management
    std::vector<GLuint> fbos;
    std::vector<GLuint> fbos_color;
    GLuint fbo_depth_stencil;
    int fbo_level = -1;
    bool fbo_depth_stencil_valid = false;

};

}

#endif
