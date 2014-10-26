#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/map/map.hpp>

using namespace mbgl;

void Painter::renderLine(LineBucket& bucket, util::ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;
    if (!bucket.hasData()) return;

    const LineProperties &properties = layer_desc->getProperties<LineProperties>();

    float antialiasing = 1 / state.getPixelRatio();
    float width = properties.width;
    float offset = properties.gap_width / 2;
    offset = offset == 0 ? offset : offset + width / 2;
    float blur = properties.blur + antialiasing;

    // These are the radii of the line. We are limiting it to 16, which will result
    // in a point size of 64 on retina.
    float inset = std::fmin((std::fmax(-1, offset - width / 2 - antialiasing / 2) + 1), 16.0f);
    float outset = std::fmin(offset + width / 2 + antialiasing / 2, 16.0f);

    Color color = properties.color;
    color[0] *= properties.opacity;
    color[1] *= properties.opacity;
    color[2] *= properties.opacity;
    color[3] *= properties.opacity;

    float dash_length = properties.dash_array[0];
    float dash_gap = properties.dash_array[1];

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
        glPointSize(pointSize);
#endif
        bucket.drawPoints(*linejoinShader);
    }

    if (properties.image.size()) {
        SpriteAtlasPosition imagePos = spriteAtlas.getPosition(properties.image);

        float factor = 8.0 / std::pow(2, state.getIntegerZoom() - id.z);
        float fade = std::fmod(state.getZoom(), 1.0);

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

        spriteAtlas.bind(true);
        glDepthRange(strata + strata_epsilon, 1.0f);  // may or may not matter

        bucket.drawLinePatterns(*linepatternShader);

    } else {
        useProgram(lineShader->program);

        lineShader->u_matrix = vtxMatrix;
        lineShader->u_exmatrix = extrudeMatrix;
        lineShader->u_linewidth = {{ outset, inset }};
        lineShader->u_ratio = ratio;
        lineShader->u_blur = blur;

        lineShader->u_color = color;
        lineShader->u_dasharray = {{ dash_length, dash_gap }};

        bucket.drawLines(*lineShader);
    }
}
