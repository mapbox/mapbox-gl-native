#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/programs/collision_box_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/layout/symbol_projection.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

namespace mbgl {

using namespace style;

RenderSymbolLayer::RenderSymbolLayer(Immutable<style::SymbolLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Symbol, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::SymbolLayer::Impl& RenderSymbolLayer::impl() const {
    return static_cast<const style::SymbolLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderSymbolLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false); // Should be calling createLayout() instead.
    return nullptr;
}

std::unique_ptr<SymbolLayout> RenderSymbolLayer::createLayout(const BucketParameters& parameters,
                                                              const std::vector<const RenderLayer*>& group,
                                                              std::unique_ptr<GeometryTileLayer> layer,
                                                              GlyphDependencies& glyphDependencies,
                                                              ImageDependencies& imageDependencies) const {
    return std::make_unique<SymbolLayout>(parameters,
                                          group,
                                          std::move(layer),
                                          imageDependencies,
                                          glyphDependencies);
}

void RenderSymbolLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderSymbolLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    auto hasIconOpacity = evaluated.get<style::IconColor>().constantOr(Color::black()).a > 0 ||
                          evaluated.get<style::IconHaloColor>().constantOr(Color::black()).a > 0;
    auto hasTextOpacity = evaluated.get<style::TextColor>().constantOr(Color::black()).a > 0 ||
                          evaluated.get<style::TextHaloColor>().constantOr(Color::black()).a > 0;

    passes = ((evaluated.get<style::IconOpacity>().constantOr(1) > 0 && hasIconOpacity && iconSize > 0)
              || (evaluated.get<style::TextOpacity>().constantOr(1) > 0 && hasTextOpacity && textSize > 0))
             ? RenderPass::Translucent : RenderPass::None;
}

