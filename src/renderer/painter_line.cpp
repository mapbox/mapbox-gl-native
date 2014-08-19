#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/map.hpp>

using namespace mbgl;

void Painter::renderLine(LineBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;
    if (!bucket.hasData()) return;

    const LineProperties &properties = layer_desc->getProperties<LineProperties>();

    float width = properties.width;
    float offset = properties.offset / 2;
    float antialiasing = 1 / map.getState().getPixelRatio();
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

    const mat4 &vtxMatrix = translatedMatrix(matrix, properties.translate, id, properties.translateAnchor);

    depthRange(strata, 1.0f);

    // We're only drawing end caps + round line joins if the line is > 2px. Otherwise, they aren't visible anyway.
    if (bucket.hasPoints() && outset > 1.0f) {
        useProgram(linejoinShader->program);
        linejoinShader->setMatrix(vtxMatrix);
        linejoinShader->setColor(color);
        linejoinShader->setWorld({{
                map.getState().getFramebufferWidth() * 0.5f,
                map.getState().getFramebufferHeight() * 0.5f
            }
        });
        linejoinShader->setLineWidth({{
                ((outset - 0.25f) * map.getState().getPixelRatio()),
                ((inset - 0.25f) * map.getState().getPixelRatio())
            }
        });

        float pointSize = std::ceil(map.getState().getPixelRatio() * outset * 2.0);
#if defined(GL_ES_VERSION_2_0)
        linejoinShader->setSize(pointSize);
#else
        glPointSize(pointSize);
#endif
        bucket.drawPoints(*linejoinShader);
    }

    // var imagePos = properties.image && imageSprite.getPosition(properties.image);
    bool imagePos = false;
    if (imagePos) {
        // var factor = 8 / Math.pow(2, painter.transform.zoom - params.z);

        // imageSprite.bind(gl, true);

        // //factor = Math.pow(2, 4 - painter.transform.zoom + params.z);
        // gl.switchShader(painter.linepatternShader, painter.translatedMatrix || painter.posMatrix, painter.extrudeMatrix);
        // shader = painter.linepatternShader;
        // glUniform2fv(painter.linepatternShader.u_pattern_size, [imagePos.size[0] * factor, imagePos.size[1] ]);
        // glUniform2fv(painter.linepatternShader.u_pattern_tl, imagePos.tl);
        // glUniform2fv(painter.linepatternShader.u_pattern_br, imagePos.br);
        // glUniform1f(painter.linepatternShader.u_fade, painter.transform.z % 1.0);

    } else {
        useProgram(lineShader->program);
        lineShader->setMatrix(vtxMatrix);
        lineShader->setExtrudeMatrix(extrudeMatrix);
        lineShader->setDashArray({{ dash_length, dash_gap }});
        lineShader->setLineWidth({{ outset, inset }});
        lineShader->setRatio(map.getState().getPixelRatio());
        lineShader->setBlur(blur);
        lineShader->setColor(color);
        bucket.drawLines(*lineShader);
    }
}
