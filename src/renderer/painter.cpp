#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/clip_ids.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat3.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/map/source.hpp>

#if defined(DEBUG)
#include <mbgl/util/stopwatch.hpp>
#endif

#include <cassert>
#include <algorithm>
#include <iostream>

using namespace mbgl;

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

Painter::Painter(SpriteAtlas& spriteAtlas_, GlyphAtlas& glyphAtlas_)
    : spriteAtlas(spriteAtlas_)
    , glyphAtlas(glyphAtlas_)
{
}

Painter::~Painter() {
    cleanup();
}

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(300);
}

void Painter::setup() {
#if defined(DEBUG)
    util::stopwatch stopwatch("painter setup");
#endif
    setupShaders();

    assert(iconShader);
    assert(plainShader);
    assert(outlineShader);
    assert(lineShader);
    assert(linejoinShader);
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
    if (!plainShader) plainShader = std::make_unique<PlainShader>();
    if (!outlineShader) outlineShader = std::make_unique<OutlineShader>();
    if (!lineShader) lineShader = std::make_unique<LineShader>();
    if (!linejoinShader) linejoinShader = std::make_unique<LinejoinShader>();
    if (!linepatternShader) linepatternShader = std::make_unique<LinepatternShader>();
    if (!patternShader) patternShader = std::make_unique<PatternShader>();
    if (!iconShader) iconShader = std::make_unique<IconShader>();
    if (!rasterShader) rasterShader = std::make_unique<RasterShader>();
    if (!sdfGlyphShader) sdfGlyphShader = std::make_unique<SDFGlyphShader>();
    if (!sdfIconShader) sdfIconShader = std::make_unique<SDFIconShader>();
    if (!dotShader) dotShader = std::make_unique<DotShader>();
    if (!gaussianShader) gaussianShader = std::make_unique<GaussianShader>();
}

void Painter::deleteShaders() {
    plainShader = nullptr;
    outlineShader = nullptr;
    lineShader = nullptr;
    linejoinShader = nullptr;
    linepatternShader = nullptr;
    patternShader = nullptr;
    iconShader = nullptr;
    rasterShader = nullptr;
    sdfGlyphShader = nullptr;
    sdfIconShader = nullptr;
    dotShader = nullptr;
    gaussianShader = nullptr;
}

void Painter::cleanup() {
}

void Painter::terminate() {
    cleanup();
    deleteShaders();
}

void Painter::resize() {
    if (gl_viewport != state.getFramebufferDimensions()) {
        gl_viewport = state.getFramebufferDimensions();
        assert(gl_viewport[0] > 0 && gl_viewport[1] > 0);
        glViewport(0, 0, gl_viewport[0], gl_viewport[1]);
    }
}

void Painter::setDebug(bool enabled) {
    debug = enabled;
}

void Painter::useProgram(uint32_t program) {
    if (gl_program != program) {
        glUseProgram(program);
        gl_program = program;
    }
}

void Painter::lineWidth(float line_width) {
    if (gl_lineWidth != line_width) {
        glLineWidth(line_width);
        gl_lineWidth = line_width;
    }
}

void Painter::depthMask(bool value) {
    if (gl_depthMask != value) {
        glDepthMask(value ? GL_TRUE : GL_FALSE);
        gl_depthMask = value;
    }
}

