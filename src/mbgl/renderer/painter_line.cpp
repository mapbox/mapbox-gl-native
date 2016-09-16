#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/util/mat2.hpp>

namespace mbgl {

using namespace style;

void Painter::renderLine(PaintParameters& parameters,
                         LineBucket& bucket,
                         const LineLayer& layer,
                         const RenderTile& tile) {
    // Abort early.
    if (pass == RenderPass::Opaque)
        return;

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
    const float ratio = 1.0 / tile.id.pixelsToTileUnits(1.0, state.getZoom());

    mat2 antialiasingMatrix;
    matrix::identity(antialiasingMatrix);
    matrix::scale(antialiasingMatrix, antialiasingMatrix, 1.0, std::cos(state.getPitch()));
    matrix::rotate(antialiasingMatrix, antialiasingMatrix, state.getAngle());

    // calculate how much longer the real world distance is at the top of the screen
    // than at the middle of the screen.
    float topedgelength = std::sqrt(std::pow(state.getHeight(), 2.0f) / 4.0f *
                                    (1.0f + std::pow(state.getAltitude(), 2.0f)));
    float x = state.getHeight() / 2.0f * std::tan(state.getPitch());
    float extra = (topedgelength + x) / topedgelength - 1.0f;

    mat4 vtxMatrix =
        tile.translatedMatrix(properties.lineTranslate, properties.lineTranslateAnchor, state);

    setDepthSublayer(0);

    auto& linesdfShader = parameters.shaders.linesdf;
    auto& linepatternShader = parameters.shaders.linepattern;
    auto& lineShader = parameters.shaders.line;

    if (!properties.lineDasharray.value.from.empty()) {
        config.program = linesdfShader.getID();

        linesdfShader.u_matrix = vtxMatrix;
        linesdfShader.u_linewidth = properties.lineWidth / 2;
        linesdfShader.u_gapwidth = properties.lineGapWidth / 2;
        linesdfShader.u_antialiasing = antialiasing / 2;
        linesdfShader.u_ratio = ratio;
        linesdfShader.u_blur = blur;
        linesdfShader.u_color = color;
        linesdfShader.u_opacity = opacity;

        const LinePatternCap cap =
            layout.lineCap == LineCapType::Round ? LinePatternCap::Round : LinePatternCap::Square;
        LinePatternPos posA = lineAtlas->getDashPosition(properties.lineDasharray.value.from, cap);
        LinePatternPos posB = lineAtlas->getDashPosition(properties.lineDasharray.value.to, cap);

        const float widthA =
            posA.width * properties.lineDasharray.value.fromScale * layer.impl->dashLineWidth;
        const float widthB =
            posB.width * properties.lineDasharray.value.toScale * layer.impl->dashLineWidth;

        float scaleXA = 1.0 / tile.id.pixelsToTileUnits(widthA, state.getIntegerZoom());
        float scaleYA = -posA.height / 2.0;
        float scaleXB = 1.0 / tile.id.pixelsToTileUnits(widthB, state.getIntegerZoom());
        float scaleYB = -posB.height / 2.0;

        linesdfShader.u_patternscale_a = { { scaleXA, scaleYA } };
        linesdfShader.u_tex_y_a = posA.y;
        linesdfShader.u_patternscale_b = { { scaleXB, scaleYB } };
        linesdfShader.u_tex_y_b = posB.y;
        linesdfShader.u_sdfgamma =
            lineAtlas->width / (std::min(widthA, widthB) * 256.0 * frame.pixelRatio) / 2;
        linesdfShader.u_mix = properties.lineDasharray.value.t;
        linesdfShader.u_extra = extra;
        linesdfShader.u_offset = -properties.lineOffset;
        linesdfShader.u_antialiasingmatrix = antialiasingMatrix;

        linesdfShader.u_image = 0;
        lineAtlas->bind(store, config, 0);

        bucket.drawLineSDF(linesdfShader, store, paintMode());

    } else if (!properties.linePattern.value.from.empty()) {
        optional<SpriteAtlasPosition> imagePosA = spriteAtlas->getPosition(
            properties.linePattern.value.from, SpritePatternMode::Repeating);
        optional<SpriteAtlasPosition> imagePosB =
            spriteAtlas->getPosition(properties.linePattern.value.to, SpritePatternMode::Repeating);

        if (!imagePosA || !imagePosB)
            return;

        config.program = linepatternShader.getID();

        linepatternShader.u_matrix = vtxMatrix;
        linepatternShader.u_linewidth = properties.lineWidth / 2;
        linepatternShader.u_gapwidth = properties.lineGapWidth / 2;
        linepatternShader.u_antialiasing = antialiasing / 2;
        linepatternShader.u_ratio = ratio;
        linepatternShader.u_blur = blur;

        linepatternShader.u_pattern_size_a = { { tile.id.pixelsToTileUnits(
                                                     (*imagePosA).size[0] *
                                                         properties.linePattern.value.fromScale,
                                                     state.getIntegerZoom()),
                                                 (*imagePosA).size[1] } };
        linepatternShader.u_pattern_tl_a = (*imagePosA).tl;
        linepatternShader.u_pattern_br_a = (*imagePosA).br;

        linepatternShader.u_pattern_size_b = {
            { tile.id.pixelsToTileUnits((*imagePosB).size[0] * properties.linePattern.value.toScale,
                                        state.getIntegerZoom()),
              (*imagePosB).size[1] }
        };
        linepatternShader.u_pattern_tl_b = (*imagePosB).tl;
        linepatternShader.u_pattern_br_b = (*imagePosB).br;

        linepatternShader.u_fade = properties.linePattern.value.t;
        linepatternShader.u_opacity = properties.lineOpacity;
        linepatternShader.u_extra = extra;
        linepatternShader.u_offset = -properties.lineOffset;
        linepatternShader.u_antialiasingmatrix = antialiasingMatrix;

        linepatternShader.u_image = 0;
        spriteAtlas->bind(true, store, config, 0);

        bucket.drawLinePatterns(linepatternShader, store, paintMode());

    } else {
        config.program = lineShader.getID();

        lineShader.u_matrix = vtxMatrix;
        lineShader.u_linewidth = properties.lineWidth / 2;
        lineShader.u_gapwidth = properties.lineGapWidth / 2;
        lineShader.u_antialiasing = antialiasing / 2;
        lineShader.u_ratio = ratio;
        lineShader.u_blur = blur;
        lineShader.u_extra = extra;
        lineShader.u_offset = -properties.lineOffset;
        lineShader.u_antialiasingmatrix = antialiasingMatrix;

        lineShader.u_color = color;
        lineShader.u_opacity = opacity;

        bucket.drawLines(lineShader, store, paintMode());
    }
}

} // namespace mbgl
