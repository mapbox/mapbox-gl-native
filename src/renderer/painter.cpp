#include <cassert>
#include <algorithm>


#include <llmr/renderer/painter.hpp>
#include <llmr/util/std.hpp>

#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/renderer/point_bucket.hpp>
#include <llmr/renderer/text_bucket.hpp>

#include <llmr/map/transform.hpp>
#include <llmr/map/settings.hpp>
#include <llmr/geometry/debug_font_buffer.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/style/style.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/util/raster.hpp>

using namespace llmr;

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

Painter::Painter(Transform& transform, Settings& settings, Style& style, GlyphAtlas& glyphAtlas)
    : transform(transform),
      settings(settings),
      style(style),
      glyphAtlas(glyphAtlas) {
}

void Painter::setup() {
    setupShaders();

    assert(pointShader);
    assert(plainShader);
    assert(outlineShader);
    assert(lineShader);
    assert(linejoinShader);
    assert(patternShader);
    assert(rasterShader);
    assert(textShader);


    // Blending
    // We are blending new pixels on top of old pixels. Since we have depth testing
    // and are drawing opaque fragments first front-to-back, then translucent
    // fragments back-to-front, this shades the fewest fragments possible.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

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
    rasterShader = std::make_unique<RasterShader>();
    textShader = std::make_unique<TextShader>();
}

void Painter::resize(int width, int height) {
    glViewport(0, 0, width, height);
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
    matrix::ortho(projMatrix, 0, transform.width, transform.height, 0, 0, 1);

    // The extrusion matrix.
    matrix::identity(extrudeMatrix);
    matrix::multiply(extrudeMatrix, projMatrix, extrudeMatrix);
    matrix::rotate_z(extrudeMatrix, extrudeMatrix, transform.getAngle());

    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    matrix::identity(nativeMatrix);
    matrix::multiply(nativeMatrix, projMatrix, nativeMatrix);
}

void Painter::prepareClippingMask() {
    useProgram(plainShader->program);
    glDisable(GL_DEPTH_TEST);
    depthMask(false);
    glDepthRange(1.0f, 1.0f);

    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));
}

void Painter::drawClippingMask(const mat4& matrix, uint8_t clip_id, bool opaque) {
    plainShader->setMatrix(matrix);
    if (opaque) {
        plainShader->setColor(style.computed.background.color);
    }

    glStencilFunc(GL_ALWAYS, clip_id, 0xFF);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
}

void Painter::finishClippingMask() {
    glEnable(GL_DEPTH_TEST);
    depthMask(true);
    glStencilMask(0x0);
}

void Painter::clear() {
    glStencilMask(0xFF);
    depthMask(true);
#ifdef NVIDIA
    // We're painting in a way that allows us to skip clearing the color buffer.
    // On Tegra hardware, this is faster.
    glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
}

void Painter::render(const Tile& tile) {
    assert(tile.data);
    if (tile.data->state != TileData::State::parsed) {
        return;
    }

    frameHistory.record(transform.getNormalizedZoom());

    matrix = tile.matrix;
    glStencilFunc(GL_EQUAL, tile.clip_id, 0xFF);

    if (tile.data->use_raster) {
        renderRaster(tile.data);
    } else {
        renderLayers(tile.data, style.layers);
    }

    if (settings.debug) {
        renderDebug(tile.data);
    }
}

