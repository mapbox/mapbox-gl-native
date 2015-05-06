#include <mbgl/renderer/painter.hpp>

#include <mbgl/map/source.hpp>
#include <mbgl/map/tile.hpp>

#include <mbgl/platform/log.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket.hpp>

#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>

#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/dot_shader.hpp>
#include <mbgl/shader/gaussian_shader.hpp>

#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat3.hpp>

#if defined(DEBUG)
#include <mbgl/util/stopwatch.hpp>
#endif

#include <cassert>
#include <algorithm>
#include <iostream>

using namespace mbgl;

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

Painter::Painter(SpriteAtlas& spriteAtlas_, GlyphAtlas& glyphAtlas_, LineAtlas& lineAtlas_)
    : spriteAtlas(spriteAtlas_)
    , glyphAtlas(glyphAtlas_)
    , lineAtlas(lineAtlas_)
{
}

Painter::~Painter() {
}

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(std::chrono::milliseconds(300));
}

void Painter::setup() {
    // Enable GL debugging
    if ((gl::DebugMessageControl != nullptr) && (gl::DebugMessageCallback != nullptr)) {
        // This will enable all messages including performance hints
        //MBGL_CHECK_ERROR(gl::DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE));

        // This will only enable high and medium severity messages
        MBGL_CHECK_ERROR(gl::DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE));
        MBGL_CHECK_ERROR(gl::DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE));
        MBGL_CHECK_ERROR(gl::DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE));

        MBGL_CHECK_ERROR(gl::DebugMessageCallback(gl::debug_callback, nullptr));
    }

    setupShaders();

    assert(iconShader);
    assert(plainShader);
    assert(outlineShader);
    assert(lineShader);
    assert(linepatternShader);
    assert(patternShader);
    assert(rasterShader);
    assert(sdfGlyphShader);
    assert(sdfIconShader);
    assert(dotShader);
    assert(gaussianShader);


    // Blending
    // We are blending new pixels on top of old pixels. Since we have depth testing
    // and are drawing opaque fragments first front-to-back, then translucent
    // fragments back-to-front, this shades the fewest fragments possible.
    config.blend = true;
    MBGL_CHECK_ERROR(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));

    // Set clear values
    config.clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    config.clearDepth = 1.0f;
    config.clearStencil = 0x0;

    // Stencil test
    MBGL_CHECK_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

    // Depth test
    glDepthFunc(GL_LEQUAL);
}

void Painter::setupShaders() {
    if (!plainShader) plainShader = util::make_unique<PlainShader>();
    if (!outlineShader) outlineShader = util::make_unique<OutlineShader>();
    if (!lineShader) lineShader = util::make_unique<LineShader>();
    if (!linesdfShader) linesdfShader = util::make_unique<LineSDFShader>();
    if (!linepatternShader) linepatternShader = util::make_unique<LinepatternShader>();
    if (!patternShader) patternShader = util::make_unique<PatternShader>();
    if (!iconShader) iconShader = util::make_unique<IconShader>();
    if (!rasterShader) rasterShader = util::make_unique<RasterShader>();
    if (!sdfGlyphShader) sdfGlyphShader = util::make_unique<SDFGlyphShader>();
    if (!sdfIconShader) sdfIconShader = util::make_unique<SDFIconShader>();
    if (!dotShader) dotShader = util::make_unique<DotShader>();
    if (!gaussianShader) gaussianShader = util::make_unique<GaussianShader>();
}

void Painter::resize() {
    if (gl_viewport != state.getFramebufferDimensions()) {
        gl_viewport = state.getFramebufferDimensions();
        assert(gl_viewport[0] > 0 && gl_viewport[1] > 0);
        MBGL_CHECK_ERROR(glViewport(0, 0, gl_viewport[0], gl_viewport[1]));
    }
}

void Painter::setDebug(bool enabled) {
    debug = enabled;
}

void Painter::useProgram(uint32_t program) {
    if (gl_program != program) {
        MBGL_CHECK_ERROR(glUseProgram(program));
        gl_program = program;
    }
}

void Painter::lineWidth(float line_width) {
    if (gl_lineWidth != line_width) {
        MBGL_CHECK_ERROR(glLineWidth(line_width));
        gl_lineWidth = line_width;
    }
}

