#ifndef LLMR_RENDERER_PAINTER
#define LLMR_RENDERER_PAINTER

#include <llmr/map/tile.hpp>
#include <llmr/geometry/vertex_buffer.hpp>
#include <llmr/util/mat4.hpp>

namespace llmr {

class Transform;
class Settings;
class Style;
class Tile;
// class LayerDescription;

class Shader;
class PlainShader;
class OutlineShader;
class FillShader;
class LineShader;
class PatternShader;

class FillBucket;
class LineBucket;

class Painter {
public:
    Painter(Transform& transform, Settings& settings, Style& style);

    // Make noncopyable
    Painter(const Painter&) = delete;
    Painter(const Painter&&) = delete;
    Painter& operator=(const Painter&) = delete;
    Painter& operator=(const Painter && ) = delete;


    void setup();


    void clear();
    void changeMatrix(const Tile::ID& id);
    void render(const std::shared_ptr<Tile>& tile);
    void renderLayers(const std::shared_ptr<Tile>& tile, const std::vector<LayerDescription>& layers);
    void renderDebug(const std::shared_ptr<Tile>& tile);

    void renderMatte();
    void renderBackground();
    void renderFill(FillBucket& bucket, const std::string& layer_name, const Tile::ID& id);
    void renderLine(LineBucket& bucket, const std::string& layer_name, const Tile::ID& id);


    void drawClippingMask();
    bool switchShader(std::shared_ptr<Shader> shader);

private:
    void setupShaders();

public:

private:
    Transform& transform;
    Settings& settings;
    Style& style;

    mat4 nativeMatrix;
    mat4 matrix;
    mat4 exMatrix;

    std::shared_ptr<Shader> currentShader;
    std::shared_ptr<FillShader> fillShader;
    std::shared_ptr<PlainShader> plainShader;
    std::shared_ptr<OutlineShader> outlineShader;
    std::shared_ptr<LineShader> lineShader;
    std::shared_ptr<PatternShader> patternShader;

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

    // Set up the tile boundary lines we're using to draw the tile outlines.
    VertexBuffer tileBorderBuffer = {
        0, 0,
        4096, 0,
        4096, 4096,
        0, 4096,
        0, 0
    };

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

};

}

#endif
