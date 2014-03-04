#include <cassert>
#include <algorithm>


#include <llmr/renderer/painter.hpp>
#include <llmr/util/std.hpp>

#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/renderer/point_bucket.hpp>

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

    assert(pointShader);
    assert(plainShader);
    assert(outlineShader);
    assert(lineShader);
    assert(linejoinShader);
    assert(patternShader);


    // Blending
    // We are blending the new pixels *behind* the existing pixels. That way we can
    // draw front-to-back and use then stencil buffer to cull opaque pixels early.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

    // Set clear values
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClearStencil(0x0);

    // Stencil test
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void Painter::setupShaders() {
    plainShader = std::make_unique<PlainShader>();
    outlineShader = std::make_unique<OutlineShader>();
    lineShader = std::make_unique<LineShader>();
    linejoinShader = std::make_unique<LinejoinShader>();
    patternShader = std::make_unique<PatternShader>();
    pointShader = std::make_unique<PointShader>();
}

void Painter::useProgram(uint32_t program) {
    if (gl_program != program) {
        glUseProgram(program);
        gl_program = program;
    }
}

void Painter::lineWidth(float lineWidth) {
    if (gl_lineWidth != lineWidth) {
        glLineWidth(lineWidth);
        gl_lineWidth = lineWidth;
    }
}

void Painter::depthMask(bool value) {
    if (gl_depthMask != value) {
        glDepthMask(value ? GL_TRUE : GL_FALSE);
        gl_depthMask = value;
    }
}

void Painter::changeMatrix() {
    // Initialize projection matrix
    matrix::ortho(projMatrix, 0, transform.width, transform.height, 0, 1, 10);

    // The extrusion matrix.
    matrix::identity(extrudeMatrix);
    matrix::multiply(extrudeMatrix, projMatrix, extrudeMatrix);
    matrix::rotate_z(extrudeMatrix, extrudeMatrix, transform.getAngle());

    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    matrix::identity(nativeMatrix);
    matrix::translate(nativeMatrix, nativeMatrix, 0, 0, -1);
    matrix::multiply(nativeMatrix, projMatrix, nativeMatrix);
}

void Painter::prepareClippingMask() {
    useProgram(plainShader->program);
    glDisable(GL_DEPTH_TEST);

    glColorMask(false, false, false, false);
    // This has been set by the call to Painter::clear()
    // glStencilMask(0xFF);

    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));
}

void Painter::drawClippingMask(const mat4& matrix, uint8_t clip_id) {
    plainShader->setMatrix(matrix);
    glStencilFunc(GL_ALWAYS, clip_id, 0xFF);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
}

void Painter::finishClippingMask() {
    glColorMask(true, true, true, true);
    glEnable(GL_DEPTH_TEST);
    glStencilMask(0x0);
}

