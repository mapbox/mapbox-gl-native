#include <mbgl/renderer/painter.hpp>

#include <mbgl/map/source.hpp>
#include <mbgl/map/tile.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/map/map_data.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/gl/debugging.hpp>

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
#include <mbgl/shader/box_shader.hpp>
#include <mbgl/shader/circle_shader.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat3.hpp>

#if defined(DEBUG)
#include <mbgl/util/stopwatch.hpp>
#endif

#include <cassert>
#include <algorithm>
#include <iostream>

using namespace mbgl;

Painter::Painter(MapData& data_) : data(data_) {
    setup();
}

Painter::~Painter() {
}

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(data.getDefaultFadeDuration());
}

void Painter::setup() {
    gl::debugging::enable();

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
    assert(circleShader);


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
    if (!plainShader) plainShader = std::make_unique<PlainShader>();
    if (!outlineShader) outlineShader = std::make_unique<OutlineShader>();
    if (!lineShader) lineShader = std::make_unique<LineShader>();
    if (!linesdfShader) linesdfShader = std::make_unique<LineSDFShader>();
    if (!linepatternShader) linepatternShader = std::make_unique<LinepatternShader>();
    if (!patternShader) patternShader = std::make_unique<PatternShader>();
    if (!iconShader) iconShader = std::make_unique<IconShader>();
    if (!rasterShader) rasterShader = std::make_unique<RasterShader>();
    if (!sdfGlyphShader) sdfGlyphShader = std::make_unique<SDFGlyphShader>();
    if (!sdfIconShader) sdfIconShader = std::make_unique<SDFIconShader>();
    if (!dotShader) dotShader = std::make_unique<DotShader>();
    if (!collisionBoxShader) collisionBoxShader = std::make_unique<CollisionBoxShader>();
    if (!circleShader) circleShader = std::make_unique<CircleShader>();
}

void Painter::resize() {
    if (gl_viewport != frame.framebufferSize) {
        gl_viewport = frame.framebufferSize;
        assert(gl_viewport[0] > 0 && gl_viewport[1] > 0);
        MBGL_CHECK_ERROR(glViewport(0, 0, gl_viewport[0], gl_viewport[1]));
    }
}

void Painter::useProgram(GLuint program) {
    if (gl_program != program) {
        MBGL_CHECK_ERROR(glUseProgram(program));
        gl_program = program;
    }
}

void Painter::lineWidth(GLfloat line_width) {
    if (gl_lineWidth != line_width) {
        MBGL_CHECK_ERROR(glLineWidth(line_width));
        gl_lineWidth = line_width;
    }
}

void Painter::changeMatrix() {

    state.getProjMatrix(projMatrix);

    // The extrusion matrix.
    matrix::ortho(extrudeMatrix, 0, state.getWidth(), state.getHeight(), 0, 0, -1);

    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    matrix::identity(nativeMatrix);
    matrix::multiply(nativeMatrix, projMatrix, nativeMatrix);
}

void Painter::clear() {
    MBGL_DEBUG_GROUP("clear");
    config.stencilTest = true;
    config.stencilMask = 0xFF;
    config.depthTest = false;
    config.depthMask = GL_TRUE;
    config.clearColor = { background[0], background[1], background[2], background[3] };
    MBGL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Painter::prepareTile(const Tile& tile) {
    const GLint ref = (GLint)tile.clip.reference.to_ulong();
    const GLuint mask = (GLuint)tile.clip.mask.to_ulong();
    config.stencilFunc = { GL_EQUAL, ref, mask };
}

void Painter::render(const Style& style, TransformState state_, const FrameData& frame_) {
    state = state_;
    frame = frame_;

    glyphAtlas = style.glyphAtlas.get();
    spriteAtlas = style.spriteAtlas.get();
    lineAtlas = style.lineAtlas.get();

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
        MBGL_DEBUG_GROUP("upload");

        tileStencilBuffer.upload();
        tileBorderBuffer.upload();
        spriteAtlas->upload();
        lineAtlas->upload();
        glyphAtlas->upload();

        for (const auto& item : order) {
            if (item.bucket && item.bucket->needsUpload()) {
                item.bucket->upload();
            }
        }
    }


    // - CLIPPING MASKS ----------------------------------------------------------------------------
    // Draws the clipping masks to the stencil buffer.
    {
        MBGL_DEBUG_GROUP("clip");

        // Update all clipping IDs.
        ClipIDGenerator generator;
        for (const auto& source : sources) {
            generator.update(source->getLoadedTiles());
            source->updateMatrices(projMatrix, state);
        }

        clear();

        drawClippingMasks(sources);
    }

    frameHistory.record(data.getAnimationTime(), state.getNormalizedZoom());

    // Actually render the layers
    if (debug::renderTree) { Log::Info(Event::Render, "{"); indent++; }

    // TODO: Correctly compute the number of layers recursively beforehand.
    depthRangeSize = 1 - (order.size() + 2) * numSublayers * depthEpsilon;

    // - OPAQUE PASS -------------------------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque objects first.
    renderPass(RenderPass::Opaque,
               order.rbegin(), order.rend(),
               0, 1);

    // - TRANSLUCENT PASS --------------------------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render bottom-to-top.
    renderPass(RenderPass::Translucent,
               order.begin(), order.end(),
               static_cast<GLsizei>(order.size()) - 1, -1);

    if (debug::renderTree) { Log::Info(Event::Render, "}"); indent--; }

    // - DEBUG PASS --------------------------------------------------------------------------------
    // Renders debug overlays.
    {
        MBGL_DEBUG_GROUP("debug");

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
        MBGL_DEBUG_GROUP("cleanup");

        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
        MBGL_CHECK_ERROR(VertexArrayObject::Unbind());
    }
}