void Painter::depthRange(const float near, const float far) {
    if (gl_depthRange[0] != near || gl_depthRange[1] != far) {
        glDepthRange(near, far);
        gl_depthRange = {{ near, far }};
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
    glStencilMask(0xFF);
    depthMask(true);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Painter::setOpaque() {
    if (pass != RenderPass::Opaque) {
        pass = RenderPass::Opaque;
        glDisable(GL_BLEND);
        depthMask(true);
    }
}

void Painter::setTranslucent() {
    if (pass != RenderPass::Translucent) {
        pass = RenderPass::Translucent;
        glEnable(GL_BLEND);
        depthMask(false);
    }
}

void Painter::setStrata(float value) {
    strata = value;
}

void Painter::prepareTile(const Tile& tile) {
    const GLint ref = (GLint)tile.clip.reference.to_ulong();
    const GLuint mask = (GLuint)tile.clip.mask.to_ulong();
    glStencilFunc(GL_EQUAL, ref, mask);
}

void Painter::render(const Style& style, const std::set<util::ptr<StyleSource>>& sources,
                     TransformState state_, timestamp time) {
    state = state_;

    clear();
    resize();
    changeMatrix();

    // Update all clipping IDs.
    ClipIDGenerator generator;
    for (const util::ptr<StyleSource> &source : sources) {
        generator.update(source->source->getLoadedTiles());
        source->source->updateMatrices(projMatrix, state);
    }

    drawClippingMasks(sources);

    frameHistory.record(time, state.getNormalizedZoom());

    // Actually render the layers
    if (debug::renderTree) { std::cout << "{" << std::endl; indent++; }
    renderLayers(style.layers);
    if (debug::renderTree) { std::cout << "}" << std::endl; indent--; }

    // Finalize the rendering, e.g. by calling debug render calls per tile.
    // This guarantees that we have at least one function per tile called.
    // When only rendering layers via the stylesheet, it's possible that we don't
    // ever visit a tile during rendering.
    for (const util::ptr<StyleSource> &source : sources) {
        source->source->finishRender(*this);
    }

    glFlush();
}

void Painter::renderLayers(util::ptr<StyleLayerGroup> group) {
    if (!group) {
        // Make sure that we actually do have a layer group.
        return;
    }

    // TODO: Correctly compute the number of layers recursively beforehand.
    float strata_thickness = 1.0f / (group->layers.size() + 1);

    // - FIRST PASS ------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque
    // objects first.

    if (debug::renderTree) {
        std::cout << std::string(indent++ * 4, ' ') << "OPAQUE {" << std::endl;
    }
    int i = 0;
    for (auto it = group->layers.rbegin(), end = group->layers.rend(); it != end; ++it, ++i) {
        setOpaque();
        setStrata(i * strata_thickness);
        renderLayer(*it);
    }
    if (debug::renderTree) {
        std::cout << std::string(--indent * 4, ' ') << "}" << std::endl;
    }

    // - SECOND PASS -----------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render
    // bottom-to-top.
    if (debug::renderTree) {
        std::cout << std::string(indent++ * 4, ' ') << "TRANSLUCENT {" << std::endl;
    }
    --i;
    for (auto it = group->layers.begin(), end = group->layers.end(); it != end; ++it, --i) {
        setTranslucent();
        setStrata(i * strata_thickness);
        renderLayer(*it);
    }
    if (debug::renderTree) {
        std::cout << std::string(--indent * 4, ' ') << "}" << std::endl;
    }
}

void Painter::renderLayer(util::ptr<StyleLayer> layer_desc, const Tile::ID* id, const mat4* matrix) {
    if (layer_desc->type == StyleLayerType::Background) {
        // This layer defines a background color/image.

        if (debug::renderTree) {
            std::cout << std::string(indent * 4, ' ') << "- " << layer_desc->id << " ("
                      << layer_desc->type << ")" << std::endl;
        }

        renderBackground(layer_desc);
    } else {
        // This is a singular layer.
        if (!layer_desc->bucket) {
            fprintf(stderr, "[WARNING] layer '%s' is missing bucket\n", layer_desc->id.c_str());
            return;
        }

        if (!layer_desc->bucket->style_source) {
            fprintf(stderr, "[WARNING] can't find source for layer '%s'\n", layer_desc->id.c_str());
            return;
        }

        StyleSource const& style_source = *layer_desc->bucket->style_source;

        // Skip this layer if there is no data.
        if (!style_source.source) {
            return;
        }

        // Skip this layer if it's outside the range of min/maxzoom.
        // This may occur when there /is/ a bucket created for this layer, but the min/max-zoom
        // is set to a fractional value, or value that is larger than the source maxzoom.
        const double zoom = state.getZoom();
        if (layer_desc->bucket->min_zoom > zoom ||
            layer_desc->bucket->max_zoom <= zoom) {
            return;
        }

        // Abort early if we can already deduce from the bucket type that
        // we're not going to render anything anyway during this pass.
        switch (layer_desc->type) {
            case StyleLayerType::Fill:
                if (!layer_desc->getProperties<FillProperties>().isVisible()) return;
                break;
            case StyleLayerType::Line:
                if (pass == RenderPass::Opaque) return;
                if (!layer_desc->getProperties<LineProperties>().isVisible()) return;
                break;
            case StyleLayerType::Symbol:
                if (pass == RenderPass::Opaque) return;
                if (!layer_desc->getProperties<SymbolProperties>().isVisible()) return;
                break;
            case StyleLayerType::Raster:
                if (pass == RenderPass::Opaque) return;
                if (!layer_desc->getProperties<RasterProperties>().isVisible()) return;
                break;
            default:
                break;
        }

        if (debug::renderTree) {
            std::cout << std::string(indent * 4, ' ') << "- " << layer_desc->id << " ("
                      << layer_desc->type << ")" << std::endl;
        }
        if (!id) {
            style_source.source->render(*this, layer_desc);
        } else {
            style_source.source->render(*this, layer_desc, *id, *matrix);
        }
    }
}

void Painter::renderTileLayer(const Tile& tile, util::ptr<StyleLayer> layer_desc, const mat4 &matrix) {
    assert(tile.data);
    if (tile.data->hasData(*layer_desc) || layer_desc->type == StyleLayerType::Raster) {
        gl::group group(util::sprintf<32>("render %d/%d/%d\n", tile.id.z, tile.id.y, tile.id.z));
        prepareTile(tile);
        tile.data->render(*this, layer_desc, matrix);
    }
}

void Painter::renderBackground(util::ptr<StyleLayer> layer_desc) {
    const BackgroundProperties& properties = layer_desc->getProperties<BackgroundProperties>();

    if (properties.image.size()) {
        if ((properties.opacity >= 1.0f) != (pass == RenderPass::Opaque))
            return;

        SpriteAtlasPosition imagePos = spriteAtlas.getPosition(properties.image, true);
        float zoomFraction = state.getZoomFraction();

        useProgram(patternShader->program);
        patternShader->u_matrix = identityMatrix;
        patternShader->u_pattern_tl = imagePos.tl;
        patternShader->u_pattern_br = imagePos.br;
        patternShader->u_mix = zoomFraction;
        patternShader->u_opacity = properties.opacity;

        std::array<float, 2> size = imagePos.size;
        double lon, lat;
        state.getLonLat(lon, lat);
        std::array<float, 2> center = state.locationCoordinate(lon, lat);
        float scale = 1 / std::pow(2, zoomFraction);

        mat3 matrix;
        matrix::identity(matrix);
        matrix::scale(matrix, matrix,
                      1.0f / size[0],
                      1.0f / size[1]);
        matrix::translate(matrix, matrix,
                          std::fmod(center[0] * 512, size[0]),
                          std::fmod(center[1] * 512, size[1]));
        matrix::rotate(matrix, matrix, -state.getAngle());
        matrix::scale(matrix, matrix,
                       scale * state.getWidth()  / 2,
                      -scale * state.getHeight() / 2);
        patternShader->u_patternmatrix = matrix;

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

    glDisable(GL_STENCIL_TEST);
    depthRange(strata + strata_epsilon, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEnable(GL_STENCIL_TEST);
}

mat4 Painter::translatedMatrix(const mat4& matrix, const std::array<float, 2> &translation, const Tile::ID &id, TranslateAnchorType anchor) {
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
