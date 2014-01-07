#ifndef LLMR_RENDERER_PAINTER
#define LLMR_RENDERER_PAINTER

#include "../platform/gl.hpp"

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

    void resize(GLuint new_width, GLuint new_height);
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
    GLuint width, height;
    GLfloat matrix[16];

    Shader *currentShader;
    FillShader *fillShader;
    LineShader *lineShader;

    GLuint vertexArray;
    GLuint triangleVertexBuffer;
    GLuint fillVertexBuffer;
};

}

#endif
