#include <llmr/renderer/painter.hpp>
#include <llmr/map/map.hpp>
#include <llmr/util/std.hpp>
#include <llmr/util/string.hpp>
#include <llmr/util/time.hpp>
#include <llmr/util/clip_ids.hpp>
#include <llmr/util/constants.hpp>
#if defined(DEBUG)
#include <llmr/util/timer.hpp>
#endif

#include <cassert>
#include <algorithm>

using namespace llmr;

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

Painter::Painter(Map &map)
    : map(map) {
}

Painter::~Painter() {
    cleanup();
}

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(300);
}

void Painter::setup() {
#if defined(DEBUG)
    util::timer timer("painter setup");
#endif
    setupShaders();

    assert(iconShader);
    assert(plainShader);
    assert(outlineShader);
    assert(lineShader);
    assert(linejoinShader);
    assert(patternShader);
    assert(rasterShader);
    assert(textShader);
    assert(dotShader);
    assert(compositeShader);
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
    plainShader = std::make_unique<PlainShader>();
    outlineShader = std::make_unique<OutlineShader>();
    lineShader = std::make_unique<LineShader>();
    linejoinShader = std::make_unique<LinejoinShader>();
    patternShader = std::make_unique<PatternShader>();
    iconShader = std::make_unique<IconShader>();
    rasterShader = std::make_unique<RasterShader>();
    textShader = std::make_unique<TextShader>();
    dotShader = std::make_unique<DotShader>();
    compositeShader = std::make_unique<CompositeShader>();
    gaussianShader = std::make_unique<GaussianShader>();
}

void Painter::cleanup() {
    clearFramebuffers();
}

void Painter::resize() {
    const TransformState &state = map.getState();
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
    matrix::ortho(projMatrix, 0, map.getState().getWidth(), map.getState().getHeight(), 0, 0, 1);

    // The extrusion matrix.
    matrix::identity(extrudeMatrix);
    matrix::multiply(extrudeMatrix, projMatrix, extrudeMatrix);
    matrix::rotate_z(extrudeMatrix, extrudeMatrix, map.getState().getAngle());

    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    matrix::identity(nativeMatrix);
    matrix::multiply(nativeMatrix, projMatrix, nativeMatrix);
}

void Painter::clear() {
    gl::group group("clear");
    glStencilMask(0xFF);
    depthMask(true);

    std::shared_ptr<StyleLayer> background_layer = map.getStyle()->background;
    const BackgroundProperties &properties = background_layer ? background_layer->getProperties<BackgroundProperties>() : defaultStyleProperties<BackgroundProperties>();
    glClearColor(properties.color[0], properties.color[1], properties.color[2], properties.color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Painter::setOpaque() {
    if (pass != Opaque) {
        pass = Opaque;
        glDisable(GL_BLEND);
        depthMask(true);
    }
}

void Painter::setTranslucent() {
    if (pass != Translucent) {
        pass = Translucent;
        glEnable(GL_BLEND);
        depthMask(false);
    }
}

void Painter::setStrata(float value) {
    strata = value;
}

void Painter::prepareTile(const Tile& tile) {
    matrix = tile.matrix;

    GLint id = (GLint)tile.clip.mask.to_ulong();
    GLuint mask = clipMask[tile.clip.length];
    glStencilFunc(GL_EQUAL, id, mask);
}

void Painter::renderTileLayer(const Tile& tile, std::shared_ptr<StyleLayer> layer_desc) {
    assert(tile.data);
    if (tile.data->hasData(layer_desc)) {
        gl::group group(util::sprintf<32>("render %d/%d/%d\n", tile.id.z, tile.id.y, tile.id.z));
        prepareTile(tile);
        tile.data->render(*this, layer_desc);
        frameHistory.record(map.getAnimationTime(), map.getState().getNormalizedZoom());
    }
}


const mat4 &Painter::translatedMatrix(const std::array<float, 2> &translation, const Tile::ID &id, TranslateAnchorType anchor) {
    if (translation[0] == 0 && translation[1] == 0) {
        return matrix;
    } else {
        // TODO: Get rid of the 8 (scaling from 4096 to tile size)
        const double factor = ((double)(1 << id.z)) / map.getState().getScale() * (4096.0 / util::tileSize);

        if (anchor == TranslateAnchorType::Viewport) {
            const double sin_a = std::sin(-map.getState().getAngle());
            const double cos_a = std::cos(-map.getState().getAngle());
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

void Painter::renderMatte() {
    gl::group group("matte");
    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_EQUAL, 0x0, 0xFF);

    Color matte = {{ 0, 0, 0, 1 }};

    useProgram(plainShader->program);
    plainShader->setMatrix(nativeMatrix);

    // Draw the clipping mask
    matteArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(matte);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());

    glEnable(GL_DEPTH_TEST);
}
