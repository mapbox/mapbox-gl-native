#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/mat3.hpp>

using namespace mbgl;

void Painter::renderFill(FillBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix) {
    // Abort early.
    if (!bucket.hasData()) return;

    const FillProperties &properties = layer_desc->getProperties<FillProperties>();
    mat4 vtxMatrix = translatedMatrix(matrix, properties.translate, id, properties.translateAnchor);

    Color fill_color = properties.fill_color;
    fill_color[0] *= properties.opacity;
    fill_color[1] *= properties.opacity;
    fill_color[2] *= properties.opacity;
    fill_color[3] *= properties.opacity;

    Color stroke_color = properties.stroke_color;
    if (stroke_color[3] < 0) {
        stroke_color = fill_color;
    } else {
        stroke_color[0] *= properties.opacity;
        stroke_color[1] *= properties.opacity;
        stroke_color[2] *= properties.opacity;
        stroke_color[3] *= properties.opacity;
    }

    const bool pattern = properties.image.size();

    bool outline = properties.antialias && !pattern && properties.stroke_color != properties.fill_color;
    bool fringeline = properties.antialias && !pattern && properties.stroke_color == properties.fill_color;

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (outline && pass == RenderPass::Translucent) {
        useProgram(outlineShader->program);
        outlineShader->setMatrix(vtxMatrix);
        lineWidth(2.0f); // This is always fixed and does not depend on the pixelRatio!

        outlineShader->setColor(stroke_color);

        // Draw the entire line
        outlineShader->setWorld({{
            static_cast<float>(map.getState().getFramebufferWidth()),
            static_cast<float>(map.getState().getFramebufferHeight())
        }});
        depthRange(strata, 1.0f);
        bucket.drawVertices(*outlineShader);
    }

    if (pattern) {
        // Image fill.
        Sprite &sprite = *map.getSprite();
        if (pass == RenderPass::Translucent && sprite) {
            SpriteAtlas &spriteAtlas = *map.getSpriteAtlas();
            const SpriteAtlasPosition pos = spriteAtlas.getPosition(properties.image, sprite, true);
            const float mix = std::fmod(float(map.getState().getZoom()), 1.0f);
            const float factor = 8.0 / std::pow(2, map.getState().getIntegerZoom() - id.z);

            mat3 patternMatrix;
            matrix::identity(patternMatrix);
            matrix::scale(patternMatrix, patternMatrix, 1.0f / (pos.size[0] * factor), 1.0f / (pos.size[1] * factor));

            useProgram(patternShader->program);
            patternShader->setMatrix(vtxMatrix);
            patternShader->setPatternTopLeft(pos.tl);
            patternShader->setPatternBottomRight(pos.br);
            patternShader->setOpacity(properties.opacity);
            patternShader->setImage(0);
            patternShader->setMix(mix);
            patternShader->setPatternMatrix(patternMatrix);

            glActiveTexture(GL_TEXTURE0);
            spriteAtlas.bind(true);

            // Draw the actual triangles into the color & stencil buffer.
            depthRange(strata, 1.0f);
            bucket.drawElements(*patternShader);
        }
    }
    else {
        // No image fill.
        if ((fill_color[3] >= 1.0f) == (pass == RenderPass::Opaque)) {
            // Only draw the fill when it's either opaque and we're drawing opaque
            // fragments or when it's translucent and we're drawing translucent
            // fragments
            // Draw filling rectangle.
            useProgram(plainShader->program);
            plainShader->setMatrix(vtxMatrix);
            plainShader->setColor(fill_color);

            // Draw the actual triangles into the color & stencil buffer.
            depthRange(strata + strata_epsilon, 1.0f);
            bucket.drawElements(*plainShader);
        }
    }

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (fringeline && pass == RenderPass::Translucent) {
        useProgram(outlineShader->program);
        outlineShader->setMatrix(vtxMatrix);
        lineWidth(2.0f); // This is always fixed and does not depend on the pixelRatio!

        outlineShader->setColor(fill_color);

        // Draw the entire line
        outlineShader->setWorld({{
            static_cast<float>(map.getState().getFramebufferWidth()),
            static_cast<float>(map.getState().getFramebufferHeight())
        }});

        depthRange(strata + strata_epsilon, 1.0f);
        bucket.drawVertices(*outlineShader);
    }
}
