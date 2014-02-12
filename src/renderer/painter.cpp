#include <cassert>
#include <algorithm>


#include <llmr/renderer/painter.hpp>
#include <llmr/util/std.hpp>

#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>

#include <llmr/map/transform.hpp>
#include <llmr/map/settings.hpp>
#include <llmr/geometry/debug_font_buffer.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/style/style.hpp>
#include <llmr/style/sprite.hpp>

using namespace llmr;

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

Painter::Painter(Transform& transform, Settings& settings, Style& style)
    : transform(transform),
      settings(settings),
      style(style) {
}


void Painter::setup() {
    setupShaders();

    assert(plainShader);
    assert(outlineShader);
    assert(lineShader);
    assert(linejoinShader);
    assert(patternShader);

    glEnable(GL_STENCIL_TEST);

    // We are blending the new pixels *behind* the existing pixels. That way we can
    // draw front-to-back and use then stencil buffer to cull opaque pixels early.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
}

void Painter::setupShaders() {
    plainShader = std::make_unique<PlainShader>();
    outlineShader = std::make_unique<OutlineShader>();
    lineShader = std::make_unique<LineShader>();
    linejoinShader = std::make_unique<LinejoinShader>();
    patternShader = std::make_unique<PatternShader>();
}

void Painter::changeMatrix(const Tile::ID& id) {
    // Initialize projection matrix
    mat4 projMatrix;
    matrix::ortho(projMatrix, 0, transform.width, transform.height, 0, 1, 10);

    // The position matrix.
    transform.matrixFor(matrix, id);
    matrix::multiply(matrix, projMatrix, matrix);

    // The extrusion matrix.
    matrix::identity(exMatrix);
    matrix::multiply(exMatrix, projMatrix, exMatrix);
    matrix::rotate_z(exMatrix, exMatrix, transform.getAngle());

    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    matrix::identity(nativeMatrix);
    matrix::translate(nativeMatrix, nativeMatrix, 0, 0, -1);
    matrix::multiply(nativeMatrix, projMatrix, nativeMatrix);
}

void Painter::drawClippingMask() {
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->u_matrix, 1, GL_FALSE, matrix.data());

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


    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));

    // Draw the clipping mask
    glUniform4f(plainShader->u_color, 1.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, tileStencilBuffer.index());

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
    for (iterator it = layers.rbegin(), end = layers.rend(); it != end; ++it) {
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
                bucket_it->second->render(*this, layer_desc.name, tile->id);
            }
        }
    }
}

void Painter::renderFill(FillBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
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
        glUseProgram(plainShader->program);
        glUniformMatrix4fv(plainShader->u_matrix, 1, GL_FALSE, matrix.data());

        // Draw all groups
        bucket.drawElements(*plainShader);

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
        glUseProgram(outlineShader->program);
        glUniformMatrix4fv(outlineShader->u_matrix, 1, GL_FALSE, matrix.data());
        glLineWidth(2.0f); // This is always fixed and does not depend on the pixelRatio!

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
        bucket.drawVertices(*outlineShader);
    }

    // Only draw regions that we marked
    glStencilFunc(GL_NOTEQUAL, 0x0, 0x3F);

    if (properties.image.size() && style.sprite) {
        // Draw texture fill
        ImagePosition imagePos = style.sprite->getPosition(properties.image, true);

        double factor = 8.0 / pow(2, transform.getIntegerZoom() - id.z);
        double mix = fmod(transform.getZoom(), 1.0);
        vec2<double> imageSize { imagePos.size.x * factor, imagePos.size.y * factor };

        vec2<double> offset {
            fmod(id.x * 4096, imageSize.x),
            fmod(id.y * 4096, imageSize.y)
        };

        glUseProgram(patternShader->program);
        glUniformMatrix4fv(patternShader->u_matrix, 1, GL_FALSE, matrix.data());
        glUniform2f(patternShader->u_pattern_size, imageSize.x, imageSize.y);
        glUniform2f(patternShader->u_offset, offset.x, offset.y);
        glUniform2f(patternShader->u_pattern_tl, imagePos.tl.x, imagePos.tl.y);
        glUniform2f(patternShader->u_pattern_br, imagePos.br.x, imagePos.br.y);
        glUniform4fv(patternShader->u_color, 1, fill_color.data());
        glUniform1f(patternShader->u_mix, mix);
        style.sprite->bind(true);

        // Draw a rectangle that covers the entire viewport.
        coveringPatternArray.bind(*patternShader, tileStencilBuffer, BUFFER_OFFSET(0));
        glDrawArrays(GL_TRIANGLES, 0, tileStencilBuffer.index());

    } else {
        // Draw filling rectangle.
        glUseProgram(plainShader->program);
        glUniformMatrix4fv(plainShader->u_matrix, 1, GL_FALSE, matrix.data());
        glUniform4fv(plainShader->u_color, 1, fill_color.data());

        // Draw a rectangle that covers the entire viewport.
        coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));
        glDrawArrays(GL_TRIANGLES, 0, tileStencilBuffer.index());
    }

    glStencilMask(0x00);
    glStencilFunc(GL_EQUAL, 0x80, 0x80);
}