void Painter::clear() {
    glStencilMask(0xFF);
    depthMask(true);
    // glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Painter::render(const Tile::Ptr& tile) {
    if (tile->state != Tile::parsed) {
        return;
    }

    matrix = tile->matrix;
    glStencilFunc(GL_EQUAL, tile->clip_id, 0xFF);

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
    if (bucket.empty()) return;
    if (properties.hidden) return;

    Color fill_color = properties.fill_color;
    fill_color[0] *= properties.opacity;
    fill_color[1] *= properties.opacity;
    fill_color[2] *= properties.opacity;
    fill_color[3] *= properties.opacity;

    Color stroke_color = properties.stroke_color;
    stroke_color[0] *= properties.opacity;
    stroke_color[1] *= properties.opacity;
    stroke_color[2] *= properties.opacity;
    stroke_color[3] *= properties.opacity;

    bool outline = properties.antialias && properties.stroke_color != properties.fill_color;
    bool fringeline = properties.antialias && properties.stroke_color == properties.fill_color;
    if (fringeline) {
        outline = true;
        stroke_color = fill_color;
    }

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (outline) {
        useProgram(outlineShader->program);
        outlineShader->setMatrix(matrix);
        lineWidth(2.0f); // This is always fixed and does not depend on the pixelRatio!

        outlineShader->setColor(stroke_color);
        depthMask(false);

        // Draw the entire line
        outlineShader->setWorld({{ transform.fb_width, transform.fb_height }});
        bucket.drawVertices(*outlineShader);
    } else if (fringeline) {
        // // We're only drawing to the first seven bits (== support a maximum of
        // // 127 overlapping polygons in one place before we get rendering errors).
        // glStencilMask(0x3F);
        // glClear(GL_STENCIL_BUFFER_BIT);

        // // Draw front facing triangles. Wherever the 0x80 bit is 1, we are
        // // increasing the lower 7 bits by one if the triangle is a front-facing
        // // triangle. This means that all visible polygons should be in CCW
        // // orientation, while all holes (see below) are in CW orientation.
        // glStencilFunc(GL_EQUAL, 0x80, 0x80);

        // // When we do a nonzero fill, we count the number of times a pixel is
        // // covered by a counterclockwise polygon, and subtract the number of
        // // times it is "uncovered" by a clockwise polygon.
        // glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    }

    if (properties.image.size() && *style.sprite) {
        // Draw texture fill
        ImagePosition imagePos = style.sprite->getPosition(properties.image, true);

        float factor = 8.0 / pow(2, transform.getIntegerZoom() - id.z);
        float mix = fmod(transform.getZoom(), 1.0);

        std::array<float, 2> imageSize = {{
                imagePos.size.x * factor,
                imagePos.size.y *factor
            }
        };

        std::array<float, 2> offset = {{
                (float)fmod(id.x * 4096, imageSize[0]),
                (float)fmod(id.y * 4096, imageSize[1])
            }
        };

        useProgram(patternShader->program);
        patternShader->setMatrix(matrix);
        patternShader->setOffset(offset);
        patternShader->setPatternSize(imageSize);
        patternShader->setPatternTopLeft({{ imagePos.tl.x, imagePos.tl.y }});
        patternShader->setPatternBottomRight({{ imagePos.br.x, imagePos.br.y }});
        patternShader->setColor(fill_color);
        patternShader->setMix(mix);
        style.sprite->bind(true);

        // TODO: Find out if the texture is completely opaque. If it is, we can
        // set the mask to GL_TRUE so that we won't draw any further fragments
        // after this one.
        depthMask(false);

        // Draw the actual triangles into the color & stencil buffer.
        bucket.drawElements(*patternShader);
    } else {
        // Draw filling rectangle.
        useProgram(plainShader->program);
        plainShader->setMatrix(matrix);
        plainShader->setColor(fill_color);

        // Only update the depth buffer if the color we're drawing is completely
        // opaque.
        depthMask(fill_color[3] >= 1.0f);

        // Draw the actual triangles into the color & stencil buffer.
        bucket.drawElements(*plainShader);
    }

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (fringeline) {
        // useProgram(outlineShader->program);
        // outlineShader->setMatrix(matrix);
        // lineWidth(2.0f); // This is always fixed and does not depend on the pixelRatio!

        // // From now on, we don't want to update the stencil buffer anymore.
        // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        // glStencilMask(0x0);

        // // Otherwise, we only want to draw the antialiased parts that are
        // // *outside* the current shape. This is important in case the fill
        // // or stroke color is translucent. If we wouldn't clip to outside
        // // the current shape, some pixels from the outline stroke overlapped
        // // the (non-antialiased) fill.
        // glStencilFunc(GL_EQUAL, 0x80, 0xBF);
        // outlineShader->setColor(fill_color);

        // // Draw the entire line
        // outlineShader->setWorld({{ transform.fb_width, transform.fb_height }});
        // bucket.drawVertices(*outlineShader);

        // glStencilFunc(GL_EQUAL, 0x80, 0x80);
    }
}

void Painter::renderLine(LineBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    const LineProperties& properties = style.computed.lines[layer_name];

    // Abort early.
    if (bucket.empty()) return;
    if (properties.hidden) return;

    float width = properties.width;
    float offset = properties.offset / 2;

    // These are the radii of the line. We are limiting it to 16, which will result
    // in a point size of 64 on retina.
    float inset = fmin((fmax(-1, offset - width / 2 - 0.5) + 1), 16.0f);
    float outset = fmin(offset + width / 2 + 0.5, 16.0f);

    Color color = properties.color;
    color[0] *= properties.opacity;
    color[1] *= properties.opacity;
    color[2] *= properties.opacity;
    color[3] *= properties.opacity;

    // We're only drawing end caps + round line joins if the line is > 2px. Otherwise, they aren't visible anyway.
    if (bucket.hasPoints() && outset > 1.0f) {
        useProgram(linejoinShader->program);
        linejoinShader->setMatrix(matrix);
        linejoinShader->setColor(color);
        linejoinShader->setWorld({{
                transform.fb_width * 0.5f,
                transform.fb_height * 0.5f
            }
        });
        linejoinShader->setLineWidth({{
                ((outset - 0.25f) * transform.pixelRatio),
                ((inset - 0.25f) * transform.pixelRatio)
            }
        });

        float pointSize = ceil(transform.pixelRatio * outset * 2.0);
#if defined(GL_ES_VERSION_2_0)
        linejoinShader->setSize(pointSize);
#else
        glPointSize(pointSize);
#endif
        depthMask(false);
        bucket.drawPoints(*linejoinShader);
    }

    // var imagePos = properties.image && imageSprite.getPosition(properties.image);
    bool imagePos = false;
    if (imagePos) {
        // var factor = 8 / Math.pow(2, painter.transform.zoom - params.z);

        // imageSprite.bind(gl, true);

        // //factor = Math.pow(2, 4 - painter.transform.zoom + params.z);
        // gl.switchShader(painter.linepatternShader, painter.translatedMatrix || painter.posMatrix, painter.extrudeMatrix);
        // shader = painter.linepatternShader;
        // glUniform2fv(painter.linepatternShader.u_pattern_size, [imagePos.size[0] * factor, imagePos.size[1] ]);
        // glUniform2fv(painter.linepatternShader.u_pattern_tl, imagePos.tl);
        // glUniform2fv(painter.linepatternShader.u_pattern_br, imagePos.br);
        // glUniform1f(painter.linepatternShader.u_fade, painter.transform.z % 1.0);

    } else {
        useProgram(lineShader->program);
        lineShader->setMatrix(matrix);
        lineShader->setExtrudeMatrix(extrudeMatrix);

        // TODO: Move this to transform?
        const float tilePixelRatio = transform.getScale() / (1 << transform.getIntegerZoom()) / 8;

        lineShader->setDashArray({{ 1, -1 }});
        lineShader->setLineWidth({{ outset, inset }});
        lineShader->setRatio(tilePixelRatio);
        lineShader->setColor(color);
        lineShader->setDebug(0);
        depthMask(false);
        bucket.drawLines(*lineShader);
    }
}

void Painter::renderPoint(PointBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    const PointProperties& properties = style.computed.points[layer_name];

    // Abort early.
    if (!bucket.hasPoints()) return;
    if (properties.hidden) return;

    Color color = properties.color;
    color[0] *= properties.opacity;
    color[1] *= properties.opacity;
    color[2] *= properties.opacity;
    color[3] *= properties.opacity;

    std::string sized_image = properties.image;
    sized_image.append("-");
    sized_image.append(std::to_string(static_cast<int>(std::round(properties.size))));

    ImagePosition imagePos = style.sprite->getPosition(sized_image, false);

    // fprintf(stderr, "%f/%f => %f/%f\n", imagePos.tl.x, imagePos.tl.y, imagePos.br.x, imagePos.br.y);

    useProgram(pointShader->program);
    pointShader->setMatrix(matrix);
    pointShader->setImage(0);
    pointShader->setColor(color);
    const float pointSize = properties.size * 1.4142135623730951 * transform.pixelRatio;
    #if defined(GL_ES_VERSION_2_0)
        pointShader->setSize(pointSize);
    #else
        glPointSize(pointSize);
        glEnable(GL_POINT_SPRITE);
    #endif
    pointShader->setPointTopLeft({{ imagePos.tl.x, imagePos.tl.y }});
    pointShader->setPointBottomRight({{ imagePos.br.x, imagePos.br.y }});
    style.sprite->bind(transform.rotating || transform.scaling || transform.panning);
    depthMask(false);
    bucket.drawPoints(*pointShader);
}

void Painter::renderDebug(const Tile::Ptr& tile) {
    // Disable depth test and don't count this towards the depth buffer,
    // but *don't* disable stencil test, as we want to clip the red tile border
    // to the tile viewport.
    glDisable(GL_DEPTH_TEST);
    depthMask(false);

    // Blend to the front, not the back.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    useProgram(plainShader->program);
    plainShader->setMatrix(matrix);

    // draw tile outline
    tileBorderArray.bind(*plainShader, tileBorderBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(1.0f, 0.0f, 0.0f, 1.0f);
    lineWidth(4.0f * transform.pixelRatio);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)tileBorderBuffer.index());

    // draw debug info
    tile->debugFontArray.bind(*plainShader, tile->debugFontBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    lineWidth(4.0f * transform.pixelRatio);
    glDrawArrays(GL_LINES, 0, (GLsizei)tile->debugFontBuffer.index());
    plainShader->setColor(0.0f, 0.0f, 0.0f, 1.0f);
    lineWidth(2.0f * transform.pixelRatio);
    glDrawArrays(GL_LINES, 0, (GLsizei)tile->debugFontBuffer.index());

    // Revert blending mode to blend to the back.
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    glEnable(GL_DEPTH_TEST);
}

void Painter::renderMatte() {
    glStencilFunc(GL_EQUAL, 0x0, 0xFF);

    Color white = {{ 0.9, 0.9, 0.9, 1 }};

    useProgram(plainShader->program);
    plainShader->setMatrix(nativeMatrix);

    // Draw the clipping mask
    matteArray.bind(*plainShader, matteBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(white);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)matteBuffer.index());
}

void Painter::renderBackground() {
    Color fill_color = {{ 1, 1, 1, 1 }};

    useProgram(plainShader->program);
    plainShader->setMatrix(matrix);

    // Draw the clipping mask
    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(fill_color);
    depthMask(fill_color[3] >= 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
}
