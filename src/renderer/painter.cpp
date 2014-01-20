#include <llmr/renderer/painter.hpp>
#include <iostream>
#include <assert.h>
#include <llmr/util/mat4.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/map/tile.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/map/settings.hpp>

using namespace llmr;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLshort tile_stencil_vertices[] = {
    0, 0,
    4096, 0,
    0, 4096,
    4096, 4096
};

GLshort tile_border_vertices[] = {
    0, 0,
    4096, 0,
    4096, 4096,
    0, 4096,
    0, 0
};

Painter::Painter(Transform& transform, Settings& settings)
    : transform(transform),
      settings(settings),
      currentShader(NULL),
      fillShader(NULL),
      lineShader(NULL),
      outlineShader(NULL) {
}


void Painter::setup() {
    setupShaders();

    assert(fillShader);
    assert(lineShader);
    assert(outlineShader);

    // Set up the stencil quad we're using to generate the stencil mask.
    glGenBuffers(1, &tile_stencil_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tile_stencil_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tile_stencil_vertices), tile_stencil_vertices, GL_STATIC_DRAW);

    // Set up the tile boundary lines we're using to draw the tile outlines.
    glGenBuffers(1, &tile_border_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tile_border_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tile_border_vertices), tile_border_vertices, GL_STATIC_DRAW);


    glEnable(GL_STENCIL_TEST);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Painter::setupShaders() {
    fillShader = new FillShader();
    lineShader = new LineShader();
    outlineShader = new OutlineShader();
}

void Painter::teardown() {
    glDeleteBuffers(1, &tile_stencil_buffer);

    if (fillShader) {
        delete fillShader;
        fillShader = NULL;
    }

    if (lineShader) {
        delete lineShader;
        lineShader = NULL;
    }

}

void Painter::changeMatrix(const tile::ptr& tile) {
    // Initialize projection matrix
    float projMatrix[16];
    mat4::ortho(projMatrix, 0, transform.width, transform.height, 0, 1, 10);

    float mvMatrix[16];
    transform.matrixFor(mvMatrix, tile->id);

    mat4::multiply(matrix, projMatrix, mvMatrix);
}

void Painter::drawClippingMask() {
    switchShader(lineShader);
    glUniformMatrix4fv(lineShader->u_matrix, 1, GL_FALSE, matrix);

    glColorMask(false, false, false, false);

    // Clear the entire stencil buffer, except for the 7th bit, which stores
    // the global clipping mask that allows us to avoid drawing in regions of
    // tiles we've already painted in.
    glClearStencil(0x0);
    glStencilMask(0xBF);
    glClear(GL_STENCIL_BUFFER_BIT);

    // The stencil test will fail always, meaning we set all pixels covered
    // by this geometry to 0x80. We use the highest bit 0x80 to mark the regions
    // we want to draw in. All pixels that have this bit *not* set will never be
    // drawn in.
    glStencilFunc(GL_EQUAL, 0xC0, 0x40);
    glStencilMask(0xC0);
    glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);

    // Draw the clipping mask
    glBindBuffer(GL_ARRAY_BUFFER, tile_stencil_buffer);
    glVertexAttribPointer(lineShader->a_pos, 2, GL_SHORT, false, 0, BUFFER_OFFSET(0));
    glUniform4f(lineShader->u_color, 1.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(tile_stencil_vertices));

    // glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 0x80, 0x80);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0x00);
    glColorMask(true, true, true, true);
}

void Painter::clear() {
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClearStencil(0x0);
    glStencilMask(0xFF);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Painter::render(const tile::ptr& tile) {
    if (tile->state != tile::ready) {
        return;
    }

    changeMatrix(tile);

    drawClippingMask();

    switchShader(outlineShader);
    glUniformMatrix4fv(outlineShader->u_matrix, 1, GL_FALSE, matrix);

    glStencilFunc(GL_EQUAL, 0x80, 0x80);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // glDisable(GL_STENCIL_TEST);

    tile->lineVertex.bind();
    glVertexAttribPointer(outlineShader->a_pos, 2, GL_SHORT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glUniform4f(outlineShader->u_color, 0.0f, 0.0f, 0.0f, 1.0f);
    glUniform2f(outlineShader->u_world, transform.width, transform.height);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_STRIP, 0, tile->lineVertex.length());


    if (settings.debug) {
        renderDebug(tile);
    }
}

void Painter::renderDebug(const tile::ptr& tile) {
    // draw tile outline
    switchShader(lineShader);
    glUniformMatrix4fv(lineShader->u_matrix, 1, GL_FALSE, matrix);
    glBindBuffer(GL_ARRAY_BUFFER, tile_border_buffer);
    glVertexAttribPointer(lineShader->a_pos, 2, GL_SHORT, false, 0, BUFFER_OFFSET(0));
    glUniform4f(lineShader->u_color, 1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(4.0f);
    glDrawArrays(GL_LINE_STRIP, 0, sizeof(tile_border_vertices));

    // draw debug info
    switchShader(lineShader);
    glUniformMatrix4fv(lineShader->u_matrix, 1, GL_FALSE, matrix);
    tile->debugFontVertex.bind();
    glVertexAttribPointer(lineShader->a_pos, 2, GL_SHORT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glUniform4f(lineShader->u_color, 1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(4.0f);
    glDrawArrays(GL_LINES, 0, tile->debugFontVertex.length());
    glUniform4f(lineShader->u_color, 0.0f, 0.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, tile->debugFontVertex.length());
}

// Switches to a different shader program.
/**
 * @return boolean whether the shader was actually switched
 */
bool Painter::switchShader(Shader *shader) {
    if (currentShader != shader) {
        glUseProgram(shader->program);

        // Disable all attributes from the existing shader that aren't used in
        // the new shader. Note: attribute indices are *not* program specific!
        if (currentShader) {
            const std::forward_list<uint32_t> &hitherto = currentShader->attributes;
            const std::forward_list<uint32_t> &henceforth = shader->attributes;

            // Find all attribute indices that are used in the old shader,
            // but unused in the new one.
            for (uint32_t i : hitherto) {
                if (std::find(henceforth.begin(), henceforth.end(), i) == henceforth.end()) {
                    glDisableVertexAttribArray(i);
                }
            }

            // Enable all attributes for the new shader that were not already in
            // use in the old one.
            for (uint32_t i : henceforth) {
                if (std::find(hitherto.begin(), hitherto.end(), i) == hitherto.end()) {
                    glEnableVertexAttribArray(i);
                }
            }

            currentShader = shader;
        } else {
            // Enable all attributes for the new shader (== first shader).
            for (GLuint index : shader->attributes) {
                glEnableVertexAttribArray(index);
            }
        }

        currentShader = shader;

        return true;
    } else {
        return false;
    }
}
