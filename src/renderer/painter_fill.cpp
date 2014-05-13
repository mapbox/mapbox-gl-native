#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/map/map.hpp>
#include <llmr/style/sprite.hpp>

using namespace llmr;

void Painter::renderFill(FillBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    // Abort early.
    if (!bucket.hasData()) return;

    auto fill_properties = map.getStyle().computed.fills;
    auto fill_properties_it = fill_properties.find(layer_name);
    if (fill_properties_it == fill_properties.end()) return;

    const FillProperties& properties = fill_properties_it->second;
    if (!properties.enabled) return;

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

    translateLayer(properties.translate);

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (outline && pass == Translucent) {
        useProgram(outlineShader->program);
        outlineShader->setMatrix(matrix);
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
        if (properties.image.size() && sprite && sprite->isLoaded()) {
            // Draw texture fill
            ImagePosition imagePos = sprite->getPosition(properties.image, true);

            float factor = 8.0 / pow(2, map.getState().getIntegerZoom() - id.z);
            float mix = fmod(map.getState().getZoom(), 1.0);

            std::array<float, 2> imageSize = {{
                    imagePos.size.x * factor,
                    imagePos.size.y *factor
                }
            };

            std::array<float, 2> offset = {{
                    (float)fmod(id.x * 4096, imageSize[0]),
                    (float)fmod(id.y * 4096, imageSize[1])
                }
            };

            useProgram(patternShader->program);
            patternShader->setMatrix(matrix);
            patternShader->setOffset(offset);
            patternShader->setPatternSize(imageSize);
            patternShader->setPatternTopLeft({{ imagePos.tl.x, imagePos.tl.y }});
            patternShader->setPatternBottomRight({{ imagePos.br.x, imagePos.br.y }});
            patternShader->setColor(fill_color);
            patternShader->setMix(mix);
            sprite->raster.bind(true);

            // Draw the actual triangles into the color & stencil buffer.
            glDepthRange(strata + strata_epsilon, 1.0f);
            bucket.drawElements(*patternShader);
        } else {
            // Only draw the fill when it's either opaque and we're drawing opaque
            // fragments or when it's translucent and we're drawing translucent
            // fragments
            // Draw filling rectangle.
            useProgram(plainShader->program);
            plainShader->setMatrix(matrix);
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
        outlineShader->setMatrix(matrix);
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

    translateLayer(properties.translate, true);
}
