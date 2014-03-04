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
#include <llmr/renderer/shader-point.hpp>


namespace llmr {

class Transform;
class Settings;
class Style;
class Tile;

class FillBucket;
class LineBucket;
class PointBucket;

class Painter : private util::noncopyable {
public:
    Painter(Transform& transform, Settings& settings, Style& style);

    void setup();
    void clear();
    void changeMatrix();
    void render(const std::shared_ptr<Tile>& tile);
    void renderMatte();
    void renderBackground();
    void renderFill(FillBucket& bucket, const std::string& layer_name, const Tile::ID& id);
    void renderLine(LineBucket& bucket, const std::string& layer_name, const Tile::ID& id);
    void renderPoint(PointBucket& bucket, const std::string& layer_name, const Tile::ID& id);

    void prepareClippingMask();
    void drawClippingMask(const mat4& matrix, uint8_t clip_id);
    void finishClippingMask();
private:
    void setupShaders();
    void renderLayers(const std::shared_ptr<Tile>& tile, const std::vector<LayerDescription>& layers);
    void renderDebug(const std::shared_ptr<Tile>& tile);

    void useProgram(uint32_t program);
    void lineWidth(float lineWidth);
    void depthMask(bool value);

public:
    mat4 matrix;
    mat4 projMatrix;
    mat4 nativeMatrix;
    mat4 extrudeMatrix;

private:
    Transform& transform;
    Settings& settings;
    Style& style;

    uint32_t gl_program = 0;
    float gl_lineWidth = 0;
    bool gl_depthMask = true;

    std::unique_ptr<PlainShader> plainShader;
    std::unique_ptr<OutlineShader> outlineShader;
    std::unique_ptr<LineShader> lineShader;
    std::unique_ptr<LinejoinShader> linejoinShader;
    std::unique_ptr<PatternShader> patternShader;
    std::unique_ptr<PointShader> pointShader;

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

    // Set up the tile boundary lines we're using to draw the tile outlines.
    VertexBuffer tileBorderBuffer = {
        0, 0,
        4096, 0,
        4096, 4096,
        0, 4096,
        0, 0
    };

    VertexArrayObject tileBorderArray;

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

    VertexArrayObject matteArray;
};

}

#endif
