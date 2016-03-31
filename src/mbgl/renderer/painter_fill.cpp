#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/plain_shader.hpp>

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
    config.depthMask = GL_TRUE;

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // befrom, we have to draw the outline first (!)
    if (outline && pass == RenderPass::Translucent) {
        config.program = outlineShader->getID();
        outlineShader->u_matrix = vtxMatrix;
        config.lineWidth = 2.0f; // This is always fixed and does not depend on the pixelRatio!

        outlineShader->u_color = stroke_color;

        // Draw the entire line
        outlineShader->u_world = {{
            static_cast<float>(frame.framebufferSize[0]),
            static_cast<float>(frame.framebufferSize[1])
        }};
        setDepthSublayer(0);
        bucket.drawVertices(*outlineShader, glObjectStore);
    }

    if (pattern) {
        optional<SpriteAtlasPosition> posA = spriteAtlas->getPosition(properties.pattern.value.from, true);
        optional<SpriteAtlasPosition> posB = spriteAtlas->getPosition(properties.pattern.value.to, true);

        // Image fill.
        if (pass == RenderPass::Translucent && posA && posB) {

            config.program = patternShader->getID();
            patternShader->u_matrix = vtxMatrix;
            patternShader->u_pattern_tl_a = (*posA).tl;
            patternShader->u_pattern_br_a = (*posA).br;
            patternShader->u_pattern_tl_b = (*posB).tl;
            patternShader->u_pattern_br_b = (*posB).br;
            patternShader->u_opacity = properties.opacity;
            patternShader->u_image = 0;
            patternShader->u_mix = properties.pattern.value.t;

            std::array<int, 2> imageSizeScaledA = {{
                (int)((*posA).size[0] * properties.pattern.value.fromScale),
                (int)((*posA).size[1] * properties.pattern.value.fromScale)
            }};
            std::array<int, 2> imageSizeScaledB = {{
                (int)((*posB).size[0] * properties.pattern.value.toScale),
                (int)((*posB).size[1] * properties.pattern.value.toScale)
            }};

            patternShader->u_patternscale_a = {{
                1.0f / id.pixelsToTileUnits(imageSizeScaledA[0], state.getIntegerZoom()),
                1.0f / id.pixelsToTileUnits(imageSizeScaledB[1], state.getIntegerZoom())
            }};
            patternShader->u_patternscale_b = {{
                1.0f / id.pixelsToTileUnits(imageSizeScaledB[0], state.getIntegerZoom()),
                1.0f / id.pixelsToTileUnits(imageSizeScaledB[1], state.getIntegerZoom())
            }};

            float offsetAx = (std::fmod(util::tileSize, imageSizeScaledA[0]) * id.x) / (float)imageSizeScaledA[0];
            float offsetAy = (std::fmod(util::tileSize, imageSizeScaledA[1]) * id.y) / (float)imageSizeScaledA[1];

            float offsetBx = (std::fmod(util::tileSize, imageSizeScaledB[0]) * id.x) / (float)imageSizeScaledB[0];
            float offsetBy = (std::fmod(util::tileSize, imageSizeScaledB[1]) * id.y) / (float)imageSizeScaledB[1];

            patternShader->u_offset_a = std::array<float, 2>{{offsetAx, offsetAy}};
            patternShader->u_offset_b = std::array<float, 2>{{offsetBx, offsetBy}};

            MBGL_CHECK_ERROR(glActiveTexture(GL_TEXTURE0));
            spriteAtlas->bind(true, glObjectStore);

            // Draw the actual triangles into the color & stencil buffer.
            setDepthSublayer(0);
            bucket.drawElements(*patternShader, glObjectStore);
        }
    }
    else {
        // No image fill.
        if ((fill_color[3] >= 1.0f) == (pass == RenderPass::Opaque)) {
            // Only draw the fill when it's either opaque and we're drawing opaque
            // fragments or when it's translucent and we're drawing translucent
            // fragments
            // Draw filling rectangle.
            config.program = plainShader->getID();
            plainShader->u_matrix = vtxMatrix;
            plainShader->u_color = fill_color;

            // Draw the actual triangles into the color & stencil buffer.
            setDepthSublayer(1);
            bucket.drawElements(*plainShader, glObjectStore);
        }
    }

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (fringeline && pass == RenderPass::Translucent) {
        config.program = outlineShader->getID();
        outlineShader->u_matrix = vtxMatrix;
        config.lineWidth = 2.0f; // This is always fixed and does not depend on the pixelRatio!

        outlineShader->u_color = fill_color;

        // Draw the entire line
        outlineShader->u_world = {{
            static_cast<float>(frame.framebufferSize[0]),
            static_cast<float>(frame.framebufferSize[1])
        }};

        setDepthSublayer(2);
        bucket.drawVertices(*outlineShader, glObjectStore);
    }
}
