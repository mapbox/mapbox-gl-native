#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/util/std.hpp>

using namespace mbgl;



void Painter::renderFill(FillBucket& bucket, const FillProperties& properties, const Tile::ID& id, const mat4 &vtxMatrix) {
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

    bool outline = properties.antialias && properties.stroke_color != properties.fill_color;
    bool fringeline = properties.antialias && properties.stroke_color == properties.fill_color;
    if (fringeline) {
        outline = true;
        stroke_color = fill_color;
    }


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
        glDepthRange(strata, 1.0f);
        bucket.drawVertices(*outlineShader);
    } else if (fringeline) {
        // // We're only drawing to the first seven bits (== support a maximum of
        // // 127 overlapping polygons in one place before we get rendering errors).
        // glStencilMask(0x3F);
        // glClear(GL_STENCIL_BUFFER_BIT);

        // // Draw front facing triangles. Wherever the 0x80 bit is 1, we are
        // // increasing the lower 7 bits by one if the triangle is a front-facing
        // // triangle. This means that all visible polygons should be in CCW
        // // orientation, while all holes (see below) are in CW orientation.
        // glStencilFunc(GL_EQUAL, 0x80, 0x80);

        // // When we do a nonzero fill, we count the number of times a pixel is
        // // covered by a counterclockwise polygon, and subtract the number of
        // // times it is "uncovered" by a clockwise polygon.
        // glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    }

    if ((fill_color[3] >= 1.0f) == (pass == RenderPass::Opaque)) {
        Sprite &sprite = *map.getSprite();
        if (properties.image.size() && sprite) {
            SpriteAtlas &spriteAtlas = *map.getSpriteAtlas();
            Rect<uint16_t> imagePos = spriteAtlas.getImage(properties.image, sprite);


            float factor = 8.0 / std::pow(2, map.getState().getIntegerZoom() - id.z);
            float mix = std::fmod(map.getState().getZoom(), 1.0);

            std::array<float, 2> imageSize = {{
                    imagePos.w * factor,
                    imagePos.h * factor
                }
            };

            std::array<float, 2> offset = {{
                    (float)std::fmod(id.x * 4096, imageSize[0]),
                    (float)std::fmod(id.y * 4096, imageSize[1])
                }
            };

            useProgram(patternShader->program);
            patternShader->setMatrix(vtxMatrix);
            patternShader->setOffset(offset);
            patternShader->setPatternSize(imageSize);
            patternShader->setPatternTopLeft({{
                float(imagePos.x) / spriteAtlas.getWidth(),
                float(imagePos.y) / spriteAtlas.getHeight(),
            }});
            patternShader->setPatternBottomRight({{
                float(imagePos.x + imagePos.w) / spriteAtlas.getWidth(),
                float(imagePos.y + imagePos.h) / spriteAtlas.getHeight(),
            }});
            patternShader->setColor(fill_color);
            patternShader->setMix(mix);
            spriteAtlas.bind(true);

            // Draw the actual triangles into the color & stencil buffer.
            glDepthRange(strata + strata_epsilon, 1.0f);
            bucket.drawElements(*patternShader);
        } else {
            // Only draw the fill when it's either opaque and we're drawing opaque
            // fragments or when it's translucent and we're drawing translucent
            // fragments
            // Draw filling rectangle.
            useProgram(plainShader->program);
            plainShader->setMatrix(vtxMatrix);
            plainShader->setColor(fill_color);

            // Draw the actual triangles into the color & stencil buffer.
            glDepthRange(strata + strata_epsilon, 1.0f);
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

        glDepthRange(strata + strata_epsilon, 1.0f);
        bucket.drawVertices(*outlineShader);
    }
}

void Painter::renderFill(FillBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix) {
    // Abort early.
    if (!bucket.hasData()) return;
    const FillProperties &properties = layer_desc->getProperties<FillProperties>();
    const mat4 &vtxMatrix = translatedMatrix(matrix, properties.translate, id, properties.translateAnchor);
    renderFill(bucket, properties, id, vtxMatrix);
}
