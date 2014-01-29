#include <llmr/renderer/painter.hpp>
#include <iostream>
#include <assert.h>
#include <llmr/util/mat4.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/map/tile.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/map/settings.hpp>

#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/style/style.hpp>

#include <cmath>
#include <array>
#include <numeric>

using namespace llmr;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLshort tile_stencil_vertices[] = {
    // top left triangle
    0, 0,
    4096, 0,
    0, 4096,

    // bottom right triangle
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

GLshort matte_vertices[] = {
    // top left triangle
    0, 0,
    16384, 0,
    0, 16384,

    // bottom right triangle
    16384, 0,
    0, 16384,
    16384, 16384
};

Painter::Painter(Transform& transform, Settings& settings, Style& style)
    : transform(transform),
      settings(settings),
      style(style),
      currentShader(NULL) {
}


void Painter::setup() {
    setupShaders();

    assert(fillShader);
    assert(plainShader);
    assert(outlineShader);
    assert(lineShader);

    // Set up the stencil quad we're using to generate the stencil mask.
    glGenBuffers(1, &tile_stencil_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tile_stencil_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tile_stencil_vertices), tile_stencil_vertices, GL_STATIC_DRAW);

    // Set up the tile boundary lines we're using to draw the tile outlines.
    glGenBuffers(1, &tile_border_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tile_border_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tile_border_vertices), tile_border_vertices, GL_STATIC_DRAW);

    // Set up the tile boundary lines we're using to draw the tile outlines.
    glGenBuffers(1, &matte_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, matte_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(matte_vertices), matte_vertices, GL_STATIC_DRAW);


    glEnable(GL_STENCIL_TEST);

    // We are blending the new pixels *behind* the existing pixels. That way we can
    // draw front-to-back and use then stencil buffer to cull opaque pixels early.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
}

void Painter::setupShaders() {
    fillShader = std::make_shared<FillShader>();
    plainShader = std::make_shared<PlainShader>();
    outlineShader = std::make_shared<OutlineShader>();
    lineShader = std::make_shared<LineShader>();
}

void Painter::teardown() {
    glDeleteBuffers(1, &tile_stencil_buffer);
    glDeleteBuffers(1, &tile_border_buffer);
    glDeleteBuffers(1, &matte_buffer);
}

void Painter::changeMatrix(const Tile::Ptr& tile) {
    // Initialize projection matrix
    float projMatrix[16];
    mat4::ortho(projMatrix, 0, transform.width, transform.height, 0, 1, 10);

    // The position matrix.
    transform.matrixFor(matrix, tile->id);
    mat4::multiply(matrix, projMatrix, matrix);

    // The extrusion matrix.
    mat4::identity(exMatrix);
    mat4::multiply(exMatrix, projMatrix, exMatrix);
    mat4::rotate_z(exMatrix, exMatrix, transform.getAngle());

    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    mat4::identity(nativeMatrix);
    mat4::translate(nativeMatrix, nativeMatrix, 0, 0, -1);
    mat4::multiply(nativeMatrix, projMatrix, nativeMatrix);
}

void Painter::drawClippingMask() {
    switchShader(plainShader);
    glUniformMatrix4fv(plainShader->u_matrix, 1, GL_FALSE, matrix);

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
    glVertexAttribPointer(plainShader->a_pos, 2, GL_SHORT, false, 0, BUFFER_OFFSET(0));
    glUniform4f(plainShader->u_color, 1.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(tile_stencil_vertices));

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

    renderLayers(tile, style.layers);

    if (settings.debug) {
        renderDebug(tile);
    }

    renderBackground();
}

void Painter::renderLayers(const std::shared_ptr<Tile>& tile, const std::vector<LayerDescription>& layers) {
    // Render everything top-to-bottom by using reverse iterators
    typedef std::vector<LayerDescription>::const_reverse_iterator iterator;
    for (iterator it = layers.rbegin(), end = layers.rend(); it != end; it++) {
        const LayerDescription& layer_desc = *it;

        if (layer_desc.child_layer.size()) {
            // This is a layer group.
            // TODO: create framebuffer
            renderLayers(tile, layer_desc.child_layer);
            // TODO: render framebuffer on previous framebuffer
        } else {
            // This is a singular layer. Try to find the bucket associated with
            // this layer and render it.
            auto bucket_it = tile->buckets.find(layer_desc.bucket_name);
            if (bucket_it != tile->buckets.end()) {
                assert(bucket_it->second);
                bucket_it->second->render(*this, layer_desc.name);
            }
        }
    }
}

