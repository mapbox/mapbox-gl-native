#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/shader/symbol_uniforms.hpp>
#include <mbgl/shader/collision_box_uniforms.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/tile/tile.hpp>

#include <cmath>

namespace mbgl {

using namespace style;

void Painter::renderSymbol(PaintParameters& parameters,
                           SymbolBucket& bucket,
                           const SymbolLayer& layer,
                           const RenderTile& tile) {
    if (pass == RenderPass::Opaque) {
        return;
    }

    const auto& layout = bucket.layout;

    frameHistory.bind(context, 1);

    auto draw = [&] (auto& shader,
                     auto&& uniformValues,
                     const auto& buffers,
                     const SymbolPropertyValues& values_)
    {
        // In some cases, we disable the stencil test so that labels aren't clipped
        // to tile boundaries.
        //
        // Layers with features that may be drawn overlapping aren't clipped. These
        // layers are sorted in the y direction, and to draw the correct ordering near
        // tile edges the icons are included in both tiles and clipped when drawing.
        //
        // TODO remove the `true ||` when #1673 is implemented
        const bool drawAcrossEdges = (frame.mapMode == MapMode::Continuous) && (true || !(layout.textAllowOverlap || layout.iconAllowOverlap ||
              layout.textIgnorePlacement || layout.iconIgnorePlacement));

        context.draw({
            values_.pitchAlignment == AlignmentType::Map
                ? depthModeForSublayer(0, gl::DepthMode::ReadOnly)
                : gl::DepthMode::disabled(),
            drawAcrossEdges
                ? gl::StencilMode::disabled()
                : stencilModeForClipping(tile.clip),
            colorModeForRenderPass(),
            shader,
            std::move(uniformValues),
            gl::Segmented<gl::Triangles>(
                *buffers.vertexBuffer,
                *buffers.indexBuffer,
                buffers.segments
            )
        });
    };

    if (bucket.hasIconData()) {
        auto values = layer.impl->iconPropertyValues(layout);

        SpriteAtlas& atlas = *layer.impl->spriteAtlas;
        const bool iconScaled = values.paintSize != 1.0f || frame.pixelRatio != atlas.getPixelRatio() || bucket.iconsNeedLinear;
        const bool iconTransformed = values.rotationAlignment == AlignmentType::Map || state.getPitch() != 0;
        atlas.bind(bucket.sdfIcons || state.isChanging() || iconScaled || iconTransformed, context, 0);

        const Size texsize = atlas.getSize();

        if (bucket.sdfIcons) {
            if (values.hasHalo()) {
                draw(parameters.shaders.symbolIconSDF,
                     SymbolSDFUniforms::haloValues(values, texsize, pixelsToGLUnits, tile, state, frame.pixelRatio),
                     bucket.icon,
                     values);
            }

            if (values.hasForeground()) {
                draw(parameters.shaders.symbolIconSDF,
                     SymbolSDFUniforms::foregroundValues(values, texsize, pixelsToGLUnits, tile, state, frame.pixelRatio),
                     bucket.icon,
                     values);
            }
        } else {
            draw(parameters.shaders.symbolIcon,
                 SymbolIconUniforms::values(values, texsize, pixelsToGLUnits, tile, state),
                 bucket.icon,
                 values);
        }
    }

    if (bucket.hasTextData()) {
        glyphAtlas->bind(context, 0);

        auto values = layer.impl->textPropertyValues(layout);

        const Size texsize = glyphAtlas->getSize();

        if (values.hasHalo()) {
            draw(parameters.shaders.symbolGlyph,
                 SymbolSDFUniforms::haloValues(values, texsize, pixelsToGLUnits, tile, state, frame.pixelRatio),
                 bucket.text,
                 values);
        }

        if (values.hasForeground()) {
            draw(parameters.shaders.symbolGlyph,
                 SymbolSDFUniforms::foregroundValues(values, texsize, pixelsToGLUnits, tile, state, frame.pixelRatio),
                 bucket.text,
                 values);
        }
    }

    if (bucket.hasCollisionBoxData()) {
        context.draw({
            gl::DepthMode::disabled(),
            gl::StencilMode::disabled(),
            colorModeForRenderPass(),
            shaders->collisionBox,
            CollisionBoxUniforms::Values {
                uniforms::u_matrix::Value{ tile.matrix },
                uniforms::u_scale::Value{ std::pow(2.0f, float(state.getZoom() - tile.tile.id.overscaledZ)) },
                uniforms::u_zoom::Value{ float(state.getZoom() * 10) },
                uniforms::u_maxzoom::Value{ float((tile.id.canonical.z + 1) * 10) },
            },
            gl::Unindexed<gl::Lines>(
                *bucket.collisionBox.vertexBuffer,
                1.0f
            )
        });
    }
}

} // namespace mbgl
