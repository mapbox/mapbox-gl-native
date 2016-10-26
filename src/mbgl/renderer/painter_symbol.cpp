#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/programs/collision_box_program.hpp>
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

    auto draw = [&] (auto& program,
                     auto&& uniformValues,
                     const auto& buffers,
                     const SymbolPropertyValues& values_)
    {
        // We clip symbols to their tile extent in still mode.
        const bool needsClipping = frame.mapMode == MapMode::Still;

        program.draw(
            context,
            gl::Triangles(),
            values_.pitchAlignment == AlignmentType::Map
                ? depthModeForSublayer(0, gl::DepthMode::ReadOnly)
                : gl::DepthMode::disabled(),
            needsClipping
                ? stencilModeForClipping(tile.clip)
                : gl::StencilMode::disabled(),
            colorModeForRenderPass(),
            std::move(uniformValues),
            *buffers.vertexBuffer,
            *buffers.indexBuffer,
            buffers.segments
        );
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
                draw(parameters.programs.symbolIconSDF,
                     SymbolSDFProgram::haloUniformValues(values, texsize, pixelsToGLUnits, tile, state, frame.pixelRatio),
                     bucket.icon,
                     values);
            }

            if (values.hasForeground()) {
                draw(parameters.programs.symbolIconSDF,
                     SymbolSDFProgram::foregroundUniformValues(values, texsize, pixelsToGLUnits, tile, state, frame.pixelRatio),
                     bucket.icon,
                     values);
            }
        } else {
            draw(parameters.programs.symbolIcon,
                 SymbolIconProgram::uniformValues(values, texsize, pixelsToGLUnits, tile, state),
                 bucket.icon,
                 values);
        }
    }

    if (bucket.hasTextData()) {
        glyphAtlas->bind(context, 0);

        auto values = layer.impl->textPropertyValues(layout);

        const Size texsize = glyphAtlas->getSize();

        if (values.hasHalo()) {
            draw(parameters.programs.symbolGlyph,
                 SymbolSDFProgram::haloUniformValues(values, texsize, pixelsToGLUnits, tile, state, frame.pixelRatio),
                 bucket.text,
                 values);
        }

        if (values.hasForeground()) {
            draw(parameters.programs.symbolGlyph,
                 SymbolSDFProgram::foregroundUniformValues(values, texsize, pixelsToGLUnits, tile, state, frame.pixelRatio),
                 bucket.text,
                 values);
        }
    }

    if (bucket.hasCollisionBoxData()) {
        programs->collisionBox.draw(
            context,
            gl::Lines { 1.0f },
            gl::DepthMode::disabled(),
            gl::StencilMode::disabled(),
            colorModeForRenderPass(),
            CollisionBoxProgram::UniformValues {
                uniforms::u_matrix::Value{ tile.matrix },
                uniforms::u_scale::Value{ std::pow(2.0f, float(state.getZoom() - tile.tile.id.overscaledZ)) },
                uniforms::u_zoom::Value{ float(state.getZoom() * 10) },
                uniforms::u_maxzoom::Value{ float((tile.id.canonical.z + 1) * 10) },
            },
            *bucket.collisionBox.vertexBuffer,
            *bucket.collisionBox.indexBuffer,
            bucket.collisionBox.segments
        );
    }
}

} // namespace mbgl
