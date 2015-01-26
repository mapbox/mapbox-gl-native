#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/map/map.hpp>

using namespace mbgl;

void Painter::renderLine(LineBucket& bucket, util::ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;
    if (!bucket.hasData()) return;

    const LineProperties &properties = layer_desc->getProperties<LineProperties>();

    // the distance over which the line edge fades out.
    // Retina devices need a smaller distance to avoid aliasing.
    float antialiasing = 1 / state.getPixelRatio();

    float blur = properties.blur + antialiasing;
    float edgeWidth = properties.width / 2.0;
    float inset = -1;
    float offset = 0;
    float shift = 0;

    if (properties.gap_width != 0) {
        inset = properties.gap_width / 2.0 + antialiasing * 0.5;
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

    float ratio = state.getPixelRatio();
    mat4 vtxMatrix = translatedMatrix(matrix, properties.translate, id, properties.translateAnchor);

    depthRange(strata, 1.0f);

    // We're only drawing end caps + round line joins if the line is > 2px. Otherwise, they aren't visible anyway.
    if (bucket.hasPoints() && outset > 1.0f) {
        useProgram(linejoinShader->program);
        linejoinShader->u_matrix = vtxMatrix;
        linejoinShader->u_color = color;
        linejoinShader->u_world = {{
            state.getFramebufferWidth() * 0.5f,
            state.getFramebufferHeight() * 0.5f
        }};
        linejoinShader->u_linewidth = {{
            ((outset - 0.25f) * state.getPixelRatio()),
            ((inset - 0.25f) * state.getPixelRatio())
        }};

        float pointSize = std::ceil(state.getPixelRatio() * outset * 2.0);
#if defined(GL_ES_VERSION_2_0)
        linejoinShader->u_size = pointSize;
#else
        MBGL_CHECK_ERROR(glPointSize(pointSize));
#endif
        bucket.drawPoints(*linejoinShader);
    }

    float duration = 300 * 1_millisecond;
    const float fraction = std::fmod(float(state.getZoom()), 1.0f);
    float t = std::min((util::now() - lastIntegerZoomTime) / duration, 1.0f);

    if (properties.dash_array.size()) {

        useProgram(linesdfShader->program);

        linesdfShader->u_matrix = vtxMatrix;
        linesdfShader->u_exmatrix = extrudeMatrix;
        linesdfShader->u_linewidth = {{ outset, inset }};
        linesdfShader->u_ratio = ratio;
        linesdfShader->u_blur = blur;
        linesdfShader->u_color = color;

        LinePatternPos pos = lineAtlas.getDashPosition(properties.dash_array, bucket.properties.cap == CapType::Round);
        lineAtlas.bind();

        float patternratio = std::pow(2.0, std::floor(std::log2(state.getScale())) - id.z) / 8.0;
        float scaleX = patternratio / pos.width / properties.dash_line_width;
        float scaleY = -pos.height / 2.0;

        float mix;
        if (state.getZoom() > lastIntegerZoom) {
            // zooming in
            mix = fraction + (1.0f - fraction) * t;
            scaleX /= 2.0;
        } else {
            // zooming out
            mix = fraction - fraction * t;
        }

        linesdfShader->u_patternscale_a = {{ scaleX, scaleY }};
        linesdfShader->u_tex_y_a = pos.y;
        linesdfShader->u_patternscale_b = {{ scaleX * 2.0f, scaleY }};
        linesdfShader->u_tex_y_b = pos.y;
        linesdfShader->u_image = 0;
        linesdfShader->u_sdfgamma = lineAtlas.width / (properties.dash_line_width * pos.width * 256.0 * state.getPixelRatio()) / 2;
        linesdfShader->u_mix = mix;

        bucket.drawLineSDF(*linesdfShader);

    } else if (properties.image.size()) {
        SpriteAtlasPosition imagePos = spriteAtlas.getPosition(properties.image, true);

        float factor = 8.0 / std::pow(2, state.getIntegerZoom() - id.z);
        float fade;
        if (state.getZoom() > lastIntegerZoom) {
            // zooming in
            fade = fraction + (1.0f - fraction) * t;
            factor *= 2.0;
        } else {
            // zooming out
            fade = fraction - fraction * t;
        }

        useProgram(linepatternShader->program);

        linepatternShader->u_matrix = vtxMatrix;
        linepatternShader->u_exmatrix = extrudeMatrix;
        linepatternShader->u_linewidth = {{ outset, inset }};
        linepatternShader->u_ratio = ratio;
        linepatternShader->u_blur = blur;

        linepatternShader->u_pattern_size = {{imagePos.size[0] * factor, imagePos.size[1]}};
        linepatternShader->u_pattern_tl = imagePos.tl;
        linepatternShader->u_pattern_br = imagePos.br;
        linepatternShader->u_fade = fade;

        MBGL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0));
        spriteAtlas.bind(true);
        MBGL_CHECK_ERROR(glDepthRange(strata + strata_epsilon, 1.0f));  // may or may not matter

        bucket.drawLinePatterns(*linepatternShader);

    } else {
        useProgram(lineShader->program);

        lineShader->u_matrix = vtxMatrix;
        lineShader->u_exmatrix = extrudeMatrix;
        lineShader->u_linewidth = {{ outset, inset }};
        lineShader->u_ratio = ratio;
        lineShader->u_blur = blur;

        lineShader->u_color = color;

        bucket.drawLines(*lineShader);
    }
}
