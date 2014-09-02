#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/map/map.hpp>

using namespace mbgl;

void Painter::renderLine(LineBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;
    if (!bucket.hasData()) return;

    const LineProperties &properties = layer_desc->getProperties<LineProperties>();

    float antialiasing = 1 / map.getState().getPixelRatio();
    float width = properties.width;
    float offset = properties.offset / 2;
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

    float ratio = map.getState().getPixelRatio();
    mat4 vtxMatrix = translatedMatrix(matrix, properties.translate, id, properties.translateAnchor);

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

    const std::shared_ptr<Sprite> &sprite = map.getSprite();
    if (properties.image.size() && sprite) {
        SpriteAtlasPosition imagePos = map.getSpriteAtlas()->getPosition(properties.image, *sprite);

        float factor = 8.0 / std::pow(2, map.getState().getIntegerZoom() - id.z);
        float fade = std::fmod(map.getState().getZoom(), 1.0);

        useProgram(linepatternShader->program);

        linepatternShader->setMatrix(vtxMatrix);
        linepatternShader->setExtrudeMatrix(extrudeMatrix);
        linepatternShader->setLineWidth({{ outset, inset }});
        linepatternShader->setRatio(ratio);
        linepatternShader->setBlur(blur);

        linepatternShader->setPatternSize({{imagePos.size[0] * factor, imagePos.size[1]}});
        linepatternShader->setPatternTopLeft(imagePos.tl);
        linepatternShader->setPatternBottomRight(imagePos.br);
        linepatternShader->setFade(fade);

        map.getSpriteAtlas()->bind(true);
        glDepthRange(strata + strata_epsilon, 1.0f);  // may or may not matter

        bucket.drawLinePatterns(*linepatternShader);

    } else {
        useProgram(lineShader->program);

        lineShader->setMatrix(vtxMatrix);
        lineShader->setExtrudeMatrix(extrudeMatrix);
        lineShader->setLineWidth({{ outset, inset }});
        lineShader->setRatio(ratio);
        lineShader->setBlur(blur);

        lineShader->setColor(color);
        lineShader->setDashArray({{ dash_length, dash_gap }});

        bucket.drawLines(*lineShader);
    }
}
