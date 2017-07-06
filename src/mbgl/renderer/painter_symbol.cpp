#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/render_symbol_layer.hpp>
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
                           const RenderSymbolLayer& layer,
                           const RenderTile& tile) {
    if (pass == RenderPass::Opaque) {
        return;
    }

    const auto& layout = bucket.layout;

    frameHistory.bind(context, 1);

    auto draw = [&] (auto& program,
                     auto&& uniformValues,
                     const auto& buffers,
                     const auto& symbolSizeBinder,
                     const SymbolPropertyValues& values_,
                     const auto& binders,
                     const auto& paintProperties)
    {
        // We clip symbols to their tile extent in still mode.
        const bool needsClipping = frame.mapMode == MapMode::Still;

        program.get(paintProperties).draw(
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
            *symbolSizeBinder,
            *buffers.indexBuffer,
            buffers.segments,
            binders,
            paintProperties,
            state.getZoom(),
            layer.getID()
        );
    };

    if (bucket.hasIconData()) {
        auto values = layer.iconPropertyValues(layout);
        auto paintPropertyValues = layer.iconPaintProperties();

        SpriteAtlas& atlas = *bucket.spriteAtlas;
        const bool iconScaled = layout.get<IconSize>().constantOr(1.0) != 1.0 ||
            frame.pixelRatio != atlas.getPixelRatio() ||
            bucket.iconsNeedLinear;
        const bool iconTransformed = values.rotationAlignment == AlignmentType::Map || state.getPitch() != 0;
        atlas.bind(bucket.sdfIcons || state.isChanging() || iconScaled || iconTransformed, context, 0);

        const Size texsize = atlas.getSize();

        if (bucket.sdfIcons) {
            if (values.hasHalo) {
                draw(parameters.programs.symbolIconSDF,
                     SymbolSDFIconProgram::uniformValues(false, values, texsize, pixelsToGLUnits, tile, state, SymbolSDFPart::Halo),
                     bucket.icon,
                     bucket.iconSizeBinder,
                     values,
                     bucket.paintPropertyBinders.at(layer.getID()).first,
                     paintPropertyValues);
            }

            if (values.hasFill) {
                draw(parameters.programs.symbolIconSDF,
                     SymbolSDFIconProgram::uniformValues(false, values, texsize, pixelsToGLUnits, tile, state, SymbolSDFPart::Fill),
                     bucket.icon,
                     bucket.iconSizeBinder,
                     values,
                     bucket.paintPropertyBinders.at(layer.getID()).first,
                     paintPropertyValues);
            }
        } else {
            draw(parameters.programs.symbolIcon,
                 SymbolIconProgram::uniformValues(false, values, texsize, pixelsToGLUnits, tile, state),
                 bucket.icon,
                 bucket.iconSizeBinder,
                 values,
                 bucket.paintPropertyBinders.at(layer.getID()).first,
                 paintPropertyValues);
        }
    }

    if (bucket.hasTextData()) {
        glyphAtlas->bind(context, 0);

        auto values = layer.textPropertyValues(layout);
        auto paintPropertyValues = layer.textPaintProperties();

        const Size texsize = glyphAtlas->getSize();

        if (values.hasHalo) {
            draw(parameters.programs.symbolGlyph,
                 SymbolSDFTextProgram::uniformValues(true, values, texsize, pixelsToGLUnits, tile, state, SymbolSDFPart::Halo),
                 bucket.text,
                 bucket.textSizeBinder,
                 values,
                 bucket.paintPropertyBinders.at(layer.getID()).second,
                 paintPropertyValues);
        }

        if (values.hasFill) {
            draw(parameters.programs.symbolGlyph,
                 SymbolSDFTextProgram::uniformValues(true, values, texsize, pixelsToGLUnits, tile, state, SymbolSDFPart::Fill),
                 bucket.text,
                 bucket.textSizeBinder,
                 values,
                 bucket.paintPropertyBinders.at(layer.getID()).second,
                 paintPropertyValues);
        }
    }

    if (bucket.hasCollisionBoxData()) {
        static const style::PaintProperties<>::Evaluated properties {};
        static const CollisionBoxProgram::PaintPropertyBinders paintAttributeData(properties, 0);

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
            bucket.collisionBox.segments,
            paintAttributeData,
            properties,
            state.getZoom(),
            layer.getID()
        );
    }
}

} // namespace mbgl
