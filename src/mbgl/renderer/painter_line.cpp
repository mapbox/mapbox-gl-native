#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/util/mat2.hpp>

namespace mbgl {

using namespace style;

void Painter::renderLine(LineBucket& bucket,
                         const LineLayer& layer,
                         const UnwrappedTileID& tileID,
                         const mat4& matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;

    config.stencilOp.reset();
    config.stencilTest = GL_TRUE;
    config.depthFunc.reset();
    config.depthTest = GL_TRUE;
    config.depthMask = GL_FALSE;

    const auto& properties = layer.impl->paint;
    const auto& layout = bucket.layout;

    // the distance over which the line edge fades out.
    // Retina devices need a smaller distance to avoid aliasing.
    float antialiasing = 1.0 / frame.pixelRatio;

    float blur = properties.lineBlur + antialiasing;

    const Color color = properties.lineColor;
    const float opacity = properties.lineOpacity;
    const float ratio = 1.0 / tileID.pixelsToTileUnits(1.0, state.getZoom());

    mat2 antialiasingMatrix;
    matrix::identity(antialiasingMatrix);
    matrix::scale(antialiasingMatrix, antialiasingMatrix, 1.0, std::cos(state.getPitch()));
    matrix::rotate(antialiasingMatrix, antialiasingMatrix, state.getAngle());

    // calculate how much longer the real world distance is at the top of the screen
    // than at the middle of the screen.
    float topedgelength = std::sqrt(std::pow(state.getHeight(), 2.0f) / 4.0f  * (1.0f + std::pow(state.getAltitude(), 2.0f)));
    float x = state.getHeight() / 2.0f * std::tan(state.getPitch());
    float extra = (topedgelength + x) / topedgelength - 1.0f;

    mat4 vtxMatrix =
        translatedMatrix(matrix, properties.lineTranslate, tileID, properties.lineTranslateAnchor);

    setDepthSublayer(0);

    const bool overdraw = isOverdraw();
    const auto& shaderLineSDF = overdraw ? linesdfOverdrawShader : linesdfShader;
    const auto& shaderLinePattern = overdraw ? linepatternOverdrawShader : linepatternShader;
    const auto& shaderLine = overdraw ? lineOverdrawShader : lineShader;

    if (!properties.lineDasharray.value.from.empty()) {
        config.program = shaderLineSDF->getID();

        shaderLineSDF->u_matrix = vtxMatrix;
        shaderLineSDF->u_linewidth = properties.lineWidth / 2;
        shaderLineSDF->u_gapwidth = properties.lineGapWidth / 2;
        shaderLineSDF->u_antialiasing = antialiasing / 2;
        shaderLineSDF->u_ratio = ratio;
        shaderLineSDF->u_blur = blur;
        shaderLineSDF->u_color = color;
        shaderLineSDF->u_opacity = opacity;

        LinePatternPos posA = lineAtlas->getDashPosition(properties.lineDasharray.value.from, layout.lineCap == LineCapType::Round, store);
        LinePatternPos posB = lineAtlas->getDashPosition(properties.lineDasharray.value.to, layout.lineCap == LineCapType::Round, store);

        const float widthA = posA.width * properties.lineDasharray.value.fromScale * layer.impl->dashLineWidth;
        const float widthB = posB.width * properties.lineDasharray.value.toScale * layer.impl->dashLineWidth;

        float scaleXA = 1.0 / tileID.pixelsToTileUnits(widthA, state.getIntegerZoom());
        float scaleYA = -posA.height / 2.0;
        float scaleXB = 1.0 / tileID.pixelsToTileUnits(widthB, state.getIntegerZoom());
        float scaleYB = -posB.height / 2.0;

        shaderLineSDF->u_patternscale_a = {{ scaleXA, scaleYA }};
        shaderLineSDF->u_tex_y_a = posA.y;
        shaderLineSDF->u_patternscale_b = {{ scaleXB, scaleYB }};
        shaderLineSDF->u_tex_y_b = posB.y;
        shaderLineSDF->u_sdfgamma = lineAtlas->width / (std::min(widthA, widthB) * 256.0 * frame.pixelRatio) / 2;
        shaderLineSDF->u_mix = properties.lineDasharray.value.t;
        shaderLineSDF->u_extra = extra;
        shaderLineSDF->u_offset = -properties.lineOffset;
        shaderLineSDF->u_antialiasingmatrix = antialiasingMatrix;

        shaderLineSDF->u_image = 0;
        config.activeTexture = GL_TEXTURE0;
        lineAtlas->bind(store);

        bucket.drawLineSDF(*shaderLineSDF, store, overdraw);

    } else if (!properties.linePattern.value.from.empty()) {
        optional<SpriteAtlasPosition> imagePosA = spriteAtlas->getPosition(properties.linePattern.value.from, true);
        optional<SpriteAtlasPosition> imagePosB = spriteAtlas->getPosition(properties.linePattern.value.to, true);

        if (!imagePosA || !imagePosB)
            return;

        config.program = shaderLinePattern->getID();

        shaderLinePattern->u_matrix = vtxMatrix;
        shaderLinePattern->u_linewidth = properties.lineWidth / 2;
        shaderLinePattern->u_gapwidth = properties.lineGapWidth / 2;
        shaderLinePattern->u_antialiasing = antialiasing / 2;
        shaderLinePattern->u_ratio = ratio;
        shaderLinePattern->u_blur = blur;

        shaderLinePattern->u_pattern_size_a = {{
            tileID.pixelsToTileUnits((*imagePosA).size[0] * properties.linePattern.value.fromScale, state.getIntegerZoom()),
            (*imagePosA).size[1]
        }};
        shaderLinePattern->u_pattern_tl_a = (*imagePosA).tl;
        shaderLinePattern->u_pattern_br_a = (*imagePosA).br;

        shaderLinePattern->u_pattern_size_b = {{
            tileID.pixelsToTileUnits((*imagePosB).size[0] * properties.linePattern.value.toScale, state.getIntegerZoom()),
            (*imagePosB).size[1]
        }};
        shaderLinePattern->u_pattern_tl_b = (*imagePosB).tl;
        shaderLinePattern->u_pattern_br_b = (*imagePosB).br;

        shaderLinePattern->u_fade = properties.linePattern.value.t;
        shaderLinePattern->u_opacity = properties.lineOpacity;
        shaderLinePattern->u_extra = extra;
        shaderLinePattern->u_offset = -properties.lineOffset;
        shaderLinePattern->u_antialiasingmatrix = antialiasingMatrix;

        shaderLinePattern->u_image = 0;
        config.activeTexture = GL_TEXTURE0;
        spriteAtlas->bind(true, store);

        bucket.drawLinePatterns(*shaderLinePattern, store, overdraw);

    } else {
        config.program = shaderLine->getID();

        shaderLine->u_matrix = vtxMatrix;
        shaderLine->u_linewidth = properties.lineWidth / 2;
        shaderLine->u_gapwidth = properties.lineGapWidth / 2;
        shaderLine->u_antialiasing = antialiasing / 2;
        shaderLine->u_ratio = ratio;
        shaderLine->u_blur = blur;
        shaderLine->u_extra = extra;
        shaderLine->u_offset = -properties.lineOffset;
        shaderLine->u_antialiasingmatrix = antialiasingMatrix;

        shaderLine->u_color = color;
        shaderLine->u_opacity = opacity;

        bucket.drawLines(*shaderLine, store, overdraw);
    }
}

} // namespace mbgl
