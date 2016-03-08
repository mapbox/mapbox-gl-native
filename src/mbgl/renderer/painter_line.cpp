#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/layer/line_layer.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/util/mat2.hpp>

using namespace mbgl;

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

    const auto& properties = layer.paint;
    const auto& layout = bucket.layout;

    // the distance over which the line edge fades out.
    // Retina devices need a smaller distance to avoid aliasing.
    float antialiasing = 1.0 / frame.pixelRatio;

    float blur = properties.lineBlur + antialiasing;
    float edgeWidth = properties.lineWidth / 2.0;
    float inset = -1;
    float offset = 0;
    float shift = 0;

    if (properties.lineGapWidth != 0) {
        inset = properties.lineGapWidth / 2.0 + antialiasing * 0.5;
        edgeWidth = properties.lineWidth;

        // shift outer lines half a pixel towards the middle to eliminate the crack
        offset = inset - antialiasing / 2.0;
    }

    float outset = offset + edgeWidth + antialiasing / 2.0 + shift;

    Color color = properties.lineColor;
    color[0] *= properties.lineOpacity;
    color[1] *= properties.lineOpacity;
    color[2] *= properties.lineOpacity;
    color[3] *= properties.lineOpacity;

    const float ratio = 1.0 / tileID.pixelsToTileUnits(1.0, state.getZoom());

    mat2 antialiasingMatrix;
    matrix::identity(antialiasingMatrix);
    matrix::scale(antialiasingMatrix, antialiasingMatrix, 1.0, std::cos(state.getPitch()));
    matrix::rotate(antialiasingMatrix, antialiasingMatrix, state.getAngle());

    // calculate how much longer the real world distance is at the top of the screen
    // than at the middle of the screen.
    float topedgelength = std::sqrt(std::pow(state.getHeight(), 2) / 4  * (1 + std::pow(state.getAltitude(), 2)));
    float x = state.getHeight() / 2.0f * std::tan(state.getPitch());
    float extra = (topedgelength + x) / topedgelength - 1;

    mat4 vtxMatrix =
        translatedMatrix(matrix, properties.lineTranslate, tileID, properties.lineTranslateAnchor);

    setDepthSublayer(0);

    if (!properties.lineDasharray.value.from.empty()) {

        config.program = linesdfShader->getID();

        linesdfShader->u_matrix = vtxMatrix;
        linesdfShader->u_exmatrix = extrudeMatrix;
        linesdfShader->u_linewidth = {{ outset, inset }};
        linesdfShader->u_ratio = ratio;
        linesdfShader->u_blur = blur;
        linesdfShader->u_color = color;

        LinePatternPos posA = lineAtlas->getDashPosition(properties.lineDasharray.value.from, layout.lineCap == LineCapType::Round, glObjectStore);
        LinePatternPos posB = lineAtlas->getDashPosition(properties.lineDasharray.value.to, layout.lineCap == LineCapType::Round, glObjectStore);

        const float widthA = posA.width * properties.lineDasharray.value.fromScale * layer.dashLineWidth;
        const float widthB = posB.width * properties.lineDasharray.value.toScale * layer.dashLineWidth;

        float scaleXA = 1.0 / tileID.pixelsToTileUnits(widthA, state.getIntegerZoom());
        float scaleYA = -posA.height / 2.0;
        float scaleXB = 1.0 / tileID.pixelsToTileUnits(widthB, state.getIntegerZoom());
        float scaleYB = -posB.height / 2.0;

        linesdfShader->u_patternscale_a = {{ scaleXA, scaleYA }};
        linesdfShader->u_tex_y_a = posA.y;
        linesdfShader->u_patternscale_b = {{ scaleXB, scaleYB }};
        linesdfShader->u_tex_y_b = posB.y;
        linesdfShader->u_sdfgamma = lineAtlas->width / (std::min(widthA, widthB) * 256.0 * frame.pixelRatio) / 2;
        linesdfShader->u_mix = properties.lineDasharray.value.t;
        linesdfShader->u_extra = extra;
        linesdfShader->u_offset = -properties.lineOffset;
        linesdfShader->u_antialiasingmatrix = antialiasingMatrix;

        linesdfShader->u_image = 0;
        config.activeTexture = GL_TEXTURE0;
        lineAtlas->bind(glObjectStore);

        bucket.drawLineSDF(*linesdfShader, glObjectStore);

    } else if (!properties.linePattern.value.from.empty()) {
        optional<SpriteAtlasPosition> imagePosA = spriteAtlas->getPosition(properties.linePattern.value.from, true);
        optional<SpriteAtlasPosition> imagePosB = spriteAtlas->getPosition(properties.linePattern.value.to, true);
        
        if (!imagePosA || !imagePosB)
            return;

        config.program = linepatternShader->getID();

        linepatternShader->u_matrix = vtxMatrix;
        linepatternShader->u_exmatrix = extrudeMatrix;
        linepatternShader->u_linewidth = {{ outset, inset }};
        linepatternShader->u_ratio = ratio;
        linepatternShader->u_blur = blur;

        linepatternShader->u_pattern_size_a = {{
            tileID.pixelsToTileUnits((*imagePosA).size[0] * properties.linePattern.value.fromScale, state.getIntegerZoom()),
            (*imagePosA).size[1]
        }};
        linepatternShader->u_pattern_tl_a = (*imagePosA).tl;
        linepatternShader->u_pattern_br_a = (*imagePosA).br;

        linepatternShader->u_pattern_size_b = {{
            tileID.pixelsToTileUnits((*imagePosB).size[0] * properties.linePattern.value.toScale, state.getIntegerZoom()),
            (*imagePosB).size[1]
        }};
        linepatternShader->u_pattern_tl_b = (*imagePosB).tl;
        linepatternShader->u_pattern_br_b = (*imagePosB).br;

        linepatternShader->u_fade = properties.linePattern.value.t;
        linepatternShader->u_opacity = properties.lineOpacity;
        linepatternShader->u_extra = extra;
        linepatternShader->u_offset = -properties.lineOffset;
        linepatternShader->u_antialiasingmatrix = antialiasingMatrix;

        linepatternShader->u_image = 0;
        config.activeTexture = GL_TEXTURE0;
        spriteAtlas->bind(true, glObjectStore);

        bucket.drawLinePatterns(*linepatternShader, glObjectStore);

    } else {
        config.program = lineShader->getID();

        lineShader->u_matrix = vtxMatrix;
        lineShader->u_exmatrix = extrudeMatrix;
        lineShader->u_linewidth = {{ outset, inset }};
        lineShader->u_ratio = ratio;
        lineShader->u_blur = blur;
        lineShader->u_extra = extra;
        lineShader->u_offset = -properties.lineOffset;
        lineShader->u_antialiasingmatrix = antialiasingMatrix;

        lineShader->u_color = color;

        bucket.drawLines(*lineShader, glObjectStore);
    }
}