void Painter::renderLine(LineBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    const LineProperties& properties = style.computed.lines[layer_name];

    // Abort early.
    if (bucket.empty()) return;
    if (properties.hidden) return;

    double width = properties.width;
    double offset = properties.offset / 2;

    // These are the radii of the line. We are limiting it to 16, which will result
    // in a point size of 64 on retina.
    double inset = fmin((fmax(-1, offset - width / 2 - 0.5) + 1), 16.0f);
    double outset = fmin(offset + width / 2 + 0.5, 16.0f);

    Color color = properties.color;
    color[0] *= properties.opacity;
    color[1] *= properties.opacity;
    color[2] *= properties.opacity;
    color[3] *= properties.opacity;

    // We're only drawing end caps + round line joins if the line is > 2px. Otherwise, they aren't visible anyway.
    if (bucket.hasPoints() && outset > 1.0f) {
        glUseProgram(linejoinShader->program);
        glUniformMatrix4fv(linejoinShader->u_matrix, 1, GL_FALSE, matrix.data());
        glUniform4fv(linejoinShader->u_color, 1, color.data());
        glUniform2f(linejoinShader->u_world, transform.fb_width / 2, transform.fb_height / 2);
        glUniform2f(linejoinShader->u_linewidth, (outset - 0.25) * transform.pixelRatio, (inset - 0.25) * transform.pixelRatio);

    #if defined(GL_ES_VERSION_2_0)
        glUniform1f(linejoinShader->u_size, ceil(transform.pixelRatio * outset * 2.0));
    #else
        glPointSize(ceil(transform.pixelRatio * outset * 2.0));
    #endif

        bucket.drawPoints(*linejoinShader);
    }

    // var imagePos = properties.image && imageSprite.getPosition(properties.image);
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
        glUseProgram(lineShader->program);
        glUniformMatrix4fv(lineShader->u_matrix, 1, GL_FALSE, matrix.data());
        glUniformMatrix4fv(lineShader->u_exmatrix, 1, GL_FALSE, exMatrix.data());

        // TODO: Move this to transform?
        const double tilePixelRatio = transform.getScale() / (1 << transform.getIntegerZoom()) / 8;

        glUniform2f(lineShader->u_dasharray, 1, -1);
        glUniform2f(lineShader->u_linewidth, outset, inset);
        glUniform1f(lineShader->u_ratio, tilePixelRatio);
        glUniform4fv(lineShader->u_color, 1, color.data());
        glUniform1f(lineShader->u_debug, 0);
        bucket.drawLines(*lineShader);
    }
}

void Painter::renderDebug(const Tile::Ptr& tile) {
    // Blend to the front, not the back.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->u_matrix, 1, GL_FALSE, matrix.data());

    // draw tile outline
    tileBorderArray.bind(*plainShader, tileBorderBuffer, BUFFER_OFFSET(0));
    glUniform4f(plainShader->u_color, 1.0f, 0.0f, 0.0f, 1.0f);
    glLineWidth(4.0f * transform.pixelRatio);
    glDrawArrays(GL_LINE_STRIP, 0, tileBorderBuffer.index());

    // draw debug info
    tile->debugFontArray.bind(*plainShader, tile->debugFontBuffer, BUFFER_OFFSET(0));
    glUniform4f(plainShader->u_color, 1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(4.0f * transform.pixelRatio);
    glDrawArrays(GL_LINES, 0, tile->debugFontBuffer.index());
    glUniform4f(plainShader->u_color, 0.0f, 0.0f, 0.0f, 1.0f);
    glLineWidth(2.0f * transform.pixelRatio);
    glDrawArrays(GL_LINES, 0, tile->debugFontBuffer.index());

    // Revert blending mode to blend to the back.
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
}

void Painter::renderMatte() {
    glDisable(GL_STENCIL_TEST);

    Color white = {{ 0.9, 0.9, 0.9, 1 }};

    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->u_matrix, 1, GL_FALSE, nativeMatrix.data());

    // Draw the clipping mask
    matteArray.bind(*plainShader, matteBuffer, BUFFER_OFFSET(0));
    glUniform4fv(plainShader->u_color, 1, white.data());
    glDrawArrays(GL_TRIANGLES, 0, matteBuffer.index());

    glEnable(GL_STENCIL_TEST);
}

void Painter::renderBackground() {
    Color white = {{ 1, 1, 1, 1 }};

    glUseProgram(plainShader->program);
    glUniformMatrix4fv(plainShader->u_matrix, 1, GL_FALSE, matrix.data());

    // Draw the clipping mask
    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));
    glUniform4fv(plainShader->u_color, 1, white.data());
    glDrawArrays(GL_TRIANGLES, 0, tileStencilBuffer.index());
}
