#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
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

    auto hasIconOpacity = evaluated.iconColor.constantOr(Color::black()).a > 0 ||
                          evaluated.iconHaloColor.constantOr(Color::black()).a > 0;
    auto hasTextOpacity = evaluated.textColor.constantOr(Color::black()).a > 0 ||
                          evaluated.textHaloColor.constantOr(Color::black()).a > 0;

    passes = ((evaluated.iconOpacity.constantOr(1) > 0 && hasIconOpacity && iconSize > 0)
              || (evaluated.textOpacity.constantOr(1) > 0 && hasTextOpacity && textSize > 0))
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
        auto bucket_ = tile.tile.getBucket<SymbolBucket>(*baseImpl);
        if (!bucket_) {
            continue;
        }
        SymbolBucket& bucket = *bucket_;

        const auto& layout = bucket.layout;

        auto draw = [&] (auto& program,
                         auto&& uniformValues,
                         const auto& buffers,
                         const auto& symbolSizeBinder,
                         const SymbolPropertyValues& values_,
                         const auto& binders,
                         const auto& paintProperties)
        {
            auto& programInstance = program.get(paintProperties);

            const auto allUniformValues = programInstance.computeAllUniformValues(
                std::move(uniformValues),
                *symbolSizeBinder,
                binders,
                paintProperties,
                parameters.state.getZoom()
            );
            const auto allAttributeBindings = programInstance.computeAllAttributeBindings(
                *buffers.vertexBuffer,
                *buffers.dynamicVertexBuffer,
                *buffers.opacityVertexBuffer,
                binders,
                paintProperties
            );

            checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

            programInstance.draw(
                parameters.context,
                gl::Triangles(),
                values_.pitchAlignment == AlignmentType::Map
                    ? parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly)
                    : gl::DepthMode::disabled(),
                gl::StencilMode::disabled(),
                parameters.colorModeForRenderPass(),
                *buffers.indexBuffer,
                buffers.segments,
                allUniformValues,
                allAttributeBindings,
                getID()
            );
        };

        assert(tile.tile.kind == Tile::Kind::Geometry);
        GeometryTile& geometryTile = static_cast<GeometryTile&>(tile.tile);

        if (bucket.hasIconData()) {
            auto values = iconPropertyValues(layout);
            auto paintPropertyValues = iconPaintProperties();

            const bool alongLine = layout.symbolPlacement != SymbolPlacementType::Point &&
                layout.iconRotationAlignment == AlignmentType::Map;

            if (alongLine) {
                reprojectLineLabels(bucket.icon.dynamicVertices,
                                    bucket.icon.placedSymbols,
                                    tile.matrix,
                                    values,
                                    tile,
                                    *bucket.iconSizeBinder,
                                    parameters.state);

                parameters.context.updateVertexBuffer(*bucket.icon.dynamicVertexBuffer, std::move(bucket.icon.dynamicVertices));
            }

            const bool iconScaled = layout.iconSize.constantOr(1.0) != 1.0 || bucket.iconsNeedLinear;
            const bool iconTransformed = values.rotationAlignment == AlignmentType::Map || parameters.state.getPitch() != 0;

            parameters.context.bindTexture(*geometryTile.iconAtlasTexture, 0,
                bucket.sdfIcons || parameters.state.isChanging() || iconScaled || iconTransformed
                    ? gl::TextureFilter::Linear : gl::TextureFilter::Nearest);

            const Size texsize = geometryTile.iconAtlasTexture->size;

            if (bucket.sdfIcons) {
                if (values.hasHalo) {
                    draw(parameters.programs.symbolIconSDF,
                         SymbolSDFIconProgram::uniformValues(false, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange, SymbolSDFPart::Halo),
                         bucket.icon,
                         bucket.iconSizeBinder,
                         values,
                         bucket.paintPropertyBinders.at(getID()).first,
                         paintPropertyValues);
                }

                if (values.hasFill) {
                    draw(parameters.programs.symbolIconSDF,
                         SymbolSDFIconProgram::uniformValues(false, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange, SymbolSDFPart::Fill),
                         bucket.icon,
                         bucket.iconSizeBinder,
                         values,
                         bucket.paintPropertyBinders.at(getID()).first,
                         paintPropertyValues);
                }
            } else {
                draw(parameters.programs.symbolIcon,
                     SymbolIconProgram::uniformValues(false, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange),
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

            const bool alongLine = layout.symbolPlacement != SymbolPlacementType::Point &&
                layout.textRotationAlignment == AlignmentType::Map;

            if (alongLine) {
                reprojectLineLabels(bucket.text.dynamicVertices,
                                    bucket.text.placedSymbols,
                                    tile.matrix,
                                    values,
                                    tile,
                                    *bucket.textSizeBinder,
                                    parameters.state);

                parameters.context.updateVertexBuffer(*bucket.text.dynamicVertexBuffer, std::move(bucket.text.dynamicVertices));
            }

            const Size texsize = geometryTile.glyphAtlasTexture->size;

            if (values.hasHalo) {
                draw(parameters.programs.symbolGlyph,
                     SymbolSDFTextProgram::uniformValues(true, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange, SymbolSDFPart::Halo),
                     bucket.text,
                     bucket.textSizeBinder,
                     values,
                     bucket.paintPropertyBinders.at(getID()).second,
                     paintPropertyValues);
            }

            if (values.hasFill) {
                draw(parameters.programs.symbolGlyph,
                     SymbolSDFTextProgram::uniformValues(true, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange, SymbolSDFPart::Fill),
                     bucket.text,
                     bucket.textSizeBinder,
                     values,
                     bucket.paintPropertyBinders.at(getID()).second,
                     paintPropertyValues);
            }
        }

        if (bucket.hasCollisionBoxData()) {
            auto pixelRatio = tile.id.pixelsToTileUnits(1, parameters.state.getZoom());
            const float scale = std::pow(2, parameters.state.getZoom() - tile.tile.id.overscaledZ);
            std::array<float,2> extrudeScale =
                {{
                    parameters.pixelsToGLUnits[0] / (pixelRatio * scale),
                    parameters.pixelsToGLUnits[1] / (pixelRatio * scale)
                    
                }};
            parameters.programs.collisionBox.draw(
                parameters.context,
                gl::Lines { 1.0f },
                gl::DepthMode::disabled(),
                gl::StencilMode::disabled(),
                parameters.colorModeForRenderPass(),
                CollisionBoxProgram::UniformValues {
                    uniforms::u_matrix::Value{ tile.matrix },
                    uniforms::u_extrude_scale::Value{ extrudeScale },
                    uniforms::u_camera_to_center_distance::Value{ parameters.state.getCameraToCenterDistance() }
                },
                *bucket.collisionBox.vertexBuffer,
                *bucket.collisionBox.dynamicVertexBuffer,
                *bucket.collisionBox.indexBuffer,
                bucket.collisionBox.segments,
                getID()
            );
        }
        if (bucket.hasCollisionCircleData()) {
            auto pixelRatio = tile.id.pixelsToTileUnits(1, parameters.state.getZoom());
            const float scale = std::pow(2, parameters.state.getZoom() - tile.tile.id.overscaledZ);
            std::array<float,2> extrudeScale =
                {{
                    parameters.pixelsToGLUnits[0] / (pixelRatio * scale),
                    parameters.pixelsToGLUnits[1] / (pixelRatio * scale)
                    
                }};

            parameters.programs.collisionCircle.draw(
                parameters.context,
                gl::Triangles(),
                gl::DepthMode::disabled(),
                gl::StencilMode::disabled(),
                parameters.colorModeForRenderPass(),
                CollisionCircleProgram::UniformValues {
                    uniforms::u_matrix::Value{ tile.matrix },
                    uniforms::u_extrude_scale::Value{ extrudeScale },
                    uniforms::u_overscale_factor::Value{ float(tile.tile.id.overscaleFactor()) },
                    uniforms::u_camera_to_center_distance::Value{ parameters.state.getCameraToCenterDistance() }
                },
                *bucket.collisionCircle.vertexBuffer,
                *bucket.collisionCircle.dynamicVertexBuffer,
                *bucket.collisionCircle.indexBuffer,
                bucket.collisionCircle.segments,
                getID()
            );

        }
    }
}