void Painter::renderFill(FillBucket& bucket, const std::string& layer_name) {
    const FillProperties& properties = style.computed.fills[layer_name];

    // Abort early.
    if (!bucket.size()) return;
    if (properties.hidden) return;

    Color fill_color = properties.fill_color;
    fill_color[0] *= properties.opacity;
    fill_color[1] *= properties.opacity;
    fill_color[2] *= properties.opacity;
    fill_color[3] *= properties.opacity;

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

        if (properties.winding == Winding::EvenOdd) {
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
        bucket.drawElements(fillShader->a_pos);

        // Now that we have the stencil mask in the stencil buffer, we can start
        // writing to the color buffer.
        glColorMask(true, true, true, true);
    }

    // From now on, we don't want to update the stencil buffer anymore.
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0x0);

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (properties.antialias) {
        switchShader(outlineShader);
        glUniformMatrix4fv(outlineShader->u_matrix, 1, GL_FALSE, matrix);
        glLineWidth(2);

        if (properties.stroke_color != properties.fill_color) {
            // If we defined a different color for the fill outline, we are
            // going to ignore the bits in 0x3F and just care about the global
            // clipping mask.
            glStencilFunc(GL_EQUAL, 0x80, 0x80);
            glUniform4fv(outlineShader->u_color, 1, properties.stroke_color.data());
        } else {
            // Otherwise, we only want to draw the antialiased parts that are
            // *outside* the current shape. This is important in case the fill
            // or stroke color is translucent. If we wouldn't clip to outside
            // the current shape, some pixels from the outline stroke overlapped
            // the (non-antialiased) fill.
            glStencilFunc(GL_EQUAL, 0x80, 0xBF);
            glUniform4fv(outlineShader->u_color, 1, fill_color.data());
        }

        // Draw the entire line
        glUniform2f(outlineShader->u_world, transform.fb_width, transform.fb_height);
        glLineWidth(2.0f);
        bucket.drawVertices(outlineShader->a_pos);
    }

    // var imagePos = layerStyle.image && imageSprite.getPosition(layerStyle.image, true);
    bool imagePos = false;
    if (imagePos) {
        // // Draw texture fill

        // var factor = 8 / Math.pow(2, painter.transform.zoom - params.z);
        // var mix = painter.transform.z % 1.0;
        // var imageSize = [imagePos.size[0] * factor, imagePos.size[1] * factor];

        // var offset = [
        //     (params.x * 4096) % imageSize[0],
        //     (params.y * 4096) % imageSize[1]
        // ];

        // glSwitchShader(painter.patternShader, painter.posMatrix, painter.exMatrix);
        // glUniform1i(painter.patternShader.u_image, 0);
        // glUniform2fv(painter.patternShader.u_pattern_size, imageSize);
        // glUniform2fv(painter.patternShader.u_offset, offset);
        // glUniform2fv(painter.patternShader.u_rotate, [1, 1]);
        // glUniform2fv(painter.patternShader.u_pattern_tl, imagePos.tl);
        // glUniform2fv(painter.patternShader.u_pattern_br, imagePos.br);
        // glUniform4fv(painter.patternShader.u_color, color);
        // glUniform1f(painter.patternShader.u_mix, mix);
        // imageSprite.bind(gl, true);
    } else {
        // Draw filling rectangle.
        switchShader(fillShader);
        glUniformMatrix4fv(fillShader->u_matrix, 1, GL_FALSE, matrix);
        glUniform4fv(fillShader->u_color, 1, fill_color.data());
    }

    // Only draw regions that we marked
    glStencilFunc(GL_NOTEQUAL, 0x0, 0x3F);

    // Draw a rectangle that covers the entire viewport.
    glBindBuffer(GL_ARRAY_BUFFER, tile_stencil_buffer);
    glVertexAttribPointer(fillShader->a_pos, 2, GL_SHORT, false, 0, BUFFER_OFFSET(0));
    glDrawArrays(GL_TRIANGLES, 0, sizeof(tile_stencil_vertices));

    glStencilMask(0x00);
    glStencilFunc(GL_EQUAL, 0x80, 0x80);
}

