#ifndef LLMR_RENDERER_PAINTER
#define LLMR_RENDERER_PAINTER

#include <llmr/map/tile.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/geometry/vertex_buffer.hpp>
#include <llmr/util/mat4.hpp>
#include <llmr/util/noncopyable.hpp>

#include <llmr/renderer/shader-plain.hpp>
#include <llmr/renderer/shader-outline.hpp>
#include <llmr/renderer/shader-pattern.hpp>
#include <llmr/renderer/shader-line.hpp>
#include <llmr/renderer/shader-linejoin.hpp>


namespace llmr {

class Transform;
class Settings;
class Style;
class Tile;

class FillBucket;
class LineBucket;

class Painter : private util::noncopyable {
public:
    Painter(Transform& transform, Settings& settings, Style& style);

    void setup();
    void clear();
    void changeMatrix(const Tile::ID& id);
    void render(const std::shared_ptr<Tile>& tile);
    void renderMatte();
    void renderBackground();
    void renderFill(FillBucket& bucket, const std::string& layer_name, const Tile::ID& id);
    void renderLine(LineBucket& bucket, const std::string& layer_name, const Tile::ID& id);

private:
    void setupShaders();
    void drawClippingMask();
    void renderLayers(const std::shared_ptr<Tile>& tile, const std::vector<LayerDescription>& layers);
    void renderDebug(const std::shared_ptr<Tile>& tile);

private:
    Transform& transform;
    Settings& settings;
    Style& style;

    mat4 nativeMatrix;
    mat4 matrix;
    mat4 exMatrix;

    std::unique_ptr<PlainShader> plainShader;
    std::unique_ptr<OutlineShader> outlineShader;
    std::unique_ptr<LineShader> lineShader;
    std::unique_ptr<LinejoinShader> linejoinShader;
    std::unique_ptr<PatternShader> patternShader;

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

    VertexArrayObject<PlainShader> coveringPlainArray;
    VertexArrayObject<PatternShader> coveringPatternArray;

    // Set up the tile boundary lines we're using to draw the tile outlines.
    VertexBuffer tileBorderBuffer = {
        0, 0,
        4096, 0,
        4096, 4096,
        0, 4096,
        0, 0
    };

    VertexArrayObject<PlainShader> tileBorderArray;

    // Set up the matte buffer we're using to draw the filling background.
    VertexBuffer matteBuffer = {
        // top left triangle
        0, 0,
        16384, 0,
        0, 16384,

        // bottom right triangle
        16384, 0,
        0, 16384,
        16384, 16384
    };

    VertexArrayObject<PlainShader> matteArray;
};

}

#endif