bool RenderSymbolLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderSymbolLayer::render(PaintParameters& parameters, RenderSource*) {
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    for (const RenderTile& tile : renderTiles) {
        assert(dynamic_cast<SymbolBucket*>(tile.tile.getBucket(*baseImpl)));
        SymbolBucket& bucket = *reinterpret_cast<SymbolBucket*>(tile.tile.getBucket(*baseImpl));

        const auto& layout = bucket.layout;

        parameters.frameHistory.bind(parameters.context, 1);

        auto draw = [&] (auto& program,
                         auto&& uniformValues,
                         const auto& buffers,
                         const auto& symbolSizeBinder,
                         const SymbolPropertyValues& values_,
                         const auto& binders,
                         const auto& paintProperties)
        {
            // We clip symbols to their tile extent in still mode.
            const bool needsClipping = parameters.mapMode == MapMode::Still;

            program.get(paintProperties).draw(
                parameters.context,
                gl::Triangles(),
                values_.pitchAlignment == AlignmentType::Map
                    ? parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly)
                    : gl::DepthMode::disabled(),
                needsClipping
                    ? parameters.stencilModeForClipping(tile.clip)
                    : gl::StencilMode::disabled(),
                parameters.colorModeForRenderPass(),
                std::move(uniformValues),
                *buffers.vertexBuffer,
                *buffers.dynamicVertexBuffer,
                *symbolSizeBinder,
                *buffers.indexBuffer,
                buffers.segments,
                binders,
                paintProperties,
                parameters.state.getZoom(),
                getID()
            );
        };

        assert(dynamic_cast<GeometryTile*>(&tile.tile));
        GeometryTile& geometryTile = static_cast<GeometryTile&>(tile.tile);

        if (bucket.hasIconData()) {
            auto values = iconPropertyValues(layout);
            auto paintPropertyValues = iconPaintProperties();

            const bool alongLine = layout.get<SymbolPlacement>() == SymbolPlacementType::Line &&
                layout.get<IconRotationAlignment>() == AlignmentType::Map;

            if (alongLine) {
                reprojectLineLabels(bucket.icon.dynamicVertices,
                                    bucket.icon.placedSymbols,
                                    tile.matrix,
                                    values,
                                    tile,
                                    *bucket.iconSizeBinder,
                                    parameters.state,
                                    parameters.frameHistory);

                parameters.context.updateVertexBuffer(*bucket.icon.dynamicVertexBuffer, std::move(bucket.icon.dynamicVertices));
            }

            const bool iconScaled = layout.get<IconSize>().constantOr(1.0) != 1.0 || bucket.iconsNeedLinear;
            const bool iconTransformed = values.rotationAlignment == AlignmentType::Map || parameters.state.getPitch() != 0;

            parameters.context.bindTexture(*geometryTile.iconAtlasTexture, 0,
                bucket.sdfIcons || parameters.state.isChanging() || iconScaled || iconTransformed
                    ? gl::TextureFilter::Linear : gl::TextureFilter::Nearest);

            const Size texsize = geometryTile.iconAtlasTexture->size;

            if (bucket.sdfIcons) {
                if (values.hasHalo) {
                    draw(parameters.programs.symbolIconSDF,
                         SymbolSDFIconProgram::uniformValues(false, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, SymbolSDFPart::Halo),
                         bucket.icon,
                         bucket.iconSizeBinder,
                         values,
                         bucket.paintPropertyBinders.at(getID()).first,
                         paintPropertyValues);
                }

                if (values.hasFill) {
                    draw(parameters.programs.symbolIconSDF,
                         SymbolSDFIconProgram::uniformValues(false, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, SymbolSDFPart::Fill),
                         bucket.icon,
                         bucket.iconSizeBinder,
                         values,
                         bucket.paintPropertyBinders.at(getID()).first,
                         paintPropertyValues);
                }
            } else {
                draw(parameters.programs.symbolIcon,
                     SymbolIconProgram::uniformValues(false, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state),
                     bucket.icon,
                     bucket.iconSizeBinder,
                     values,
                     bucket.paintPropertyBinders.at(getID()).first,
                     paintPropertyValues);
            }
        }

        if (bucket.hasTextData()) {
            parameters.context.bindTexture(*geometryTile.glyphAtlasTexture, 0, gl::TextureFilter::Linear);

            auto values = textPropertyValues(layout);
            auto paintPropertyValues = textPaintProperties();

            const bool alongLine = layout.get<SymbolPlacement>() == SymbolPlacementType::Line &&
                layout.get<TextRotationAlignment>() == AlignmentType::Map;

            if (alongLine) {
                reprojectLineLabels(bucket.text.dynamicVertices,
                                    bucket.text.placedSymbols,
                                    tile.matrix,
                                    values,
                                    tile,
                                    *bucket.textSizeBinder,
                                    parameters.state,
                                    parameters.frameHistory);

                parameters.context.updateVertexBuffer(*bucket.text.dynamicVertexBuffer, std::move(bucket.text.dynamicVertices));
            }

            const Size texsize = geometryTile.glyphAtlasTexture->size;

            if (values.hasHalo) {
                draw(parameters.programs.symbolGlyph,
                     SymbolSDFTextProgram::uniformValues(true, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, SymbolSDFPart::Halo),
                     bucket.text,
                     bucket.textSizeBinder,
                     values,
                     bucket.paintPropertyBinders.at(getID()).second,
                     paintPropertyValues);
            }

            if (values.hasFill) {
                draw(parameters.programs.symbolGlyph,
                     SymbolSDFTextProgram::uniformValues(true, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, SymbolSDFPart::Fill),
                     bucket.text,
                     bucket.textSizeBinder,
                     values,
                     bucket.paintPropertyBinders.at(getID()).second,
                     paintPropertyValues);
            }
        }

        if (bucket.hasCollisionBoxData()) {
            static const style::Properties<>::PossiblyEvaluated properties {};
            static const CollisionBoxProgram::PaintPropertyBinders paintAttributeData(properties, 0);

            parameters.programs.collisionBox.draw(
                parameters.context,
                gl::Lines { 1.0f },
                gl::DepthMode::disabled(),
                parameters.stencilModeForClipping(tile.clip),
                parameters.colorModeForRenderPass(),
                CollisionBoxProgram::UniformValues {
                    uniforms::u_matrix::Value{ tile.matrix },
                    uniforms::u_scale::Value{ std::pow(2.0f, float(parameters.state.getZoom() - tile.tile.id.overscaledZ)) },
                    uniforms::u_zoom::Value{ float(parameters.state.getZoom() * 10) },
                    uniforms::u_maxzoom::Value{ float((tile.id.canonical.z + 1) * 10) },
                    uniforms::u_collision_y_stretch::Value{ tile.tile.yStretch() },
                    uniforms::u_camera_to_center_distance::Value{ parameters.state.getCameraToCenterDistance() },
                    uniforms::u_pitch::Value{ parameters.state.getPitch() },
                    uniforms::u_fadetexture::Value{ 1 }
                },
                *bucket.collisionBox.vertexBuffer,
                *bucket.collisionBox.indexBuffer,
                bucket.collisionBox.segments,
                paintAttributeData,
                properties,
                parameters.state.getZoom(),
                getID()
            );
        }
    }
}

