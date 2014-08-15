#include <mbgl/renderer/painter.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/clip_ids.hpp>
#include <mbgl/util/constants.hpp>
#if defined(DEBUG)
#include <mbgl/util/timer.hpp>
#endif

#include <cassert>
#include <algorithm>

using namespace mbgl;

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
    if (!patternShader) patternShader = std::make_unique<PatternShader>();
    if (!iconShader) iconShader = std::make_unique<IconShader>();
    if (!rasterShader) rasterShader = std::make_unique<RasterShader>();
    if (!textShader) textShader = std::make_unique<TextShader>();
    if (!dotShader) dotShader = std::make_unique<DotShader>();
    if (!gaussianShader) gaussianShader = std::make_unique<GaussianShader>();
}

void Painter::cleanup() {
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

void Painter::depthRange(const float near, const float far) {
    if (gl_depthRange[0] != near || gl_depthRange[1] != far) {
        glDepthRange(near, far);
        gl_depthRange = {{ near, far }};
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

    const BackgroundProperties &properties = map.getStyle()->getBackgroundProperties();
    glClearColor(properties.color[0], properties.color[1], properties.color[2], properties.color[3]);
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
    GLint id = (GLint)tile.clip.mask.to_ulong();
    GLuint mask = clipMask[tile.clip.length];
    glStencilFunc(GL_EQUAL, id, mask);
}

void Painter::renderTileLayer(const Tile& tile, std::shared_ptr<StyleLayer> layer_desc, const mat4 &matrix) {
    assert(tile.data);
    if (tile.data->hasData(layer_desc) || layer_desc->type == StyleLayerType::Raster) {
        gl::group group(util::sprintf<32>("render %d/%d/%d\n", tile.id.z, tile.id.y, tile.id.z));
        prepareTile(tile);
        tile.data->render(*this, layer_desc, matrix);
        frameHistory.record(map.getAnimationTime(), map.getState().getNormalizedZoom());
    }
}


const mat4 &Painter::translatedMatrix(const mat4& matrix, const std::array<float, 2> &translation, const Tile::ID &id, TranslateAnchorType anchor) {
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
