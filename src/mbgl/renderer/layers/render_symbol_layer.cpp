#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/programs/collision_box_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/layout/symbol_projection.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

namespace mbgl {

using namespace style;

namespace {
Point<float> calculateVariableRenderShift(style::SymbolAnchorType anchor, float width, float height, float radialOffset, float textBoxScale, float renderTextSize) {
    AnchorAlignment alignment = AnchorAlignment::getAnchorAlignment(anchor);
    float shiftX = -(alignment.horizontalAlign - 0.5f) * width;
    float shiftY = -(alignment.verticalAlign - 0.5f) * height;
    Point<float> offset = SymbolLayout::evaluateRadialOffset(anchor, radialOffset);
    return Point<float>(
        (shiftX / textBoxScale + offset.x) * renderTextSize,
        (shiftY / textBoxScale + offset.y) * renderTextSize
    );
}
} // namespace

RenderSymbolLayer::RenderSymbolLayer(Immutable<style::SymbolLayer::Impl> _impl)
    : RenderLayer(std::move(_impl)),
      unevaluated(impl().paint.untransitioned()) {
}

const style::SymbolLayer::Impl& RenderSymbolLayer::impl() const {
    return static_cast<const style::SymbolLayer::Impl&>(*baseImpl);
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

bool RenderSymbolLayer::hasCrossfade() const {
    return false;
}

const std::string& RenderSymbolLayer::layerID() const {
    return RenderLayer::getID();
}

const RenderLayerSymbolInterface* RenderSymbolLayer::getSymbolInterface() const {
    return this;
}

const std::vector<std::reference_wrapper<RenderTile>>& RenderSymbolLayer::getRenderTiles() const {
    return renderTiles;
}

SymbolBucket* RenderSymbolLayer::getSymbolBucket(const RenderTile& renderTile) const {
    return renderTile.tile.getBucket<SymbolBucket>(*baseImpl);
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
        assert(bucket.paintProperties.find(getID()) != bucket.paintProperties.end());
        const auto& bucketPaintProperties = bucket.paintProperties.at(getID());
        const auto& evaluated_ = bucketPaintProperties.evaluated;
        const auto& layout = bucket.layout;

        auto draw = [&] (auto& programInstance,
                         auto&& uniformValues,
                         const auto& buffers,
                         const auto& symbolSizeBinder,
                         const SymbolPropertyValues& values_,
                         const auto& binders,
                         const auto& paintProperties,
                         auto&& textureBindings) {
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
                *parameters.renderPass,
                gfx::Triangles(),
                values_.pitchAlignment == AlignmentType::Map
                    ? parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly)
                    : gfx::DepthMode::disabled(),
                gfx::StencilMode::disabled(),
                parameters.colorModeForRenderPass(),
                gfx::CullFaceMode::disabled(),
                *buffers.indexBuffer,
                buffers.segments,
                allUniformValues,
                allAttributeBindings,
                std::move(textureBindings),
                getID()
            );
        };

        assert(tile.tile.kind == Tile::Kind::Geometry);
        auto& geometryTile = static_cast<GeometryTile&>(tile.tile);

        if (bucket.hasIconData()) {
            auto values = iconPropertyValues(evaluated_, layout);
            const auto& paintPropertyValues = iconPaintProperties(evaluated_);

            const bool alongLine = layout.get<SymbolPlacement>() != SymbolPlacementType::Point &&
                layout.get<IconRotationAlignment>() == AlignmentType::Map;

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

            const bool iconScaled = layout.get<IconSize>().constantOr(1.0) != 1.0 || bucket.iconsNeedLinear;
            const bool iconTransformed = values.rotationAlignment == AlignmentType::Map || parameters.state.getPitch() != 0;

            const gfx::TextureBinding textureBinding{ geometryTile.iconAtlasTexture->getResource(),
                                                      bucket.sdfIcons ||
                                                              parameters.state.isChanging() ||
                                                              iconScaled || iconTransformed
                                                          ? gfx::TextureFilterType::Linear
                                                          : gfx::TextureFilterType::Nearest };

            const Size texsize = geometryTile.iconAtlasTexture->size;

            if (bucket.sdfIcons) {
                if (values.hasHalo) {
                    draw(parameters.programs.getSymbolLayerPrograms().symbolIconSDF,
                         SymbolSDFIconProgram::layoutUniformValues(false, false, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange, SymbolSDFPart::Halo),
                         bucket.icon,
                         bucket.iconSizeBinder,
                         values,
                         bucketPaintProperties.iconBinders,
                         paintPropertyValues,
                         SymbolSDFIconProgram::TextureBindings{
                             textureBinding,
                         });
                }

                if (values.hasFill) {
                    draw(parameters.programs.getSymbolLayerPrograms().symbolIconSDF,
                         SymbolSDFIconProgram::layoutUniformValues(false, false, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange, SymbolSDFPart::Fill),
                         bucket.icon,
                         bucket.iconSizeBinder,
                         values,
                         bucketPaintProperties.iconBinders,
                         paintPropertyValues,
                         SymbolSDFIconProgram::TextureBindings{
                             textureBinding,
                         });
                }
            } else {
                draw(parameters.programs.getSymbolLayerPrograms().symbolIcon,
                     SymbolIconProgram::layoutUniformValues(false, false, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange),
                     bucket.icon,
                     bucket.iconSizeBinder,
                     values,
                     bucketPaintProperties.iconBinders,
                     paintPropertyValues,
                     SymbolIconProgram::TextureBindings{
                         textureBinding,
                     });
            }
        }