style::IconPaintProperties::PossiblyEvaluated RenderSymbolLayer::iconPaintProperties() const {
    return style::IconPaintProperties::PossiblyEvaluated {
            evaluated.get<style::IconOpacity>(),
            evaluated.get<style::IconColor>(),
            evaluated.get<style::IconHaloColor>(),
            evaluated.get<style::IconHaloWidth>(),
            evaluated.get<style::IconHaloBlur>(),
            evaluated.get<style::IconTranslate>(),
            evaluated.get<style::IconTranslateAnchor>()
    };
}

style::TextPaintProperties::PossiblyEvaluated RenderSymbolLayer::textPaintProperties() const {
    return style::TextPaintProperties::PossiblyEvaluated {
            evaluated.get<style::TextOpacity>(),
            evaluated.get<style::TextColor>(),
            evaluated.get<style::TextHaloColor>(),
            evaluated.get<style::TextHaloWidth>(),
            evaluated.get<style::TextHaloBlur>(),
            evaluated.get<style::TextTranslate>(),
            evaluated.get<style::TextTranslateAnchor>()
    };
}


style::SymbolPropertyValues RenderSymbolLayer::iconPropertyValues(const style::SymbolLayoutProperties::PossiblyEvaluated& layout_) const {
    return style::SymbolPropertyValues {
            layout_.get<style::IconPitchAlignment>(),
            layout_.get<style::IconRotationAlignment>(),
            layout_.get<style::IconKeepUpright>(),
            evaluated.get<style::IconTranslate>(),
            evaluated.get<style::IconTranslateAnchor>(),
            evaluated.get<style::IconHaloColor>().constantOr(Color::black()).a > 0 &&
            evaluated.get<style::IconHaloWidth>().constantOr(1),
            evaluated.get<style::IconColor>().constantOr(Color::black()).a > 0,
            10.0f
    };
}

style::SymbolPropertyValues RenderSymbolLayer::textPropertyValues(const style::SymbolLayoutProperties::PossiblyEvaluated& layout_) const {
    // We hide line labels with viewport alignment as they move into the distance
    // because the approximations we use for drawing their glyphs get progressively worse
    // The "1.5" here means we start hiding them when the distance from the label
    // to the camera is 50% greater than the distance from the center of the map
    // to the camera. Depending on viewport properties, you might expect this to filter
    // the top third of the screen at pitch 60, and do almost nothing at pitch 45
    // "10" is effectively infinite at any pitch we support
    const bool limitMaxDistance =
        layout_.get<style::SymbolPlacement>() == style::SymbolPlacementType::Line
        && layout_.get<style::TextRotationAlignment>() == style::AlignmentType::Map
        && layout_.get<style::TextPitchAlignment>() == style::AlignmentType::Viewport;

    return style::SymbolPropertyValues {
            layout_.get<style::TextPitchAlignment>(),
            layout_.get<style::TextRotationAlignment>(),
            layout_.get<style::TextKeepUpright>(),
            evaluated.get<style::TextTranslate>(),
            evaluated.get<style::TextTranslateAnchor>(),
            evaluated.get<style::TextHaloColor>().constantOr(Color::black()).a > 0 &&
            evaluated.get<style::TextHaloWidth>().constantOr(1),
            evaluated.get<style::TextColor>().constantOr(Color::black()).a > 0,
            limitMaxDistance ? 1.5f : 10.0f
    };
}

} // namespace mbgl
