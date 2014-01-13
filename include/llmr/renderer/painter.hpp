#ifndef LLMR_RENDERER_PAINTER
#define LLMR_RENDERER_PAINTER

#include "shader-fill.hpp"
#include "shader-line.hpp"

namespace llmr {

class transform;
class tile;

class painter {
public:
    painter(class transform *tranform);

    void setup();
    void teardown();
    void viewport();


    void clear();
    void render(tile *tile);

    void switchShader(Shader *shader, float matrix[16]);

private:
    void setupShaders();
    void changeMatrix(tile *tile);

public:

private:
    transform *transform;
    float matrix[16];

    Shader *currentShader;
    FillShader *fillShader;
    LineShader *lineShader;

    // uint32_t vertexArray;
    uint32_t triangleVertexBuffer;
    uint32_t fillVertexBuffer;
};

}

#endif
