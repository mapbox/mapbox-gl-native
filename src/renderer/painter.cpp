#include <llmr/renderer/painter.hpp>
#include <iostream>
#include <assert.h>
#include <llmr/util/mat4.h>
#include <llmr/platform/platform.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/map/tile.hpp>
#include <llmr/platform/gl.hpp>

using namespace llmr;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLfloat triangle_vertices[] = {
    100, 100,
    100, 300,
    150, 150
};

GLfloat fill_vertices[] = {
    0, 0,
    300, 0,
    0, 300,
    300, 300
};


painter::painter(class platform *platform)
    : platform(platform),
      transform(NULL),
      width(0),
      height(0),
      currentShader(NULL),
      fillShader(NULL) {

}

void painter::setTransform(class transform *transform) {
    this->transform = transform;
}


void painter::setup() {
    setupShaders();


    assert(fillShader);
    assert(lineShader);



    glGenBuffers(1, &triangleVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &fillVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, fillVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fill_vertices), fill_vertices, GL_STATIC_DRAW);


    // glGenVertexArraysOES(1, &vertexArray);
    // glBindVertexArrayOES(vertexArray);
    // glBindVertexArrayOES(0);
}

void painter::setupShaders() {
    fillShader = new FillShader();
    lineShader = new LineShader();
}

void painter::teardown() {
    glDeleteBuffers(1, &triangleVertexBuffer);
    glDeleteBuffers(1, &fillVertexBuffer);

    // glDeleteVertexArraysOES(1, &vertexArray);

    if (fillShader) {
        delete fillShader;
        fillShader = NULL;
    }

    if (lineShader) {
        delete lineShader;
        lineShader = NULL;
    }

}

void painter::resize(GLuint new_width, GLuint new_height) {
    if (width == new_width && height == new_height) {
        return;
    }

    fprintf(stderr, "changing viewport size to %d/%d\n", new_width, new_height);
    width = new_width;
    height = new_height;
}

void painter::changeMatrix() {
    assert(transform);
    assert(width);
    assert(height);

    // Initialize projection matrix
    float projMatrix[16];
    mat4_ortho(projMatrix, 0, width, height, 0, 1, 10);

    float mvMatrix[16];
    mat4_identity(mvMatrix);
    mat4_translate(mvMatrix, mvMatrix, transform->x, transform->y, -1);
    mat4_scale(mvMatrix, mvMatrix, transform->scale / 8, transform->scale / 8, 1);

    mat4_multiply(matrix, projMatrix, mvMatrix);
}

void painter::clear() {
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void painter::render(tile *tile) {
    changeMatrix();

    glDisable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //    fprintf(stderr, "render tile\n");
    switchShader(lineShader, matrix);
    glUniformMatrix4fv(lineShader->u_matrix, 1, GL_FALSE, matrix);

    //    glEnable(GL_BLEND);

    // glVertexAttribPointer(fillShader->a_pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    // glVertexAttribPointer(fillShader->a_pos, 2, GL_SHORT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //    glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBuffer);
    tile->lineVertex.bind();
    glVertexAttribPointer(lineShader->a_pos, 2, GL_SHORT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glUniform4f(lineShader->u_color, 0.0f, 0.0f, 0.0f, 1.0f);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINE_STRIP, 0, tile->lineVertex.length());

    //    switchShader(fillShader, matrix);
    //    glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBuffer);
    //    glVertexAttribPointer(fillShader->a_pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //    glUniform4f(fillShader->u_color, 0.0f, 1.0f, 0.0f, 1.0f);
    //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);




    // glBindVertexArrayOES(vertexArray);
    // switchShader(fillShader, matrix);
    // glUniformMatrix4fv(fillShader->u_matrix, 1, GL_FALSE, matrix);

    // glEnable(GL_STENCIL_TEST);

    // Draw stencil mask.
    // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
    // glStencilFunc(GL_EQUAL, 0xFF, 0xFF);
    // glStencilMask(0xFF);

    // glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBuffer);
    // glVertexAttribPointer(fillShader->a_pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    // glUniform4f(fillShader->u_color, 0.0f, 1.0f, 0.0f, 1.0f);
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);


    // // Draw covering fill.
    // glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // glStencilFunc(GL_EQUAL, 0xFF, 0xFF);

    // glBindBuffer(GL_ARRAY_BUFFER, fillVertexBuffer);
    // glVertexAttribPointer(fillShader->a_pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    // glUniform4f(fillShader->u_color, 1.0f, 0.0f, 0.0f, 1.0f);
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void painter::viewport() {

}

// Switches to a different shader program.
void painter::switchShader(Shader *shader, float matrix[16]) {
    if (currentShader != shader) {
        glUseProgram(shader->program);

        // Disable all attributes from the existing shader that aren't used in
        // the new shader. Note: attribute indices are *not* program specific!
        if (currentShader) {
            // const std::vector<GLuint> &enabled = currentShader->attributes;
            // const std::vector<GLuint> &required = shader->attributes;
            // TODO

            // for (var i = 0; i < enabled.length; i++) {
            //     if (required.indexOf(enabled[i]) < 0) {
            //         gl.disableVertexAttribArray(enabled[i]);
            //     }
            // }

            // // Enable all attributes for the new shader.
            // for (var j = 0; j < required.length; j++) {
            //     if (enabled.indexOf(required[j]) < 0) {
            //         gl.enableVertexAttribArray(required[j]);
            //     }
            // }
        } else {
            // Enable all attributes for the new shader.
            for (GLuint index : shader->attributes) {
                glEnableVertexAttribArray(index);
            }
        }
    }

    currentShader = shader;
}