void Painter::renderLine(LineBucket& bucket, const std::string& layer_name) {
    const LineProperties& properties = style.computed.lines[layer_name];

    // Abort early.
    if (!bucket.size()) return;
    if (properties.hidden) return;

    double width = properties.width;
    double offset = (properties.offset || 0) / 2;
    double inset = fmax(-1, offset - width / 2 - 0.5) + 1;
    double outset = offset + width / 2 + 0.5;

    // var imagePos = properties.image && imageSprite.getPosition(properties.image);
    // var shader;
    bool imagePos = false;

    if (imagePos) {
        // var factor = 8 / Math.pow(2, painter.transform.zoom - params.z);

        // imageSprite.bind(gl, true);

        // //factor = Math.pow(2, 4 - painter.transform.zoom + params.z);
        // gl.switchShader(painter.linepatternShader, painter.translatedMatrix || painter.posMatrix, painter.exMatrix);
        // shader = painter.linepatternShader;
        // glUniform2fv(painter.linepatternShader.u_pattern_size, [imagePos.size[0] * factor, imagePos.size[1] ]);
        // glUniform2fv(painter.linepatternShader.u_pattern_tl, imagePos.tl);
        // glUniform2fv(painter.linepatternShader.u_pattern_br, imagePos.br);
        // glUniform1f(painter.linepatternShader.u_fade, painter.transform.z % 1.0);

    } else {
        switchShader(lineShader);
        glUniformMatrix4fv(lineShader->u_matrix, 1, GL_FALSE, matrix);
        glUniformMatrix4fv(lineShader->u_exmatrix, 1, GL_FALSE, exMatrix);
        // glUniform2fv(painter.lineShader.u_dasharray, properties.dasharray || [1, -1]);
        glUniform2f(lineShader->u_dasharray, 1, -1);
    }


    // TODO: Move this to transform?
    const double tilePixelRatio = transform.getScale() / (1 << transform.getIntegerZoom()) / 8;

    glUniform2f(lineShader->u_linewidth, outset, inset);
    glUniform1f(lineShader->u_ratio, tilePixelRatio);
    glUniform1f(lineShader->u_gamma, transform.pixelRatio);

    // const Color& color = properties.color;
    // if (!params.antialiasing) {
    //     color[3] = Infinity;
    //     glUniform4fv(lineShader->u_color, color);
    // } else {
    glUniform4fv(lineShader->u_color, 1, properties.color.data());
    // }


    glUniform1f(lineShader->u_point, 0);

    bucket.bind();
    char *vertex_index = bucket.vertexOffset();
    glVertexAttribPointer(lineShader->a_pos, 4, GL_SHORT, false, 8, vertex_index);
    glVertexAttribPointer(lineShader->a_extrude, 2, GL_BYTE, false, 8, vertex_index + 6);
    glVertexAttribPointer(lineShader->a_linesofar, 2, GL_SHORT, false, 8, vertex_index + 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, bucket.size());

    if (bucket.geometry.join == JoinType::Round) {
        glUniform1f(lineShader->u_point, 1);
        glDrawArrays(GL_POINTS, 0, bucket.size());
    }

    // statistics
    // stats.lines += count;
}

void Painter::renderDebug(const Tile::Ptr& tile) {
    // Blend to the front, not the back.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // draw tile outline
    switchShader(plainShader);
    glUniformMatrix4fv(plainShader->u_matrix, 1, GL_FALSE, matrix);
    glBindBuffer(GL_ARRAY_BUFFER, tile_border_buffer);
    glVertexAttribPointer(plainShader->a_pos, 2, GL_SHORT, false, 0, BUFFER_OFFSET(0));
    glUniform4f(plainShader->u_color, 1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(4.0f);
    glDrawArrays(GL_LINE_STRIP, 0, sizeof(tile_border_vertices));

    // draw debug info
    switchShader(plainShader);
    glUniformMatrix4fv(plainShader->u_matrix, 1, GL_FALSE, matrix);
    tile->debugFontBuffer->bind();
    glVertexAttribPointer(plainShader->a_pos, 2, GL_SHORT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glUniform4f(plainShader->u_color, 1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(4.0f);
    glDrawArrays(GL_LINES, 0, tile->debugFontBuffer->length());
    glUniform4f(plainShader->u_color, 0.0f, 0.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, tile->debugFontBuffer->length());

    // Revert blending mode to blend to the back.
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
}

void Painter::renderMatte() {
    glDisable(GL_STENCIL_TEST);

    switchShader(fillShader);
    glUniformMatrix4fv(fillShader->u_matrix, 1, GL_FALSE, nativeMatrix);

    Color white = {{ 1, 1, 1, 1 }};

    // Draw the clipping mask
    glBindBuffer(GL_ARRAY_BUFFER, matte_buffer);
    glVertexAttribPointer(fillShader->a_pos, 2, GL_SHORT, false, 0, BUFFER_OFFSET(0));
    glUniform4fv(fillShader->u_color, 1, white.data());
    glDrawArrays(GL_TRIANGLES, 0, sizeof(matte_vertices));

    glEnable(GL_STENCIL_TEST);
}

void Painter::renderBackground() {
    switchShader(fillShader);
    glUniformMatrix4fv(fillShader->u_matrix, 1, GL_FALSE, matrix);

    Color white = {{ 1, 1, 1, 1 }};

    // Draw the clipping mask
    glBindBuffer(GL_ARRAY_BUFFER, tile_stencil_buffer);
    glVertexAttribPointer(fillShader->a_pos, 2, GL_SHORT, false, 0, BUFFER_OFFSET(0));
    glUniform4fv(fillShader->u_color, 1, white.data());
    glDrawArrays(GL_TRIANGLES, 0, sizeof(tile_stencil_vertices));
}


// Switches to a different shader program.
/**
 * @return boolean whether the shader was actually switched
 */
bool Painter::switchShader(std::shared_ptr<Shader> shader) {
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