        if (bucket.hasTextData()) {
            const gfx::TextureBinding textureBinding{ geometryTile.glyphAtlasTexture->getResource(),
                                                      gfx::TextureFilterType::Linear };

            auto values = textPropertyValues(evaluated_, layout);
            const auto& paintPropertyValues = textPaintProperties(evaluated_);
            bool hasVariablePacement = false;

            const bool alongLine = layout.get<SymbolPlacement>() != SymbolPlacementType::Point &&
                layout.get<TextRotationAlignment>() == AlignmentType::Map;

            if (alongLine) {
                reprojectLineLabels(bucket.text.dynamicVertices,
                                    bucket.text.placedSymbols,
                                    tile.matrix,
                                    values,
                                    tile,
                                    *bucket.textSizeBinder,
                                    parameters.state);

                parameters.context.updateVertexBuffer(*bucket.text.dynamicVertexBuffer, std::move(bucket.text.dynamicVertices));
            } else if (!layout.get<TextVariableAnchor>().empty()) {
                bucket.text.dynamicVertices.clear();

                const auto partiallyEvaluatedSize = bucket.textSizeBinder->evaluateForZoom(parameters.state.getZoom());
                const float tileScale = std::pow(2, parameters.state.getZoom() - tile.tile.id.overscaledZ);
                const bool rotateWithMap = layout.get<TextRotationAlignment>() == AlignmentType::Map;
                const bool pitchWithMap = layout.get<TextPitchAlignment>() == AlignmentType::Map;
                const float pixelsToTileUnits = tile.id.pixelsToTileUnits(1.0, parameters.state.getZoom());
                const auto labelPlaneMatrix = getLabelPlaneMatrix(tile.matrix, pitchWithMap, rotateWithMap, parameters.state, pixelsToTileUnits);

                for (const PlacedSymbol& symbol : bucket.text.placedSymbols) {
                    optional<VariableOffset> variableOffset;
                    if (!symbol.hidden && symbol.crossTileID != 0u) {
                        auto it = parameters.variableOffsets.get().find(symbol.crossTileID);
                        if (it != parameters.variableOffsets.get().end()) {
                            variableOffset = it->second;
                            hasVariablePacement |= true;
                        }
                    }

                    if (!variableOffset) {
                        // These symbols are from a justification that is not being used, or a label that wasn't placed
                        // so we don't need to do the extra math to figure out what incremental shift to apply.
                        hideGlyphs(symbol.glyphOffsets.size(), bucket.text.dynamicVertices);
                    } else {
                        const Point<float> tileAnchor = symbol.anchorPoint;
                        const auto projectedAnchor = project(tileAnchor, pitchWithMap ? tile.matrix : labelPlaneMatrix);
                        const float perspectiveRatio = 0.5f + 0.5f * (parameters.state.getCameraToCenterDistance() / projectedAnchor.second);
                        float renderTextSize = evaluateSizeForFeature(partiallyEvaluatedSize, symbol) * perspectiveRatio / util::ONE_EM;
                        if (pitchWithMap) {
                            // Go from size in pixels to equivalent size in tile units
                            renderTextSize *= bucket.tilePixelRatio / tileScale;
                        }

                        auto shift = calculateVariableRenderShift(
                                (*variableOffset).anchor,
                                (*variableOffset).width,
                                (*variableOffset).height,
                                (*variableOffset).radialOffset,
                                (*variableOffset).textBoxScale,
                                renderTextSize);

                        // Usual case is that we take the projected anchor and add the pixel-based shift
                        // calculated above. In the (somewhat weird) case of pitch-aligned text, we add an equivalent
                        // tile-unit based shift to the anchor before projecting to the label plane.
                        Point<float> shiftedAnchor;
                        if (pitchWithMap) {
                            shiftedAnchor = project(Point<float>(tileAnchor.x + shift.x, tileAnchor.y + shift.y),
                                                    labelPlaneMatrix).first;
                        } else {
                            if (rotateWithMap) {
                                auto rotated = util::rotate(shift, -parameters.state.getPitch());
                                shiftedAnchor = Point<float>(projectedAnchor.first.x + rotated.x,
                                                             projectedAnchor.first.y + rotated.y);
                            } else {
                                 shiftedAnchor = Point<float>(projectedAnchor.first.x + shift.x,
                                                              projectedAnchor.first.y + shift.y);
                            }
                        }

                        for (std::size_t i = 0; i < symbol.glyphOffsets.size(); i++) {
                            addDynamicAttributes(shiftedAnchor, 0, bucket.text.dynamicVertices);
                        }
                    }
                }
                parameters.context.updateVertexBuffer(*bucket.text.dynamicVertexBuffer, std::move(bucket.text.dynamicVertices));
            }

            const Size texsize = geometryTile.glyphAtlasTexture->size;

            if (values.hasHalo) {
                draw(parameters.programs.getSymbolLayerPrograms().symbolGlyph,
                     SymbolSDFTextProgram::layoutUniformValues(true, hasVariablePacement, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange, SymbolSDFPart::Halo),
                     bucket.text,
                     bucket.textSizeBinder,
                     values,
                     bucketPaintProperties.textBinders,
                     paintPropertyValues,
                     SymbolSDFTextProgram::TextureBindings{
                         textureBinding,
                     });
            }

            if (values.hasFill) {
                draw(parameters.programs.getSymbolLayerPrograms().symbolGlyph,
                     SymbolSDFTextProgram::layoutUniformValues(true, hasVariablePacement, values, texsize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange, SymbolSDFPart::Fill),
                     bucket.text,
                     bucket.textSizeBinder,
                     values,
                     bucketPaintProperties.textBinders,
                     paintPropertyValues,
                     SymbolSDFTextProgram::TextureBindings{
                         textureBinding,
                     });
            }
        }

