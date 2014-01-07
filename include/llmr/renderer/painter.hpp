#ifndef LLMR_RENDERER_PAINTER
#define LLMR_RENDERER_PAINTER

#include "shader-fill.hpp"
#include "shader-line.hpp"

namespace llmr {

class platform;
class transform;
class tile;

class painter {
public:
    painter(class platform *platform);

    void setup();
    void teardown();

    void resize(uint32_t new_width, uint32_t new_height);
    void viewport();


    void clear();
    void render(tile *tile);

    void switchShader(Shader *shader, float matrix[16]);
    void setTransform(transform *transform);

private:
    void setupShaders();
    void changeMatrix();

public:

private:
    platform *platform;
    transform *transform;
    uint32_t width, height;
    float matrix[16];

    Shader *currentShader;
    FillShader *fillShader;
    LineShader *lineShader;

    uint32_t vertexArray;
    uint32_t triangleVertexBuffer;
    uint32_t fillVertexBuffer;
};

}

#endif