template <class Iterator>
void Painter::renderPass(RenderPass pass_,
                         Iterator it, Iterator end,
                         GLsizei i, int8_t increment) {
    pass = pass_;

    MBGL_DEBUG_GROUP(pass == RenderPass::Opaque ? "opaque" : "translucent");

    if (debug::renderTree) {
        Log::Info(Event::Render, "%*s%s {", indent++ * 4, "",
                  pass == RenderPass::Opaque ? "opaque" : "translucent");
    }

    config.blend = pass == RenderPass::Translucent;

    for (; it != end; ++it, i += increment) {
        currentLayer = i;
        const auto& item = *it;
        if (item.bucket && item.tile) {
            if (item.layer.hasRenderPass(pass)) {
                MBGL_DEBUG_GROUP(item.layer.id + " - " + std::string(item.tile->id));
                prepareTile(*item.tile);
                item.bucket->render(*this, item.layer, item.tile->id, item.tile->matrix);
            }
        } else {
            MBGL_DEBUG_GROUP("background");
            renderBackground(item.layer);
        }
    }

    if (debug::renderTree) {
        Log::Info(Event::Render, "%*s%s", --indent * 4, "", "}");
    }
}

std::vector<RenderItem> Painter::determineRenderOrder(const Style& style) {
    std::vector<RenderItem> order;

    for (const auto& layerPtr : style.layers) {
        const auto& layer = *layerPtr;
        if (layer.bucket->visibility == VisibilityType::None) continue;
        if (layer.type == StyleLayerType::Background) {
            // This layer defines a background color/image.
            if (layer.properties.is<BackgroundPaintProperties>()) {
                auto& props = layer.properties.get<BackgroundPaintProperties>();
                if (props.image.from.empty()) {
                    // This is a solid background. We can use glClear().
                    background = props.color;
                    background[0] *= props.opacity;
                    background[1] *= props.opacity;
                    background[2] *= props.opacity;
                    background[3] *= props.opacity;
                } else {
                    // This is a textured background. We need to render it with a quad.
                    background = {{ 0, 0, 0, 0 }};
                    order.emplace_back(layer);
                }
            }
            continue;
        }

        // This is a singular layer.
        if (!layer.bucket) {
            Log::Warning(Event::Render, "layer '%s' is missing bucket", layer.id.c_str());
            continue;
        }

        Source* source = style.getSource(layer.bucket->source);
        if (!source) {
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

        const auto& tiles = source->getTiles();
        for (auto tile : tiles) {
            assert(tile);
            if (!tile->data && !tile->data->isReady()) {
                continue;
            }

            // We're not clipping symbol layers, so when we have both parents and children of symbol
            // layers, we drop all children in favor of their parent to avoid duplicate labels.
            // See https://github.com/mapbox/mapbox-gl-native/issues/2482
            if (layer.type == StyleLayerType::Symbol) {
                bool skip = false;
                // Look back through the buckets we decided to render to find out whether there is
                // already a bucket from this layer that is a parent of this tile. Tiles are ordered
                // by zoom level when we obtain them from getTiles().
                for (auto it = order.rbegin(); it != order.rend() && (&it->layer == &layer); ++it) {
                    if (tile->id.isChildOf(it->tile->id)) {
                        skip = true;
                        break;
                    }
                }
                if (skip) {
                    continue;
                }
            }

            auto bucket = tile->data->getBucket(layer);
            if (bucket) {
                order.emplace_back(layer, tile, bucket);
            }
        }
    }

    return order;
}

void Painter::renderBackground(const StyleLayer &layer_desc) {
    // Note: This function is only called for textured background. Otherwise, the background color
    // is created with glClear.
    const BackgroundPaintProperties& properties = layer_desc.getProperties<BackgroundPaintProperties>();

    if (!properties.image.to.empty()) {
        if ((properties.opacity >= 1.0f) != (pass == RenderPass::Opaque))
            return;

        SpriteAtlasPosition imagePosA = spriteAtlas->getPosition(properties.image.from, true);
        SpriteAtlasPosition imagePosB = spriteAtlas->getPosition(properties.image.to, true);
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
        vec2<double> center = state.latLngToPoint(latLng);
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

        VertexArrayObject::Unbind();
        backgroundBuffer.bind();
        patternShader->bind(0);
        spriteAtlas->bind(true);
    }

    config.stencilTest = false;
    config.depthTest = true;
    config.depthRange = { 1.0f, 1.0f };

    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
}

mat4 Painter::translatedMatrix(const mat4& matrix, const std::array<float, 2> &translation, const TileID &id, TranslateAnchorType anchor) {
    if (translation[0] == 0 && translation[1] == 0) {
        return matrix;
    } else {
        // TODO: Get rid of the 8 (scaling from 4096 to tile size)
        const double factor = ((double)(1 << id.z)) / state.getScale() * (4096.0 / util::tileSize / id.overscaling);

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

void Painter::setDepthSublayer(int n) {
    float nearDepth = ((1 + currentLayer) * numSublayers + n) * depthEpsilon;
    float farDepth = nearDepth + depthRangeSize;
    config.depthRange = { nearDepth, farDepth };
}
