#include <llmr/renderer/painter.hpp>

#include <llmr/map/map.hpp>

#include <llmr/util/std.hpp>
#include <llmr/util/string.hpp>
#include <llmr/util/timer.hpp>
#include <llmr/util/time.hpp>
#include <llmr/util/clip_ids.hpp>
#include <llmr/util/constants.hpp>

#include <cassert>
#include <algorithm>

using namespace llmr;

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

Painter::Painter(Map &map)
    : map(map) {
}

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(300);
}

void Painter::setup() {
    util::timer timer("painter setup");

    setupShaders();

    assert(pointShader);
    assert(plainShader);
    assert(outlineShader);
    assert(lineShader);
    assert(linejoinShader);
    assert(patternShader);
    assert(rasterShader);
    assert(textShader);
    assert(dotShader);


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
    dotShader = std::make_unique<DotShader>();
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
#ifdef NVIDIA
    // We're painting in a way that allows us to skip clearing the color buffer.
    // On Tegra hardware, this is faster.
    glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
}

void Painter::startOpaquePass() {
    gl::start_group("opaque pass");
    pass = Opaque;
    glDisable(GL_BLEND);
    depthMask(true);
}

void Painter::startTranslucentPass() {
    gl::start_group("translucent pass");
    pass = Translucent;
    glEnable(GL_BLEND);
    depthMask(false);
}

void Painter::endPass() {
    gl::end_group();
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

void Painter::renderTileLayer(const Tile& tile, const LayerDescription &layer_desc) {
    assert(tile.data);
    if (tile.data->hasData(layer_desc)) {
        gl::group group(util::sprintf<32>("render %d/%d/%d", tile.id.z, tile.id.y, tile.id.z));
        prepareTile(tile);
        tile.data->render(*this, layer_desc);
        frameHistory.record(map.getAnimationTime(), map.getState().getNormalizedZoom());
    }
}

void Painter::translateLayer(const std::array<float, 2> translation, const Tile::ID &id, bool reverse) {
    if (translation[0] || translation[1]) {
        const int direction = reverse ? -1 : 1;
        // TODO: Get rid of the 8 (scaling from 4096 to tile size)
        const double factor = direction * ((double)(1 << id.z)) / map.getState().getScale() /
                              map.getState().getPixelRatio() * (4096.0 / util::tileSize);
        matrix::translate(matrix, matrix, translation[0] * factor, translation[1] * factor, 0);
    }
}

void Painter::renderMatte() {
    gl::group group("matte");
    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_EQUAL, 0x0, 0xFF);

    // Color white = {{ 0.9, 0.9, 0.9, 1 }};
    Color white = {{ 1, 1, 0, 1 }};

    useProgram(plainShader->program);
    plainShader->setMatrix(nativeMatrix);

    // Draw the clipping mask
    matteArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(white);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());

    glEnable(GL_DEPTH_TEST);
}
