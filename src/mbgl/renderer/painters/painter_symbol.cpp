#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/programs/collision_box_program.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/layout/symbol_projection.hpp>

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
            *buffers.dynamicVertexBuffer,
            *symbolSizeBinder,
            *buffers.indexBuffer,
            buffers.segments,
            binders,
            paintProperties,
            state.getZoom(),
            layer.getID()
        );
    };

    assert(dynamic_cast<GeometryTile*>(&tile.tile));
    GeometryTile& geometryTile = static_cast<GeometryTile&>(tile.tile);


    if (bucket.hasIconData()) {
        auto values = layer.iconPropertyValues(layout);
        auto paintPropertyValues = layer.iconPaintProperties();

        const bool alongLine = bucket.layout.get<SymbolPlacement>() == SymbolPlacementType::Line &&
            bucket.layout.get<IconRotationAlignment>() == AlignmentType::Map;

        if (alongLine) {
            reprojectLineLabels(bucket.icon.dynamicVertices, bucket.icon.placedSymbols, tile.matrix, values, tile, *(bucket.iconSizeBinder), state, frameHistory);
            context.updateVertexBuffer(*bucket.icon.dynamicVertexBuffer, std::move(bucket.icon.dynamicVertices));
        }

        const bool iconScaled = layout.get<IconSize>().constantOr(1.0) != 1.0 || bucket.iconsNeedLinear;
        const bool iconTransformed = values.rotationAlignment == AlignmentType::Map || state.getPitch() != 0;

        context.bindTexture(*geometryTile.iconAtlasTexture, 0,
            bucket.sdfIcons || state.isChanging() || iconScaled || iconTransformed
                ? gl::TextureFilter::Linear : gl::TextureFilter::Nearest);

        const Size texsize = geometryTile.iconAtlasTexture->size;

        if (bucket.sdfIcons) {
            if (values.hasHalo) {
                draw(parameters.programs.symbolIconSDF,
                     SymbolSDFIconProgram::uniformValues(false, values, texsize, pixelsToGLUnits, alongLine, tile, state, SymbolSDFPart::Halo),
                     bucket.icon,
                     bucket.iconSizeBinder,
                     values,
                     bucket.paintPropertyBinders.at(layer.getID()).first,
                     paintPropertyValues);
            }

            if (values.hasFill) {
                draw(parameters.programs.symbolIconSDF,
                     SymbolSDFIconProgram::uniformValues(false, values, texsize, pixelsToGLUnits, alongLine, tile, state, SymbolSDFPart::Fill),
                     bucket.icon,
                     bucket.iconSizeBinder,
                     values,
                     bucket.paintPropertyBinders.at(layer.getID()).first,
                     paintPropertyValues);
            }
        } else {
            draw(parameters.programs.symbolIcon,
                 SymbolIconProgram::uniformValues(false, values, texsize, pixelsToGLUnits, alongLine, tile, state),
                 bucket.icon,
                 bucket.iconSizeBinder,
                 values,
                 bucket.paintPropertyBinders.at(layer.getID()).first,
                 paintPropertyValues);
        }
    }

    if (bucket.hasTextData()) {
        context.bindTexture(*geometryTile.glyphAtlasTexture, 0, gl::TextureFilter::Linear);

        auto values = layer.textPropertyValues(layout);
        auto paintPropertyValues = layer.textPaintProperties();

        const bool alongLine = bucket.layout.get<SymbolPlacement>() == SymbolPlacementType::Line &&
            bucket.layout.get<TextRotationAlignment>() == AlignmentType::Map;

        if (alongLine) {
            reprojectLineLabels(bucket.text.dynamicVertices, bucket.text.placedSymbols, tile.matrix, values, tile, *(bucket.textSizeBinder), state, frameHistory);
            context.updateVertexBuffer(*bucket.text.dynamicVertexBuffer, std::move(bucket.text.dynamicVertices));
        }

        const Size texsize = geometryTile.glyphAtlasTexture->size;

        if (values.hasHalo) {
            draw(parameters.programs.symbolGlyph,
                 SymbolSDFTextProgram::uniformValues(true, values, texsize, pixelsToGLUnits, alongLine, tile, state, SymbolSDFPart::Halo),
                 bucket.text,
                 bucket.textSizeBinder,
                 values,
                 bucket.paintPropertyBinders.at(layer.getID()).second,
                 paintPropertyValues);
        }

        if (values.hasFill) {
            draw(parameters.programs.symbolGlyph,
                 SymbolSDFTextProgram::uniformValues(true, values, texsize, pixelsToGLUnits, alongLine, tile, state, SymbolSDFPart::Fill),
                 bucket.text,
                 bucket.textSizeBinder,
                 values,
                 bucket.paintPropertyBinders.at(layer.getID()).second,
                 paintPropertyValues);
        }
    }

    if (bucket.hasCollisionBoxData()) {
        static const style::Properties<>::PossiblyEvaluated properties {};
        static const CollisionBoxProgram::PaintPropertyBinders paintAttributeData(properties, 0);

        programs->collisionBox.draw(
            context,
            gl::Lines { 1.0f },
            gl::DepthMode::disabled(),
            stencilModeForClipping(tile.clip),
            colorModeForRenderPass(),
            CollisionBoxProgram::UniformValues {
                uniforms::u_matrix::Value{ tile.matrix },
                uniforms::u_scale::Value{ std::pow(2.0f, float(state.getZoom() - tile.tile.id.overscaledZ)) },
                uniforms::u_zoom::Value{ float(state.getZoom() * 10) },
                uniforms::u_maxzoom::Value{ float((tile.id.canonical.z + 1) * 10) },
                uniforms::u_collision_y_stretch::Value{ tile.tile.yStretch() },
                uniforms::u_camera_to_center_distance::Value{ state.getCameraToCenterDistance() },
                uniforms::u_pitch::Value{ state.getPitch() },
                uniforms::u_fadetexture::Value{ 1 }
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
