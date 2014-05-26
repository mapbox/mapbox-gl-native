#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/map/map.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/geometry/sprite_atlas.hpp>
#include <llmr/util/std.hpp>

using namespace llmr;



void Painter::renderFill(FillBucket& bucket, const FillProperties& properties, const Tile::ID& id, const mat4 &vtxMatrix) {
    Color fill_color = properties.fill_color;
    fill_color[0] *= properties.opacity;
    fill_color[1] *= properties.opacity;
    fill_color[2] *= properties.opacity;
    fill_color[3] *= properties.opacity;

    Color stroke_color = properties.stroke_color;
    stroke_color[0] *= properties.opacity;
    stroke_color[1] *= properties.opacity;
    stroke_color[2] *= properties.opacity;
    stroke_color[3] *= properties.opacity;

    bool outline = properties.antialias && properties.stroke_color != properties.fill_color;
    bool fringeline = properties.antialias && properties.stroke_color == properties.fill_color;
    if (fringeline) {
        outline = true;
        stroke_color = fill_color;
    }


    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (outline && pass == Translucent) {
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

    if ((fill_color[3] >= 1.0f) == (pass == Opaque)) {
        auto &sprite = map.getStyle().sprite;
        if (properties.image.size() && sprite) {
            auto &spriteAtlas = map.getSpriteAtlas();
            Rect<uint16_t> imagePos = spriteAtlas.getImage(properties.image, *sprite);


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
    if (fringeline && pass == Translucent) {
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

void Painter::renderFill(FillBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    // Abort early.
    if (!bucket.hasData()) return;

    auto fill_properties = map.getStyle().computed.fills;
    auto fill_properties_it = fill_properties.find(layer_name);
    if (fill_properties_it == fill_properties.end()) return;

    const FillProperties& properties = fill_properties_it->second;
    if (!properties.enabled) return;


    if (properties.prerender) {
        if (pass == Translucent) {
            // Buffer value around the 0..4096 extent that will be drawn into the 256x256 pixel
            // texture. We later scale the texture so that the actual bounds will align with this
            // tile's bounds. The reason we do this is so that the
            const int buffer = 128;

            if (!bucket.prerendered) {
                bucket.prerendered = std::make_unique<PrerenderedTexture>();
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_STENCIL_TEST);

                bucket.prerendered->bindFramebuffer();
                // Render the actual tile.
#if GL_EXT_discard_framebuffer
                const GLenum discards[] = { GL_COLOR_ATTACHMENT0 };
                glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
#endif
                glClear(GL_COLOR_BUFFER_BIT);

                glViewport(0, 0, 256, 256);


                // When drawing the fill, we want to draw a 16.66% buffer around too, so we
                // essentially downscale everyting, and then upscale it later when rendering.

                mat4 vtxMatrix;
                matrix::ortho(vtxMatrix, -buffer, 4096 + buffer, -4096 - buffer, buffer, 0, 1);
                matrix::translate(vtxMatrix, vtxMatrix, 0, -4096, 0);

                pass = Opaque;
                renderFill(bucket, properties, id, vtxMatrix);
                pass = Translucent;
                renderFill(bucket, properties, id, vtxMatrix);



                mat4 regularMatrix;
                matrix::ortho(regularMatrix, 0, 4096, -4096, 0, 0, 1);
                matrix::translate(regularMatrix, regularMatrix, 0, -4096, 0);


                // BLUR

                GLuint original_texture = bucket.prerendered->getTexture();

                GLuint secondary_texture;
                glGenTextures(1, &secondary_texture);
                glBindTexture(GL_TEXTURE_2D, secondary_texture);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                glBindTexture(GL_TEXTURE_2D, 0);


                useProgram(gaussianShader->program);
                gaussianShader->setMatrix(regularMatrix);
                gaussianShader->setImage(0);
                glActiveTexture(GL_TEXTURE0);


                for (int i = 0; i < 1; i++) {
                    // Render horizontal
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, secondary_texture, 0);
#if GL_EXT_discard_framebuffer
                    const GLenum discards[] = { GL_COLOR_ATTACHMENT0 };
                    glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
#endif
                    glClear(GL_COLOR_BUFFER_BIT);

                    gaussianShader->setOffset({{ 1.0f / 256.0f, 0 }});
                    glBindTexture(GL_TEXTURE_2D, original_texture);
                    coveringGaussianArray.bind(*gaussianShader, tileStencilBuffer, BUFFER_OFFSET(0));
                    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());



                    // Render vertical
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, original_texture, 0);
#if GL_EXT_discard_framebuffer
                    glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
#endif
                    glClear(GL_COLOR_BUFFER_BIT);

                    gaussianShader->setOffset({{ 0, 1.0f / 256.0f }});
                    glBindTexture(GL_TEXTURE_2D, secondary_texture);
                    coveringGaussianArray.bind(*gaussianShader, tileStencilBuffer, BUFFER_OFFSET(0));
                    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
                }

                glDeleteTextures(1, &secondary_texture);


                // RESET STATE


                bucket.prerendered->unbindFramebuffer();
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_STENCIL_TEST);

                glViewport(0, 0, gl_viewport[0], gl_viewport[1]);
            }

            // draw the texture on a quad
            depthMask(false);

            useProgram(rasterShader->program);
            rasterShader->setMatrix(matrix);
            rasterShader->setOpacity(1);

            glDepthRange(strata, 1.0f);

            glActiveTexture(GL_TEXTURE0);
            rasterShader->setImage(0);
            rasterShader->setBuffer(buffer);
            bucket.prerendered->bindTexture();
            coveringRasterArray.bind(*rasterShader, tileStencilBuffer, BUFFER_OFFSET(0));
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());

            depthMask(true);
        }
    } else {
        const mat4 &vtxMatrix = translatedMatrix(properties.translate, id, properties.translateAnchor);
        renderFill(bucket, properties, id, vtxMatrix);
    }
}