        if (bucket.hasCollisionBoxData()) {
            static const style::Properties<>::PossiblyEvaluated properties {};
            static const CollisionBoxProgram::Binders paintAttributeData(properties, 0);

            auto pixelRatio = tile.id.pixelsToTileUnits(1, parameters.state.getZoom());
            const float scale = std::pow(2, parameters.state.getZoom() - tile.tile.id.overscaledZ);
            std::array<float,2> extrudeScale =
                {{
                    parameters.pixelsToGLUnits[0] / (pixelRatio * scale),
                    parameters.pixelsToGLUnits[1] / (pixelRatio * scale)
                    
                }};
            parameters.programs.getSymbolLayerPrograms().collisionBox.draw(
                parameters.context,
                *parameters.renderPass,
                gfx::Lines { 1.0f },
                gfx::DepthMode::disabled(),
                gfx::StencilMode::disabled(),
                parameters.colorModeForRenderPass(),
                gfx::CullFaceMode::disabled(),
                CollisionBoxProgram::LayoutUniformValues {
                    uniforms::matrix::Value( tile.matrix ),
                    uniforms::extrude_scale::Value( extrudeScale ),
                    uniforms::camera_to_center_distance::Value( parameters.state.getCameraToCenterDistance() )
                },
                *bucket.collisionBox.vertexBuffer,
                *bucket.collisionBox.dynamicVertexBuffer,
                *bucket.collisionBox.indexBuffer,
                bucket.collisionBox.segments,
                paintAttributeData,
                properties,
                CollisionBoxProgram::TextureBindings{},
                parameters.state.getZoom(),
                getID()
            );
        }
        if (bucket.hasCollisionCircleData()) {
            static const style::Properties<>::PossiblyEvaluated properties {};
            static const CollisionBoxProgram::Binders paintAttributeData(properties, 0);

            auto pixelRatio = tile.id.pixelsToTileUnits(1, parameters.state.getZoom());
            const float scale = std::pow(2, parameters.state.getZoom() - tile.tile.id.overscaledZ);
            std::array<float,2> extrudeScale =
                {{
                    parameters.pixelsToGLUnits[0] / (pixelRatio * scale),
                    parameters.pixelsToGLUnits[1] / (pixelRatio * scale)
                    
                }};

            parameters.programs.getSymbolLayerPrograms().collisionCircle.draw(
                parameters.context,
                *parameters.renderPass,
                gfx::Triangles(),
                gfx::DepthMode::disabled(),
                gfx::StencilMode::disabled(),
                parameters.colorModeForRenderPass(),
                gfx::CullFaceMode::disabled(),
                CollisionCircleProgram::LayoutUniformValues {
                    uniforms::matrix::Value( tile.matrix ),
                    uniforms::extrude_scale::Value( extrudeScale ),
                    uniforms::overscale_factor::Value( float(tile.tile.id.overscaleFactor()) ),
                    uniforms::camera_to_center_distance::Value( parameters.state.getCameraToCenterDistance() )
                },
                *bucket.collisionCircle.vertexBuffer,
                *bucket.collisionCircle.dynamicVertexBuffer,
                *bucket.collisionCircle.indexBuffer,
                bucket.collisionCircle.segments,
                paintAttributeData,
                properties,
                CollisionCircleProgram::TextureBindings{},
                parameters.state.getZoom(),
                getID()
            );

        }
    }
}

