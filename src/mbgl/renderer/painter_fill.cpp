#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_layout.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/util/mat3.hpp>

using namespace mbgl;

void Painter::renderFill(FillBucket& bucket, const StyleLayer &layer_desc, const TileID& id, const mat4 &matrix) {
    const FillProperties &properties = layer_desc.getProperties<FillProperties>();
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

    const bool pattern = !properties.image.from.empty();

    bool outline = properties.antialias && !pattern && stroke_color != fill_color;
    bool fringeline = properties.antialias && !pattern && stroke_color == fill_color;

    config.stencilTest = true;
    config.depthTest = true;

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // befrom, we have to draw the outline first (!)
    if (outline && pass == RenderPass::Translucent) {
        useProgram(outlineShader->program);
        outlineShader->u_matrix = vtxMatrix;
        lineWidth(2.0f); // This is always fixed and does not depend on the pixelRatio!

        outlineShader->u_color = stroke_color;

        // Draw the entire line
        outlineShader->u_world = {{
            static_cast<float>(frame.framebufferSize[0]),
            static_cast<float>(frame.framebufferSize[1])
        }};
        config.depthRange = { strata, 1.0f };
        bucket.drawVertices(*outlineShader);
    }

    if (pattern) {
        // Image fill.
        if (pass == RenderPass::Translucent) {

            const SpriteAtlasPosition posA = spriteAtlas->getPosition(properties.image.from, true);
            const SpriteAtlasPosition posB = spriteAtlas->getPosition(properties.image.to, true);
            float factor = 8.0 / std::pow(2, state.getIntegerZoom() - id.z) / id.overscaling;

            mat3 patternMatrixA;
            matrix::identity(patternMatrixA);
            matrix::scale(patternMatrixA, patternMatrixA,
                    1.0f / (posA.size[0] * factor * properties.image.fromScale),
                    1.0f / (posA.size[1] * factor * properties.image.fromScale));
            mat3 patternMatrixB;
            matrix::identity(patternMatrixB);
            matrix::scale(patternMatrixB, patternMatrixB,
                    1.0f / (posB.size[0] * factor * properties.image.toScale),
                    1.0f / (posB.size[1] * factor * properties.image.toScale));

            useProgram(patternShader->program);
            patternShader->u_matrix = vtxMatrix;
            patternShader->u_pattern_tl_a = posA.tl;
            patternShader->u_pattern_br_a = posA.br;
            patternShader->u_pattern_tl_b = posB.tl;
            patternShader->u_pattern_br_b = posB.br;
            patternShader->u_opacity = properties.opacity;
            patternShader->u_image = 0;
            patternShader->u_mix = properties.image.t;
            patternShader->u_patternmatrix_a = patternMatrixA;
            patternShader->u_patternmatrix_b = patternMatrixB;

            MBGL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0));
            spriteAtlas->bind(true);

            // Draw the actual triangles into the color & stencil buffer.
            config.depthMask = GL_TRUE;
            config.depthRange = { strata, 1.0f };
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
            plainShader->u_matrix = vtxMatrix;
            plainShader->u_color = fill_color;

            // Draw the actual triangles into the color & stencil buffer.
            config.depthMask = GL_TRUE;
            config.depthRange = { strata + strata_epsilon, 1.0f };
            bucket.drawElements(*plainShader);
        }
    }

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (fringeline && pass == RenderPass::Translucent) {
        useProgram(outlineShader->program);
        outlineShader->u_matrix = vtxMatrix;
        lineWidth(2.0f); // This is always fixed and does not depend on the pixelRatio!

        outlineShader->u_color = fill_color;

        // Draw the entire line
        outlineShader->u_world = {{
            static_cast<float>(frame.framebufferSize[0]),
            static_cast<float>(frame.framebufferSize[1])
        }};

        config.depthRange = { strata + strata_epsilon + strata_epsilon, 1.0f };
        bucket.drawVertices(*outlineShader);
    }
}
