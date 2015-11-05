#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/util/mat3.hpp>

using namespace mbgl;

void Painter::renderFill(FillBucket& bucket, const FillLayer& layer, const TileID& id, const mat4& matrix) {
    const FillPaintProperties& properties = layer.paint;
    mat4 vtxMatrix = translatedMatrix(matrix, properties.translate, id, properties.translateAnchor);

    Color fill_color = properties.color;
    fill_color[0] *= properties.opacity;
    fill_color[1] *= properties.opacity;
    fill_color[2] *= properties.opacity;
    fill_color[3] *= properties.opacity;

    Color stroke_color = properties.outlineColor;
    if (stroke_color[3] < 0) {
        stroke_color = fill_color;
    } else {
        stroke_color[0] *= properties.opacity;
        stroke_color[1] *= properties.opacity;
        stroke_color[2] *= properties.opacity;
        stroke_color[3] *= properties.opacity;
    }

    const bool pattern = !properties.pattern.value.from.empty();

    bool outline = properties.antialias && !pattern && stroke_color != fill_color;
    bool fringeline = properties.antialias && !pattern && stroke_color == fill_color;

    config.stencilOp.reset();
    config.stencilTest = GL_TRUE;
    config.depthFunc.reset();
    config.depthTest = GL_TRUE;

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // befrom, we have to draw the outline first (!)
    if (outline && pass == RenderPass::Translucent) {
        config.program = outlineShader->program;
        outlineShader->u_matrix = vtxMatrix;
        config.lineWidth = 2.0f; // This is always fixed and does not depend on the pixelRatio!

        outlineShader->u_color = stroke_color;

        // Draw the entire line
        outlineShader->u_world = {{
            static_cast<float>(frame.framebufferSize[0]),
            static_cast<float>(frame.framebufferSize[1])
        }};
        setDepthSublayer(0);
        bucket.drawVertices(*outlineShader);
    }

    if (pattern) {
        // Image fill.
        if (pass == RenderPass::Translucent) {

            const SpriteAtlasPosition posA = spriteAtlas->getPosition(properties.pattern.value.from, true);
            const SpriteAtlasPosition posB = spriteAtlas->getPosition(properties.pattern.value.to, true);
            float factor = 8.0 / std::pow(2, state.getIntegerZoom() - id.z) / id.overscaling;

            mat3 patternMatrixA;
            matrix::identity(patternMatrixA);
            matrix::scale(patternMatrixA, patternMatrixA,
                    1.0f / (posA.size[0] * factor * properties.pattern.value.fromScale),
                    1.0f / (posA.size[1] * factor * properties.pattern.value.fromScale));
            mat3 patternMatrixB;
            matrix::identity(patternMatrixB);
            matrix::scale(patternMatrixB, patternMatrixB,
                    1.0f / (posB.size[0] * factor * properties.pattern.value.toScale),
                    1.0f / (posB.size[1] * factor * properties.pattern.value.toScale));

            config.program = patternShader->program;
            patternShader->u_matrix = vtxMatrix;
            patternShader->u_pattern_tl_a = posA.tl;
            patternShader->u_pattern_br_a = posA.br;
            patternShader->u_pattern_tl_b = posB.tl;
            patternShader->u_pattern_br_b = posB.br;
            patternShader->u_opacity = properties.opacity;
            patternShader->u_image = 0;
            patternShader->u_mix = properties.pattern.value.t;
            patternShader->u_patternmatrix_a = patternMatrixA;
            patternShader->u_patternmatrix_b = patternMatrixB;

            MBGL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0));
            spriteAtlas->bind(true);

            // Draw the actual triangles into the color & stencil buffer.
            config.depthMask = GL_TRUE;
            setDepthSublayer(0);
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
            config.program = plainShader->program;
            plainShader->u_matrix = vtxMatrix;
            plainShader->u_color = fill_color;

            // Draw the actual triangles into the color & stencil buffer.
            config.depthMask = GL_TRUE;
            setDepthSublayer(1);
            bucket.drawElements(*plainShader);
        }
    }

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (fringeline && pass == RenderPass::Translucent) {
        config.program = outlineShader->program;
        outlineShader->u_matrix = vtxMatrix;
        config.lineWidth = 2.0f; // This is always fixed and does not depend on the pixelRatio!

        outlineShader->u_color = fill_color;

        // Draw the entire line
        outlineShader->u_world = {{
            static_cast<float>(frame.framebufferSize[0]),
            static_cast<float>(frame.framebufferSize[1])
        }};

        setDepthSublayer(2);
        bucket.drawVertices(*outlineShader);
    }
}
