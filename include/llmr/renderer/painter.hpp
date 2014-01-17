#ifndef LLMR_RENDERER_PAINTER
#define LLMR_RENDERER_PAINTER

#include "shader-fill.hpp"
#include "shader-line.hpp"
#include "shader-outline.hpp"

namespace llmr {

class transform;
class tile;

class painter {
public:
    painter(class transform *transform, class settings *settings);

    void setup();
    void teardown();

    void clear();
    void render(const std::shared_ptr<tile>& tile);
    void renderDebug(const std::shared_ptr<tile>& tile);

    void drawClippingMask();
    bool switchShader(Shader *shader);

private:
    void setupShaders();
    void changeMatrix(const std::shared_ptr<tile>& tile);

public:

private:
    transform *transform;
    settings *settings;
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