style::IconPaintProperties::PossiblyEvaluated RenderSymbolLayer::iconPaintProperties() const {
    return style::IconPaintProperties::PossiblyEvaluated {
        evaluated.iconOpacity,
        evaluated.iconColor,
        evaluated.iconHaloColor,
        evaluated.iconHaloWidth,
        evaluated.iconHaloBlur,
        evaluated.iconTranslate,
        evaluated.iconTranslateAnchor
    };
}

style::TextPaintProperties::PossiblyEvaluated RenderSymbolLayer::textPaintProperties() const {
    return style::TextPaintProperties::PossiblyEvaluated {
        evaluated.textOpacity,
        evaluated.textColor,
        evaluated.textHaloColor,
        evaluated.textHaloWidth,
        evaluated.textHaloBlur,
        evaluated.textTranslate,
        evaluated.textTranslateAnchor
    };
}


style::SymbolPropertyValues RenderSymbolLayer::iconPropertyValues(const style::SymbolLayoutProperties::PossiblyEvaluated& layout_) const {
    return style::SymbolPropertyValues {
        layout_.iconPitchAlignment,
        layout_.iconRotationAlignment,
        layout_.iconKeepUpright,
        evaluated.iconTranslate,
        evaluated.iconTranslateAnchor,
        evaluated.iconHaloColor.constantOr(Color::black()).a > 0 &&
        evaluated.iconHaloWidth.constantOr(1),
        evaluated.iconColor.constantOr(Color::black()).a > 0
    };
}

style::SymbolPropertyValues RenderSymbolLayer::textPropertyValues(const style::SymbolLayoutProperties::PossiblyEvaluated& layout_) const {
    return style::SymbolPropertyValues {
        layout_.textPitchAlignment,
        layout_.textRotationAlignment,
        layout_.textKeepUpright,
        evaluated.textTranslate,
        evaluated.textTranslateAnchor,
        evaluated.textHaloColor.constantOr(Color::black()).a > 0 &&
        evaluated.textHaloWidth.constantOr(1),
        evaluated.textColor.constantOr(Color::black()).a > 0
    };
}

} // namespace mbgl