// static
style::IconPaintProperties::PossiblyEvaluated RenderSymbolLayer::iconPaintProperties(const style::SymbolPaintProperties::PossiblyEvaluated& evaluated_) {
    return style::IconPaintProperties::PossiblyEvaluated {
            evaluated_.get<style::IconOpacity>(),
            evaluated_.get<style::IconColor>(),
            evaluated_.get<style::IconHaloColor>(),
            evaluated_.get<style::IconHaloWidth>(),
            evaluated_.get<style::IconHaloBlur>(),
            evaluated_.get<style::IconTranslate>(),
            evaluated_.get<style::IconTranslateAnchor>()
    };
}

// static
style::TextPaintProperties::PossiblyEvaluated RenderSymbolLayer::textPaintProperties(const style::SymbolPaintProperties::PossiblyEvaluated& evaluated_) {
    return style::TextPaintProperties::PossiblyEvaluated {
            evaluated_.get<style::TextOpacity>(),
            evaluated_.get<style::TextColor>(),
            evaluated_.get<style::TextHaloColor>(),
            evaluated_.get<style::TextHaloWidth>(),
            evaluated_.get<style::TextHaloBlur>(),
            evaluated_.get<style::TextTranslate>(),
            evaluated_.get<style::TextTranslateAnchor>()
    };
}

// static
style::SymbolPropertyValues RenderSymbolLayer::iconPropertyValues(const style::SymbolPaintProperties::PossiblyEvaluated& evaluated_,
                                                                  const style::SymbolLayoutProperties::PossiblyEvaluated& layout_) {
    return style::SymbolPropertyValues {
            layout_.get<style::IconPitchAlignment>(),
            layout_.get<style::IconRotationAlignment>(),
            layout_.get<style::IconKeepUpright>(),
            evaluated_.get<style::IconTranslate>(),
            evaluated_.get<style::IconTranslateAnchor>(),
            evaluated_.get<style::IconHaloColor>().constantOr(Color::black()).a > 0 &&
            evaluated_.get<style::IconHaloWidth>().constantOr(1),
            evaluated_.get<style::IconColor>().constantOr(Color::black()).a > 0
    };
}

// static
style::SymbolPropertyValues RenderSymbolLayer::textPropertyValues(const style::SymbolPaintProperties::PossiblyEvaluated& evaluated_,
                                                                  const style::SymbolLayoutProperties::PossiblyEvaluated& layout_) {
    return style::SymbolPropertyValues {
            layout_.get<style::TextPitchAlignment>(),
            layout_.get<style::TextRotationAlignment>(),
            layout_.get<style::TextKeepUpright>(),
            evaluated_.get<style::TextTranslate>(),
            evaluated_.get<style::TextTranslateAnchor>(),
            evaluated_.get<style::TextHaloColor>().constantOr(Color::black()).a > 0 &&
            evaluated_.get<style::TextHaloWidth>().constantOr(1),
            evaluated_.get<style::TextColor>().constantOr(Color::black()).a > 0
    };
}

void RenderSymbolLayer::setRenderTiles(RenderTiles tiles, const TransformState& state) {
    auto filterFn = [](auto& tile){ return !tile.tile.isRenderable(); };
    renderTiles = RenderLayer::filterRenderTiles(std::move(tiles), filterFn);
    // Sort symbol tiles in opposite y position, so tiles with overlapping symbols are drawn
    // on top of each other, with lower symbols being drawn on top of higher symbols.
    std::sort(renderTiles.begin(), renderTiles.end(), [&state](const auto& a, const auto& b) {
        Point<float> pa(a.get().id.canonical.x, a.get().id.canonical.y);
        Point<float> pb(b.get().id.canonical.x, b.get().id.canonical.y);

        auto par = util::rotate(pa, state.getBearing());
        auto pbr = util::rotate(pb, state.getBearing());

        return std::tie(b.get().id.canonical.z, par.y, par.x) < std::tie(a.get().id.canonical.z, pbr.y, pbr.x);
    });
}

void RenderSymbolLayer::updateBucketPaintProperties(Bucket* bucket) const {
    assert(bucket->supportsLayer(*baseImpl));
    static_cast<SymbolBucket*>(bucket)->updatePaintProperties(getID(), evaluated);
}

} // namespace mbgl