void Painter::changeMatrix() {
    // Initialize projection matrix
    matrix::ortho(projMatrix, 0, state.getWidth(), state.getHeight(), 0, 0, 1);

    // The extrusion matrix.
    matrix::identity(extrudeMatrix);
    matrix::multiply(extrudeMatrix, projMatrix, extrudeMatrix);
    matrix::rotate_z(extrudeMatrix, extrudeMatrix, state.getAngle());

    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    matrix::identity(nativeMatrix);
    matrix::multiply(nativeMatrix, projMatrix, nativeMatrix);
}

void Painter::clear() {
    gl::group group("clear");
    config.stencilTest = true;
    config.stencilMask = 0xFF;
    config.depthTest = false;
    config.depthMask = GL_TRUE;
    config.clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    MBGL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Painter::setOpaque() {
    if (pass != RenderPass::Opaque) {
        pass = RenderPass::Opaque;
        config.blend = false;
    }
}

void Painter::setTranslucent() {
    if (pass != RenderPass::Translucent) {
        pass = RenderPass::Translucent;
        config.blend = true;
    }
}

void Painter::setStrata(float value) {
    strata = value;
}

void Painter::prepareTile(const Tile& tile) {
    const GLint ref = (GLint)tile.clip.reference.to_ulong();
    const GLuint mask = (GLuint)tile.clip.mask.to_ulong();
    config.stencilFunc = { GL_EQUAL, ref, mask };
}

void Painter::render(const Style& style, TransformState state_, TimePoint time) {
    state = state_;

    std::set<Source*> sources;
    for (const auto& source : style.sources) {
        if (source->enabled) {
            sources.insert(source.get());
        }
    }

    resize();
    changeMatrix();

    // Figure out what buckets we have to draw and what order we have to draw them in.
    const auto order = determineRenderOrder(style);

    // - UPLOAD PASS -------------------------------------------------------------------------------
    // Uploads all required buffers and images before we do any actual rendering.
    {
        const gl::group upload("upload");

        tileStencilBuffer.upload();
        tileBorderBuffer.upload();
        spriteAtlas.upload();
        lineAtlas.upload();
        glyphAtlas.upload();

        for (const auto& item : order) {
            if (item.bucket && item.bucket->needsUpload()) {
                item.bucket->upload();
            }
        }
    }


    // - CLIPPING MASKS ----------------------------------------------------------------------------
    // Draws the clipping masks to the stencil buffer.
    {
        const gl::group clip("clip");

        // Update all clipping IDs.
        ClipIDGenerator generator;
        for (const auto& source : sources) {
            generator.update(source->getLoadedTiles());
            source->updateMatrices(projMatrix, state);
        }

        clear();

        drawClippingMasks(sources);
    }

    frameHistory.record(time, state.getNormalizedZoom());

    // Actually render the layers
    if (debug::renderTree) { Log::Info(Event::Render, "{"); indent++; }

    // TODO: Correctly compute the number of layers recursively beforehand.
    const float strata_thickness = 1.0f / (order.size() + 1);

    // Layer index
    int i = 0;

    // - OPAQUE PASS -------------------------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque objects first.
    {
        const gl::group _("opaque");

        if (debug::renderTree) {
            Log::Info(Event::Render, "%*s%s", indent++ * 4, "", "OPAQUE {");
        }
        i = 0;
        setOpaque();
        for (auto it = order.rbegin(), end = order.rend(); it != end; ++it, ++i) {
            const auto& item = *it;
            if (item.bucket && item.tile) {
                if (item.hasRenderPass(RenderPass::Opaque)) {
                    const gl::group group(item.layer.id + " - " + std::string(item.tile->id));
                    setStrata(i * strata_thickness);
                    prepareTile(*item.tile);
                    item.bucket->render(*this, item.layer, item.tile->id, item.tile->matrix);
                }
            } else {
                const gl::group group("background");
                renderBackground(item.layer);
            }
        }
        if (debug::renderTree) {
            Log::Info(Event::Render, "%*s%s", --indent * 4, "", "}");
        }
    }

    // - TRANSLUCENT PASS --------------------------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render bottom-to-top.
    {
        const gl::group _("translucent");

        if (debug::renderTree) {
            Log::Info(Event::Render, "%*s%s", indent++ * 4, "", "TRANSLUCENT {");
        }
        --i; // After the last iteration, this is incremented, so we have to decrement it again.
        setTranslucent();
        for (auto it = order.begin(), end = order.end(); it != end; ++it, --i) {
            const auto& item = *it;
            if (item.bucket && item.tile) {
                if (item.hasRenderPass(RenderPass::Translucent)) {
                    const gl::group group(item.layer.id + " - " + std::string(item.tile->id));
                    setStrata(i * strata_thickness);
                    prepareTile(*item.tile);
                    item.bucket->render(*this, item.layer, item.tile->id, item.tile->matrix);
                }
            }
        }
        if (debug::renderTree) {
            Log::Info(Event::Render, "%*s%s", --indent * 4, "", "}");
        }
    }

    if (debug::renderTree) { Log::Info(Event::Render, "}"); indent--; }

    // - DEBUG PASS --------------------------------------------------------------------------------
    // Renders debug overlays.
    {
        const gl::group _("debug");

        // Finalize the rendering, e.g. by calling debug render calls per tile.
        // This guarantees that we have at least one function per tile called.
        // When only rendering layers via the stylesheet, it's possible that we don't
        // ever visit a tile during rendering.
        for (const auto& source : sources) {
            source->finishRender(*this);
        }
    }

    // TODO: Find a better way to unbind VAOs after we're done with them without introducing
    // unnecessary bind(0)/bind(N) sequences.
    {
        const gl::group _("cleanup");

        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
        MBGL_CHECK_ERROR(gl::BindVertexArray(0));
    }
}

std::vector<RenderItem> Painter::determineRenderOrder(const Style& style) {
    std::vector<RenderItem> order;

    for (const auto& layerPtr : style.layers) {
        const auto& layer = *layerPtr;
        if (layer.bucket->visibility == VisibilityType::None) continue;
        if (layer.type == StyleLayerType::Background) {
            // This layer defines a background color/image.
            order.emplace_back(layer);
            continue;
        }

        // This is a singular layer.
        if (!layer.bucket) {
            Log::Warning(Event::Render, "layer '%s' is missing bucket", layer.id.c_str());
            continue;
        }

        if (!layer.bucket->source) {
            Log::Warning(Event::Render, "can't find source for layer '%s'", layer.id.c_str());
            continue;
        }

        // Skip this layer if it's outside the range of min/maxzoom.
        // This may occur when there /is/ a bucket created for this layer, but the min/max-zoom
        // is set to a fractional value, or value that is larger than the source maxzoom.
        const double zoom = state.getZoom();
        if (layer.bucket->min_zoom > zoom ||
            layer.bucket->max_zoom <= zoom) {
            continue;
        }

        // Don't include invisible layers.
        if (!layer.isVisible()) {
            continue;
        }

        // Determine what render passes we need for this layer.
        const RenderPass passes = determineRenderPasses(layer);

        const auto& tiles = layer.bucket->source->getTiles();
        for (auto tile : tiles) {
            assert(tile);
            if (!tile->data && !tile->data->ready()) {
                continue;
            }

            auto bucket = tile->data->getBucket(layer);
            if (bucket) {
                order.emplace_back(layer, tile, bucket, passes);
            }
        }
    }

    return order;
}

RenderPass Painter::determineRenderPasses(const StyleLayer& layer) {
    RenderPass passes = RenderPass::None;

    if (layer.properties.is<FillProperties>()) {
        const FillProperties &properties = layer.properties.get<FillProperties>();
        const float alpha = properties.fill_color[3] * properties.opacity;

        if (properties.antialias) {
            passes |= RenderPass::Translucent;
        }
        if (properties.image.from.size() || alpha < 1.0f) {
            passes |= RenderPass::Translucent;
        } else {
            passes |= RenderPass::Opaque;
        }
    } else {
        passes |= RenderPass::Translucent;
    }

    return passes;
}

void Painter::renderBackground(const StyleLayer &layer_desc) {
    const BackgroundProperties& properties = layer_desc.getProperties<BackgroundProperties>();

    if (properties.image.to.size()) {
        if ((properties.opacity >= 1.0f) != (pass == RenderPass::Opaque))
            return;

        SpriteAtlasPosition imagePosA = spriteAtlas.getPosition(properties.image.from, true);
        SpriteAtlasPosition imagePosB = spriteAtlas.getPosition(properties.image.to, true);
        float zoomFraction = state.getZoomFraction();

        useProgram(patternShader->program);
        patternShader->u_matrix = identityMatrix;
        patternShader->u_pattern_tl_a = imagePosA.tl;
        patternShader->u_pattern_br_a = imagePosA.br;
        patternShader->u_pattern_tl_b = imagePosB.tl;
        patternShader->u_pattern_br_b = imagePosB.br;
        patternShader->u_mix = properties.image.t;
        patternShader->u_opacity = properties.opacity;

        LatLng latLng = state.getLatLng();
        vec2<double> center = state.pixelForLatLng(latLng);
        float scale = 1 / std::pow(2, zoomFraction);

        std::array<float, 2> sizeA = imagePosA.size;
        mat3 matrixA;
        matrix::identity(matrixA);
        matrix::scale(matrixA, matrixA,
                      1.0f / (sizeA[0] * properties.image.fromScale),
                      1.0f / (sizeA[1] * properties.image.fromScale));
        matrix::translate(matrixA, matrixA,
                          std::fmod(center.x * 512, sizeA[0] * properties.image.fromScale),
                          std::fmod(center.y * 512, sizeA[1] * properties.image.fromScale));
        matrix::rotate(matrixA, matrixA, -state.getAngle());
        matrix::scale(matrixA, matrixA,
                       scale * state.getWidth()  / 2,
                      -scale * state.getHeight() / 2);

        std::array<float, 2> sizeB = imagePosB.size;
        mat3 matrixB;
        matrix::identity(matrixB);
        matrix::scale(matrixB, matrixB,
                      1.0f / (sizeB[0] * properties.image.toScale),
                      1.0f / (sizeB[1] * properties.image.toScale));
        matrix::translate(matrixB, matrixB,
                          std::fmod(center.x * 512, sizeB[0] * properties.image.toScale),
                          std::fmod(center.y * 512, sizeB[1] * properties.image.toScale));
        matrix::rotate(matrixB, matrixB, -state.getAngle());
        matrix::scale(matrixB, matrixB,
                       scale * state.getWidth()  / 2,
                      -scale * state.getHeight() / 2);

        patternShader->u_patternmatrix_a = matrixA;
        patternShader->u_patternmatrix_b = matrixB;

        backgroundBuffer.bind();
        patternShader->bind(0);
        spriteAtlas.bind(true);
    } else {
        Color color = properties.color;
        color[0] *= properties.opacity;
        color[1] *= properties.opacity;
        color[2] *= properties.opacity;
        color[3] *= properties.opacity;

        if ((color[3] >= 1.0f) != (pass == RenderPass::Opaque))
            return;

        useProgram(plainShader->program);
        plainShader->u_matrix = identityMatrix;
        plainShader->u_color = color;
        backgroundArray.bind(*plainShader, backgroundBuffer, BUFFER_OFFSET(0));
    }

    config.stencilTest = false;
    config.depthTest = true;
    config.depthRange = { strata + strata_epsilon, 1.0f };
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
}

mat4 Painter::translatedMatrix(const mat4& matrix, const std::array<float, 2> &translation, const TileID &id, TranslateAnchorType anchor) {
    if (translation[0] == 0 && translation[1] == 0) {
        return matrix;
    } else {
        // TODO: Get rid of the 8 (scaling from 4096 to tile size)
        const double factor = ((double)(1 << id.z)) / state.getScale() * (4096.0 / util::tileSize);

        mat4 vtxMatrix;
        if (anchor == TranslateAnchorType::Viewport) {
            const double sin_a = std::sin(-state.getAngle());
            const double cos_a = std::cos(-state.getAngle());
            matrix::translate(vtxMatrix, matrix,
                    factor * (translation[0] * cos_a - translation[1] * sin_a),
                    factor * (translation[0] * sin_a + translation[1] * cos_a),
                    0);
        } else {
            matrix::translate(vtxMatrix, matrix,
                    factor * translation[0],
                    factor * translation[1],
                    0);
        }

        return vtxMatrix;
    }
}