void Painter::renderRaster(const std::shared_ptr<TileData>& tile_data) {
    useProgram(rasterShader->program);
    rasterShader->setMatrix(matrix);
    rasterShader->setImage(0);
    rasterShader->setOpacity(tile_data->raster->opacity);
    tile_data->raster->bind(true);

    coveringRasterArray.bind(*rasterShader, tileStencilBuffer, BUFFER_OFFSET(0));
    glDepthRange(strata, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
}

void Painter::renderLayers(const std::shared_ptr<TileData>& tile_data, const std::vector<LayerDescription>& layers) {
    float strata_thickness = 1.0f / (layers.size() + 1);

    // - FIRST PASS ------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque
    // objects first.
    pass = Opaque;
    glDisable(GL_BLEND);
    depthMask(true);

    typedef std::vector<LayerDescription>::const_reverse_iterator riterator;
    int i = 0;
    for (riterator it = layers.rbegin(), end = layers.rend(); it != end; ++it, ++i) {
        strata = i * strata_thickness;
        renderLayer(tile_data, *it);
    }

    // - SECOND PASS -----------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render
    // bottom-to-top.
    pass = Translucent;
    glEnable(GL_BLEND);
    depthMask(false);

    typedef std::vector<LayerDescription>::const_iterator iterator;
    --i;
    for (iterator it = layers.begin(), end = layers.end(); it != end; ++it, --i) {
        strata = i * strata_thickness;
        renderLayer(tile_data, *it);
    }
}

void Painter::renderLayer(const std::shared_ptr<TileData>& tile_data, const LayerDescription& layer_desc) {
    if (layer_desc.child_layer.size()) {
        // This is a layer group.
        // TODO: create framebuffer
        renderLayers(tile_data, layer_desc.child_layer);
        // TODO: render framebuffer on previous framebuffer
    } else {
        // This is a singular layer. Try to find the bucket associated with
        // this layer and render it.
        auto bucket_it = tile_data->buckets.find(layer_desc.bucket_name);
        if (bucket_it != tile_data->buckets.end()) {
            assert(bucket_it->second);
            bucket_it->second->render(*this, layer_desc.name, tile_data->id);
        }
    }
}

void Painter::renderFill(FillBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    // Abort early.
    if (bucket.empty()) return;

    const FillProperties& properties = style.computed.fills[layer_name];
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
    if (outline && pass == Translucent) {
        useProgram(outlineShader->program);
        outlineShader->setMatrix(matrix);
        lineWidth(2.0f); // This is always fixed and does not depend on the pixelRatio!

        outlineShader->setColor(stroke_color);

        // Draw the entire line
        outlineShader->setWorld({{ transform.fb_width, transform.fb_height }});
        glDepthRange(strata, 1.0f);
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

    if ((fill_color[3] >= 1.0f) == (pass == Opaque)) {
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
            style.sprite->raster->bind(true);

            // Draw the actual triangles into the color & stencil buffer.
            glDepthRange(strata + strata_epsilon, 1.0f);
            bucket.drawElements(*patternShader);
        } else {
            // Only draw the fill when it's either opaque and we're drawing opaque
            // fragments or when it's translucent and we're drawing translucent
            // fragments
            // Draw filling rectangle.
            useProgram(plainShader->program);
            plainShader->setMatrix(matrix);
            plainShader->setColor(fill_color);

            // Draw the actual triangles into the color & stencil buffer.
            glDepthRange(strata + strata_epsilon, 1.0f);
            bucket.drawElements(*plainShader);
        }
    }

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (fringeline && pass == Translucent) {
        useProgram(outlineShader->program);
        outlineShader->setMatrix(matrix);
        lineWidth(3.0f); // This is always fixed and does not depend on the pixelRatio!

        outlineShader->setColor(fill_color);

        // Draw the entire line
        outlineShader->setWorld({{
                transform.fb_width,
                transform.fb_height
            }
        });

        glDepthRange(strata + strata_epsilon, 1.0f);
        bucket.drawVertices(*outlineShader);
    }
}

void Painter::renderLine(LineBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    // Abort early.
    if (pass == Opaque) return;
    if (bucket.empty()) return;

    const LineProperties& properties = style.computed.lines[layer_name];
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


    glDepthRange(strata, 1.0f);

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
        bucket.drawLines(*lineShader);
    }
}

void Painter::renderPoint(PointBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    // Abort early.
    if (!bucket.hasPoints()) return;
    if (pass == Opaque) return;

    const PointProperties& properties = style.computed.points[layer_name];
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
    if (*style.sprite->raster) {
        style.sprite->raster->bind(transform.rotating || transform.scaling || transform.panning);
    }
    glDepthRange(strata, 1.0f);
    bucket.drawPoints(*pointShader);
}

