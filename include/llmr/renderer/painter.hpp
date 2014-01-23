#ifndef LLMR_RENDERER_PAINTER
#define LLMR_RENDERER_PAINTER

#include "shader-fill.hpp"
#include "shader-line.hpp"
#include "shader-outline.hpp"

#include "../map/tile.hpp"

namespace llmr {

class Settings;
class Transform;

class Painter {
public:
    Painter(Transform& transform, Settings& settings);

    // Make noncopyable
    Painter(const Painter&) = delete;
    Painter(const Painter&&) = delete;
    Painter &operator=(const Painter&) = delete;
    Painter &operator=(const Painter&&) = delete;


    void setup();
    void teardown();

    void clear();
    void render(const std::shared_ptr<Tile>& tile);
    void renderDebug(const std::shared_ptr<Tile>& tile);

    void renderBackground();
    void renderFill(const std::shared_ptr<Tile>& tile, const Tile::fill_index& index);


    void drawClippingMask();
    bool switchShader(Shader *shader);

private:
    void setupShaders();
    void changeMatrix(const std::shared_ptr<Tile>& tile);

public:

private:
    Transform& transform;
    Settings& settings;
    float matrix[16];

    Shader *currentShader;
    FillShader *fillShader;
    LineShader *lineShader;
    OutlineShader *outlineShader;

    uint32_t tile_stencil_buffer;
    uint32_t tile_border_buffer;
};

}

#endif
