#include <llmr/renderer/painter.hpp>
#include <iostream>
#include <assert.h>
#include <llmr/util/mat4.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/map/tile.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/map/settings.hpp>

#include <cmath>

#include <numeric>

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

    // We are blending the new pixels *behind* the existing pixels. That way we can
    // draw front-to-back and use then stencil buffer to cull opaque pixels early.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
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

void Painter::changeMatrix(const Tile::Ptr& tile) {
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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearStencil(0x0);
    glStencilMask(0xFF);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Painter::render(const Tile::Ptr& tile) {
    if (tile->state != Tile::ready) {
        return;
    }

    changeMatrix(tile);

    drawClippingMask();

    switchShader(outlineShader);
    glUniformMatrix4fv(outlineShader->u_matrix, 1, GL_FALSE, matrix);

    glStencilFunc(GL_EQUAL, 0x80, 0x80);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // glDisable(GL_STENCIL_TEST);

    // draw lines:
    tile->lineVertex.bind();
    glVertexAttribPointer(outlineShader->a_pos, 2, GL_SHORT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glUniform4f(outlineShader->u_color, 0.0f, 0.0f, 0.0f, 1.0f);
    glUniform2f(outlineShader->u_world, transform.fb_width, transform.fb_height);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_STRIP, 0, tile->lineVertex.length());

    // draw fills:

    // TODO: expose this to the stylesheet

    for (const Tile::fill_index& index : tile->fillIndices) {
        renderFill(tile, index);
    }

    if (settings.debug) {
        renderDebug(tile);
    }

    renderBackground();
}

void Painter::renderFill(const std::shared_ptr<Tile>& tile, const Tile::fill_index& index) {
    float color[4] = { 1.0, 0.0, 0.0, 1.0 };
    float alpha = color[3];

    // TODO: expose this to the stylesheet.
    bool evenodd = false;
    bool background = false;
    bool antialiasing = true;
    bool stroke = false;

    if (!background) {
        // Draw the stencil mask.
        {
            // We're only drawing to the first seven bits (== support a maximum of
            // 127 overlapping polygons in one place before we get rendering errors).
            glStencilMask(0x3F);
            glClear(GL_STENCIL_BUFFER_BIT);

            // Draw front facing triangles. Wherever the 0x80 bit is 1, we are
            // increasing the lower 7 bits by one if the triangle is a front-facing
            // triangle. This means that all visible polygons should be in CCW
            // orientation, while all holes (see below) are in CW orientation.
            glStencilFunc(GL_NOTEQUAL, 0x80, 0x80);

            if (evenodd) {
                // When we draw an even/odd winding fill, we just invert all the bits.
                glStencilOp(GL_INVERT, GL_KEEP, GL_KEEP);
            } else {
                // When we do a nonzero fill, we count the number of times a pixel is
                // covered by a counterclockwise polygon, and subtract the number of
                // times it is "uncovered" by a clockwise polygon.
                glStencilOpSeparate(GL_FRONT, GL_INCR_WRAP, GL_KEEP, GL_KEEP);
                glStencilOpSeparate(GL_BACK, GL_DECR_WRAP, GL_KEEP, GL_KEEP);
            }

            // When drawing a shape, we first draw all shapes to the stencil buffer
            // and incrementing all areas where polygons are
            glColorMask(false, false, false, false);

            // Draw the actual triangle fan into the stencil buffer.
            switchShader(fillShader);
            glUniformMatrix4fv(fillShader->u_matrix, 1, GL_FALSE, matrix);

            // Draw all groups
            char *vertex_index = BUFFER_OFFSET(index.vertex_start * 2 * sizeof(uint16_t));
            char *elements_index = BUFFER_OFFSET(index.elements_start * 3 * sizeof(uint16_t));
            tile->fillBuffer.bind();
            for (const Tile::fill_index::group& group : index.groups) {
                glVertexAttribPointer(fillShader->a_pos, 2, GL_SHORT, GL_FALSE, 0, vertex_index);
                glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
                vertex_index += group.vertex_length * 2 * sizeof(uint16_t);
                elements_index += group.elements_length * 3 * sizeof(uint16_t);
            }

            // Now that we have the stencil mask in the stencil buffer, we can start
            // writing to the color buffer.
            glColorMask(true, true, true, true);
        }

        // From now on, we don't want to update the stencil buffer anymore.
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilMask(0x0);

        // Because we're drawing top-to-bottom, and we update the stencil mask
        // below, we have to draw the outline first (!)
        if (antialiasing) {
            switchShader(outlineShader);
            glUniformMatrix4fv(outlineShader->u_matrix, 1, GL_FALSE, matrix);
            glLineWidth(2);

            if (stroke) {
                // If we defined a different color for the fill outline, we are
                // going to ignore the bits in 0x3F and just care about the global
                // clipping mask.
                glStencilFunc(GL_EQUAL, 0x80, 0x80);
            } else {
                // Otherwise, we only want to draw the antialiased parts that are
                // *outside* the current shape. This is important in case the fill
                // or stroke color is translucent. If we wouldn't clip to outside
                // the current shape, some pixels from the outline stroke overlapped
                // the (non-antialiased) fill.
                glStencilFunc(GL_EQUAL, 0x80, 0xBF);
            }

            glUniform2f(outlineShader->u_world, transform.fb_width, transform.fb_height);
            glUniform4f(outlineShader->u_color, color[0], color[1], color[2], color[3]);

            // Draw the entire line
            char *vertex_index = BUFFER_OFFSET(index.vertex_start * 2 * sizeof(uint16_t));
            glVertexAttribPointer(outlineShader->a_pos, 2, GL_SHORT, GL_FALSE, 0, vertex_index);
            glLineWidth(2.0f);
            glDrawArrays(GL_LINE_STRIP, 0, index.length);
        }
    }


    // var imagePos = layerStyle.image && imageSprite.getPosition(layerStyle.image, true);

    // if (imagePos) {
    //     // Draw texture fill

    //     var factor = 8 / Math.pow(2, painter.transform.zoom - params.z);
    //     var mix = painter.transform.z % 1.0;
    //     var imageSize = [imagePos.size[0] * factor, imagePos.size[1] * factor];

    //     var offset = [
    //         (params.x * 4096) % imageSize[0],
    //         (params.y * 4096) % imageSize[1]
    //     ];

    //     glSwitchShader(painter.patternShader, painter.posMatrix, painter.exMatrix);
    //     glUniform1i(painter.patternShader.u_image, 0);
    //     glUniform2fv(painter.patternShader.u_pattern_size, imageSize);
    //     glUniform2fv(painter.patternShader.u_offset, offset);
    //     glUniform2fv(painter.patternShader.u_rotate, [1, 1]);
    //     glUniform2fv(painter.patternShader.u_pattern_tl, imagePos.tl);
    //     glUniform2fv(painter.patternShader.u_pattern_br, imagePos.br);
    //     glUniform4fv(painter.patternShader.u_color, color);
    //     glUniform1f(painter.patternShader.u_mix, mix);
    //     imageSprite.bind(gl, true);

    // } else {
        // Draw filling rectangle.
        switchShader(fillShader);
        glUniformMatrix4fv(fillShader->u_matrix, 1, GL_FALSE, matrix);
        glUniform4f(fillShader->u_color, color[0], color[1], color[2], color[3]);
    // }

    if (background) {
        glStencilFunc(GL_EQUAL, 0x80, 0x80);
    } else {
        // Only draw regions that we marked
        glStencilFunc(GL_NOTEQUAL, 0x0, 0x3F);
    }

    // Draw a rectangle that covers the entire viewport.
    glBindBuffer(GL_ARRAY_BUFFER, tile_stencil_buffer);
    glVertexAttribPointer(fillShader->a_pos, 2, GL_SHORT, false, 0, BUFFER_OFFSET(0));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(tile_stencil_vertices));

    glStencilMask(0x00);
    glStencilFunc(GL_EQUAL, 0x80, 0x80);
}

void Painter::renderDebug(const Tile::Ptr& tile) {
    // Blend to the front, not the back.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

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

    // Revert blending mode to blend to the back.
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
}

void Painter::renderBackground() {
    switchShader(fillShader);
    glUniformMatrix4fv(fillShader->u_matrix, 1, GL_FALSE, matrix);

    // Draw the clipping mask
    glBindBuffer(GL_ARRAY_BUFFER, tile_stencil_buffer);
    glVertexAttribPointer(fillShader->a_pos, 2, GL_SHORT, false, 0, BUFFER_OFFSET(0));
    glUniform4f(fillShader->u_color, 1.0f, 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(tile_stencil_vertices));
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
            const std::forward_list<uint32_t>& hitherto = currentShader->attributes;
            const std::forward_list<uint32_t>& henceforth = shader->attributes;

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
