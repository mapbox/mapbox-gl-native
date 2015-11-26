#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/layer/line_layer.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/util/mat2.hpp>

using namespace mbgl;

void Painter::renderLine(LineBucket& bucket, const LineLayer& layer, const TileID& id, const mat4& matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;

    config.stencilOp.reset();
    config.stencilTest = GL_TRUE;
    config.depthFunc.reset();
    config.depthTest = GL_TRUE;
    config.depthMask = GL_FALSE;

    const auto& properties = layer.paint;
    const auto& layout = bucket.layout;

    // the distance over which the line edge fades out.
    // Retina devices need a smaller distance to avoid aliasing.
    float antialiasing = 1.0 / data.pixelRatio;

    float blur = properties.blur + antialiasing;
    float edgeWidth = properties.width / 2.0;
    float inset = -1;
    float offset = 0;
    float shift = 0;

    if (properties.gapWidth != 0) {
        inset = properties.gapWidth / 2.0 + antialiasing * 0.5;
        edgeWidth = properties.width;

        // shift outer lines half a pixel towards the middle to eliminate the crack
        offset = inset - antialiasing / 2.0;
    }

    float outset = offset + edgeWidth + antialiasing / 2.0 + shift;

    Color color = properties.color;
    color[0] *= properties.opacity;
    color[1] *= properties.opacity;
    color[2] *= properties.opacity;
    color[3] *= properties.opacity;

    float ratio = state.getScale() / std::pow(2, id.z) / 8.0 * id.overscaling;

    mat2 antialiasingMatrix;
    matrix::identity(antialiasingMatrix);
    matrix::scale(antialiasingMatrix, antialiasingMatrix, 1.0, std::cos(state.getPitch()));
    matrix::rotate(antialiasingMatrix, antialiasingMatrix, state.getAngle());

    // calculate how much longer the real world distance is at the top of the screen
    // than at the middle of the screen.
    float topedgelength = std::sqrt(std::pow(state.getHeight(), 2) / 4  * (1 + std::pow(state.getAltitude(), 2)));
    float x = state.getHeight() / 2.0f * std::tan(state.getPitch());
    float extra = (topedgelength + x) / topedgelength - 1;

    mat4 vtxMatrix = translatedMatrix(matrix, properties.translate, id, properties.translateAnchor);

    setDepthSublayer(0);

    if (!properties.dasharray.value.from.empty()) {

        config.program = linesdfShader->program;

        linesdfShader->u_matrix = vtxMatrix;
        linesdfShader->u_exmatrix = extrudeMatrix;
        linesdfShader->u_linewidth = {{ outset, inset }};
        linesdfShader->u_ratio = ratio;
        linesdfShader->u_blur = blur;
        linesdfShader->u_color = color;

        LinePatternPos posA = lineAtlas->getDashPosition(properties.dasharray.value.from, layout.cap == CapType::Round);
        LinePatternPos posB = lineAtlas->getDashPosition(properties.dasharray.value.to, layout.cap == CapType::Round);
        lineAtlas->bind();

        float patternratio = std::pow(2.0, std::floor(::log2(state.getScale())) - id.z) / 8.0 * id.overscaling;
        float scaleXA = patternratio / posA.width / properties.dashLineWidth / properties.dasharray.value.fromScale;
        float scaleYA = -posA.height / 2.0;
        float scaleXB = patternratio / posB.width / properties.dashLineWidth / properties.dasharray.value.toScale;
        float scaleYB = -posB.height / 2.0;

        linesdfShader->u_patternscale_a = {{ scaleXA, scaleYA }};
        linesdfShader->u_tex_y_a = posA.y;
        linesdfShader->u_patternscale_b = {{ scaleXB, scaleYB }};
        linesdfShader->u_tex_y_b = posB.y;
        linesdfShader->u_image = 0;
        linesdfShader->u_sdfgamma = lineAtlas->width / (properties.dashLineWidth * std::min(posA.width, posB.width) * 256.0 * data.pixelRatio) / 2;
        linesdfShader->u_mix = properties.dasharray.value.t;
        linesdfShader->u_extra = extra;
        linesdfShader->u_antialiasingmatrix = antialiasingMatrix;

        bucket.drawLineSDF(*linesdfShader);

    } else if (!properties.pattern.value.from.empty()) {
        SpriteAtlasPosition imagePosA = spriteAtlas->getPosition(properties.pattern.value.from, true);
        SpriteAtlasPosition imagePosB = spriteAtlas->getPosition(properties.pattern.value.to, true);

        float factor = 8.0 / std::pow(2, state.getIntegerZoom() - id.z) * id.overscaling;

        config.program = linepatternShader->program;

        linepatternShader->u_matrix = vtxMatrix;
        linepatternShader->u_exmatrix = extrudeMatrix;
        linepatternShader->u_linewidth = {{ outset, inset }};
        linepatternShader->u_ratio = ratio;
        linepatternShader->u_blur = blur;

        linepatternShader->u_pattern_size_a = {{imagePosA.size[0] * factor * properties.pattern.value.fromScale, imagePosA.size[1]}};
        linepatternShader->u_pattern_tl_a = imagePosA.tl;
        linepatternShader->u_pattern_br_a = imagePosA.br;
        linepatternShader->u_pattern_size_b = {{imagePosB.size[0] * factor * properties.pattern.value.toScale, imagePosB.size[1]}};
        linepatternShader->u_pattern_tl_b = imagePosB.tl;
        linepatternShader->u_pattern_br_b = imagePosB.br;
        linepatternShader->u_fade = properties.pattern.value.t;
        linepatternShader->u_opacity = properties.opacity;
        linepatternShader->u_extra = extra;
        linepatternShader->u_antialiasingmatrix = antialiasingMatrix;

        MBGL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0));
        spriteAtlas->bind(true);

        bucket.drawLinePatterns(*linepatternShader);

    } else {
        config.program = lineShader->program;

        lineShader->u_matrix = vtxMatrix;
        lineShader->u_exmatrix = extrudeMatrix;
        lineShader->u_linewidth = {{ outset, inset }};
        lineShader->u_ratio = ratio;
        lineShader->u_blur = blur;
        lineShader->u_extra = extra;
        lineShader->u_antialiasingmatrix = antialiasingMatrix;

        lineShader->u_color = color;

        bucket.drawLines(*lineShader);
    }
}