void Painter::renderText(TextBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    // Abort early.
    if (pass == Opaque) return;
    if (bucket.empty()) return;

    const TextProperties& properties = style.computed.texts[layer_name];
    if (properties.hidden) return;

    mat4 exMatrix;
    matrix::copy(exMatrix, projMatrix);
    if (bucket.geom_desc.path == TextPathType::Curve) {
        matrix::rotate_z(exMatrix, exMatrix, transform.getAngle());
    }

    const float rotate = properties.rotate;
    if (rotate != 0.0f) {
        matrix::rotate_z(exMatrix, exMatrix, rotate);
    }

    // If layerStyle.size > bucket.info.fontSize then labels may collide
    float fontSize = fmin(properties.size, bucket.geom_desc.font_size);
    matrix::scale(exMatrix, exMatrix, fontSize / 24.0f, fontSize / 24.0f, 1.0f);

    useProgram(textShader->program);
    textShader->setMatrix(matrix);
    textShader->setExtrudeMatrix(exMatrix);

    glyphAtlas.bind();
    textShader->setTextureSize({{ static_cast<float>(glyphAtlas.width), static_cast<float>(glyphAtlas.height) }});

    textShader->setGamma(2.5f / fontSize / transform.pixelRatio);

    // Convert the -pi..pi to an int8 range.
    float angle = round((transform.getAngle() + rotate) / M_PI * 128);

    // adjust min/max zooms for variable font sies
    float zoomAdjust = log(fontSize / bucket.geom_desc.font_size) / log(2);

    textShader->setAngle((int32_t)(angle + 256) % 256);
    textShader->setFlip(bucket.geom_desc.path == TextPathType::Curve ? 1 : 0);
    textShader->setZoom((transform.getNormalizedZoom() - zoomAdjust) * 10); // current zoom level

    // Label fading
    const float duration = 300.0f;
    const float currentTime = platform::time() * 1000;

    std::deque<FrameSnapshot> &history = frameHistory.history;

    // Remove frames until only one is outside the duration, or until there are only three
    while (history.size() > 3 && history[1].time + duration < currentTime) {
        history.pop_front();
    }

    if (history[1].time + duration < currentTime) {
        history[0].z = history[1].z;
    }

    size_t frameLen = history.size();
    assert("there should never be less than three frames in the history" && frameLen >= 3);

    // Find the range of zoom levels we want to fade between
    float startingZ = history.front().z;
    const FrameSnapshot lastFrame = history.back();
    float endingZ = lastFrame.z;
    float lowZ = fmin(startingZ, endingZ);
    float highZ = fmax(startingZ, endingZ);

    // Calculate the speed of zooming, and how far it would zoom in terms of zoom levels in one duration
    float zoomDiff = endingZ - history[1].z,
        timeDiff = lastFrame.time - history[1].time;
    if (timeDiff > duration) timeDiff = 1;
    float fadedist = zoomDiff / (timeDiff / duration);

    if (isnan(fadedist)) fprintf(stderr, "fadedist should never be NaN");

    // At end of a zoom when the zoom stops changing continue pretending to zoom at that speed
    // bump is how much farther it would have been if it had continued zooming at the same rate
    float bump = (currentTime - lastFrame.time) / duration * fadedist;

    textShader->setFadeDist(fadedist * 10);
    textShader->setMinFadeZoom(floor(lowZ * 10));
    textShader->setMaxFadeZoom(floor(highZ * 10));
    textShader->setFadeZoom((transform.getZoom() + bump) * 10);

    // We're drawing in the translucent pass which is bottom-to-top, so we need
    // to draw the halo first.
    if (properties.halo[3] > 0.0f) {
        textShader->setColor(properties.halo);
        textShader->setBuffer(properties.haloRadius);
        glDepthRange(strata, 1.0f);
        bucket.drawGlyphs(*textShader);
    }

    // Then, we draw the text over the halo
    textShader->setColor(properties.color);
    textShader->setBuffer((256.0f - 64.0f) / 256.0f);
    glDepthRange(strata + strata_epsilon, 1.0f);
    bucket.drawGlyphs(*textShader);
}

void Painter::renderDebug(const TileData::Ptr& tile_data) {
    // Disable depth test and don't count this towards the depth buffer,
    // but *don't* disable stencil test, as we want to clip the red tile border
    // to the tile viewport.
    glDisable(GL_DEPTH_TEST);

    useProgram(plainShader->program);
    plainShader->setMatrix(matrix);

    // draw tile outline
    tileBorderArray.bind(*plainShader, tileBorderBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(1.0f, 0.0f, 0.0f, 1.0f);
    lineWidth(4.0f * transform.pixelRatio);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)tileBorderBuffer.index());

    // draw debug info
    tile_data->debugFontArray.bind(*plainShader, tile_data->debugFontBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    lineWidth(4.0f * transform.pixelRatio);
    glDrawArrays(GL_LINES, 0, (GLsizei)tile_data->debugFontBuffer.index());
    plainShader->setColor(0.0f, 0.0f, 0.0f, 1.0f);
    lineWidth(2.0f * transform.pixelRatio);
    glDrawArrays(GL_LINES, 0, (GLsizei)tile_data->debugFontBuffer.index());

    glEnable(GL_DEPTH_TEST);
}

void Painter::renderMatte() {
    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_EQUAL, 0x0, 0xFF);

    Color white = {{ 0.9, 0.9, 0.9, 1 }};

    useProgram(plainShader->program);
    plainShader->setMatrix(nativeMatrix);

    // Draw the clipping mask
    matteArray.bind(*plainShader, matteBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(white);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)matteBuffer.index());

    glEnable(GL_DEPTH_TEST);
}
