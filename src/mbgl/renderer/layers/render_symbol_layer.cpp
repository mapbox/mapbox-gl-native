#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/upload_parameters.hpp>
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
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>
#include <set>

namespace mbgl {

using namespace style;
namespace {

style::SymbolPropertyValues iconPropertyValues(const style::SymbolPaintProperties::PossiblyEvaluated& evaluated_,
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


style::SymbolPropertyValues textPropertyValues(const style::SymbolPaintProperties::PossiblyEvaluated& evaluated_,
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

using SegmentWrapper = std::reference_wrapper<Segment<SymbolTextAttributes>>;
using SegmentVectorWrapper = std::reference_wrapper<SegmentVector<SymbolTextAttributes>>;
using SegmentsWrapper = variant<SegmentWrapper, SegmentVectorWrapper>;

struct RenderableSegment {
    RenderableSegment(SegmentWrapper segment_,
                      const RenderTile& tile_,
                      const LayerRenderData& renderData_,
                      const SymbolBucket::PaintProperties& bucketPaintProperties_,
                      float sortKey_,
                      const SymbolType type_)
        : segment(segment_),
          tile(tile_),
          renderData(renderData_),
          bucketPaintProperties(bucketPaintProperties_),
          sortKey(sortKey_),
          type(type_) {}

    SegmentWrapper segment;
    const RenderTile& tile;
    const LayerRenderData& renderData;
    const SymbolBucket::PaintProperties& bucketPaintProperties;
    float sortKey;
    SymbolType type;

    friend bool operator < (const RenderableSegment& lhs, const RenderableSegment& rhs) {
        // Sort renderable segments by a sort key.
        if (lhs.sortKey < rhs.sortKey) {
            return true;
        }

        // In cases when sort key is the same, sort by the type of a segment (text over icons),
        // and for segments of the same type with the same sort key, sort by a tile id.
        if (lhs.sortKey == rhs.sortKey) {
            if (lhs.type != SymbolType::Text && rhs.type == SymbolType::Text) {
                return true;
            }

            if (lhs.type == rhs.type)  {
                return lhs.tile.id < rhs.tile.id;
            }
        }

        return false;
    }
};

template <typename DrawFn>
void drawIcon(const DrawFn& draw,
              const RenderTile& tile,
              const LayerRenderData& renderData,
              SegmentsWrapper iconSegments,
              const SymbolBucket::PaintProperties& bucketPaintProperties,
              const PaintParameters& parameters,
              const bool sdfIcons) {
    auto& bucket = static_cast<SymbolBucket&>(*renderData.bucket);
    const auto& evaluated = getEvaluated<SymbolLayerProperties>(renderData.layerProperties);
    const auto& layout = *bucket.layout;
    auto values = iconPropertyValues(evaluated, layout);
    const auto& paintPropertyValues = RenderSymbolLayer::iconPaintProperties(evaluated);

    const bool alongLine = layout.get<SymbolPlacement>() != SymbolPlacementType::Point &&
        layout.get<IconRotationAlignment>() == AlignmentType::Map;

    const bool iconScaled = layout.get<IconSize>().constantOr(1.0) != 1.0 || bucket.iconsNeedLinear;
    const bool iconTransformed = values.rotationAlignment == AlignmentType::Map || parameters.state.getPitch() != 0;

    const gfx::TextureBinding textureBinding{ tile.getIconAtlasTexture().getResource(),
                                            sdfIcons ||
                                                    parameters.state.isChanging() ||
                                                    iconScaled || iconTransformed
                                                ? gfx::TextureFilterType::Linear
                                                : gfx::TextureFilterType::Nearest };

    const Size& iconSize = tile.getIconAtlasTexture().size;
    const bool variablePlacedIcon = bucket.hasVariablePlacement && layout.get<IconTextFit>() != IconTextFitType::None;

    if (sdfIcons) {
        if (values.hasHalo) {
            draw(parameters.programs.getSymbolLayerPrograms().symbolIconSDF,
                SymbolSDFIconProgram::layoutUniformValues(false, variablePlacedIcon, values, iconSize, parameters.pixelsToGLUnits, parameters.pixelRatio, alongLine, tile, parameters.state, parameters.symbolFadeChange, SymbolSDFPart::Halo),
                bucket.sdfIcon,
                iconSegments,
                bucket.iconSizeBinder,
                bucketPaintProperties.iconBinders,
                paintPropertyValues,
                SymbolSDFIconProgram::TextureBindings{
                    textureBinding
                },
                "halo");
        }

        if (values.hasFill) {
            draw(parameters.programs.getSymbolLayerPrograms().symbolIconSDF,
                SymbolSDFIconProgram::layoutUniformValues(false, variablePlacedIcon, values, iconSize, parameters.pixelsToGLUnits, parameters.pixelRatio, alongLine, tile, parameters.state, parameters.symbolFadeChange, SymbolSDFPart::Fill),
                bucket.sdfIcon,
                iconSegments,
                bucket.iconSizeBinder,
                bucketPaintProperties.iconBinders,
                paintPropertyValues,
                SymbolSDFIconProgram::TextureBindings{
                    textureBinding
                },
                "fill");
        }
    } else {
        draw(parameters.programs.getSymbolLayerPrograms().symbolIcon,
            SymbolIconProgram::layoutUniformValues(false, variablePlacedIcon, values, iconSize, parameters.pixelsToGLUnits, alongLine, tile, parameters.state, parameters.symbolFadeChange),
            bucket.icon,
            iconSegments,
            bucket.iconSizeBinder,
            bucketPaintProperties.iconBinders,
            paintPropertyValues,
            SymbolIconProgram::TextureBindings{
                textureBinding
            },
            "icon");
    }
}

template <typename DrawFn>
void drawText(const DrawFn& draw,
              const RenderTile& tile,
              const LayerRenderData& renderData,
              SegmentsWrapper textSegments,
              const SymbolBucket::PaintProperties& bucketPaintProperties,
              const PaintParameters& parameters) {
    auto& bucket = static_cast<SymbolBucket&>(*renderData.bucket);
    const auto& evaluated = getEvaluated<SymbolLayerProperties>(renderData.layerProperties);
    const auto& layout = *bucket.layout;

    auto values = textPropertyValues(evaluated, layout);
    const auto& paintPropertyValues = RenderSymbolLayer::textPaintProperties(evaluated);

    const bool alongLine = layout.get<SymbolPlacement>() != SymbolPlacementType::Point &&
        layout.get<TextRotationAlignment>() == AlignmentType::Map;

    const Size& glyphTexSize = tile.getGlyphAtlasTexture().size;
    const gfx::TextureBinding glyphTextureBinding{tile.getGlyphAtlasTexture().getResource(),
                                                  gfx::TextureFilterType::Linear};

    const auto drawGlyphs = [&](auto& program, const auto& uniforms, const auto& textures, SymbolSDFPart part) {
        draw(program,
             uniforms,
             bucket.text,
             textSegments,
             bucket.textSizeBinder,
             bucketPaintProperties.textBinders,
             paintPropertyValues,
             textures,
             (part == SymbolSDFPart::Halo) ? "halo" : "fill");
    };

    if (bucket.iconsInText) {
        const ZoomEvaluatedSize partiallyEvaluatedTextSize =
            bucket.textSizeBinder->evaluateForZoom(parameters.state.getZoom());
        const bool transformed = values.rotationAlignment == AlignmentType::Map || parameters.state.getPitch() != 0;
        const Size& iconTexSize = tile.getIconAtlasTexture().size;
        const gfx::TextureBinding iconTextureBinding{
            tile.getIconAtlasTexture().getResource(),
            parameters.state.isChanging() || transformed || !partiallyEvaluatedTextSize.isZoomConstant
                ? gfx::TextureFilterType::Linear
                : gfx::TextureFilterType::Nearest};
        if (values.hasHalo) {
            drawGlyphs(parameters.programs.getSymbolLayerPrograms().symbolTextAndIcon,
                       SymbolTextAndIconProgram::layoutUniformValues(bucket.hasVariablePlacement,
                                                                     values,
                                                                     glyphTexSize,
                                                                     iconTexSize,
                                                                     parameters.pixelsToGLUnits,
                                                                     parameters.pixelRatio,
                                                                     alongLine,
                                                                     tile,
                                                                     parameters.state,
                                                                     parameters.symbolFadeChange,
                                                                     SymbolSDFPart::Halo),
                       SymbolTextAndIconProgram::TextureBindings{glyphTextureBinding, iconTextureBinding},
                       SymbolSDFPart::Halo);
        }

        if (values.hasFill) {
            drawGlyphs(parameters.programs.getSymbolLayerPrograms().symbolTextAndIcon,
                       SymbolTextAndIconProgram::layoutUniformValues(bucket.hasVariablePlacement,
                                                                     values,
                                                                     glyphTexSize,
                                                                     iconTexSize,
                                                                     parameters.pixelsToGLUnits,
                                                                     parameters.pixelRatio,
                                                                     alongLine,
                                                                     tile,
                                                                     parameters.state,
                                                                     parameters.symbolFadeChange,
                                                                     SymbolSDFPart::Fill),
                       SymbolTextAndIconProgram::TextureBindings{glyphTextureBinding, iconTextureBinding},
                       SymbolSDFPart::Fill);
        }
    } else {
        if (values.hasHalo) {
            drawGlyphs(parameters.programs.getSymbolLayerPrograms().symbolGlyph,
                       SymbolSDFTextProgram::layoutUniformValues(true,
                                                                 bucket.hasVariablePlacement,
                                                                 values,
                                                                 glyphTexSize,
                                                                 parameters.pixelsToGLUnits,
                                                                 parameters.pixelRatio,
                                                                 alongLine,
                                                                 tile,
                                                                 parameters.state,
                                                                 parameters.symbolFadeChange,
                                                                 SymbolSDFPart::Halo),
                       SymbolSDFTextProgram::TextureBindings{glyphTextureBinding},
                       SymbolSDFPart::Halo);
        }

        if (values.hasFill) {
            drawGlyphs(parameters.programs.getSymbolLayerPrograms().symbolGlyph,
                       SymbolSDFTextProgram::layoutUniformValues(true,
                                                                 bucket.hasVariablePlacement,
                                                                 values,
                                                                 glyphTexSize,
                                                                 parameters.pixelsToGLUnits,
                                                                 parameters.pixelRatio,
                                                                 alongLine,
                                                                 tile,
                                                                 parameters.state,
                                                                 parameters.symbolFadeChange,
                                                                 SymbolSDFPart::Fill),
                       SymbolSDFTextProgram::TextureBindings{glyphTextureBinding},
                       SymbolSDFPart::Fill);
        }
    }
}

inline const SymbolLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == SymbolLayer::Impl::staticTypeInfo());
    return static_cast<const SymbolLayer::Impl&>(*impl);
}

} // namespace

RenderSymbolLayer::RenderSymbolLayer(Immutable<style::SymbolLayer::Impl> _impl)
    : RenderLayer(makeMutable<SymbolLayerProperties>(std::move(_impl))),
      unevaluated(impl(baseImpl).paint.untransitioned()) {
}

RenderSymbolLayer::~RenderSymbolLayer() = default;

void RenderSymbolLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
    hasFormatSectionOverrides = SymbolLayerPaintPropertyOverrides::hasOverrides(impl(baseImpl).layout.get<TextField>());
}

void RenderSymbolLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    auto properties = makeMutable<SymbolLayerProperties>(
        staticImmutableCast<SymbolLayer::Impl>(baseImpl),
        unevaluated.evaluate(parameters));
    auto& evaluated = properties->evaluated;
    auto& layout = impl(baseImpl).layout;

    if (hasFormatSectionOverrides) {
        SymbolLayerPaintPropertyOverrides::setOverrides(layout, evaluated);
    }

    auto hasIconOpacity = evaluated.get<style::IconColor>().constantOr(Color::black()).a > 0 ||
                          evaluated.get<style::IconHaloColor>().constantOr(Color::black()).a > 0;
    auto hasTextOpacity = evaluated.get<style::TextColor>().constantOr(Color::black()).a > 0 ||
                          evaluated.get<style::TextHaloColor>().constantOr(Color::black()).a > 0;

    passes = ((evaluated.get<style::IconOpacity>().constantOr(1) > 0 && hasIconOpacity && iconSize > 0)
              || (evaluated.get<style::TextOpacity>().constantOr(1) > 0 && hasTextOpacity && textSize > 0))
             ? RenderPass::Translucent : RenderPass::None;
    properties->renderPasses = mbgl::underlying_type(passes);
    evaluatedProperties = std::move(properties);
}

bool RenderSymbolLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderSymbolLayer::hasCrossfade() const {
    return false;
}

void RenderSymbolLayer::render(PaintParameters& parameters) {
    assert(renderTiles);
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    const bool sortFeaturesByKey = !impl(baseImpl).layout.get<SymbolSortKey>().isUndefined();
    std::multiset<RenderableSegment> renderableSegments;

    const auto draw = [&parameters, this] (auto& programInstance,
                                           const auto& uniformValues,
                                           const auto& buffers,
                                           auto& segments,
                                           const auto& symbolSizeBinder,
                                           const auto& binders,
                                           const auto& paintProperties,
                                           const auto& textureBindings,
                                           const std::string& suffix) {
        const auto allUniformValues = programInstance.computeAllUniformValues(
            uniformValues,
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

        this->checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

        segments.match(
            [&](const std::reference_wrapper<Segment<SymbolTextAttributes>>& segment) {
                programInstance.draw(
                    parameters.context,
                    *parameters.renderPass,
                    gfx::Triangles(),
                    parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly),
                    gfx::StencilMode::disabled(),
                    parameters.colorModeForRenderPass(),
                    gfx::CullFaceMode::disabled(),
                    *buffers.indexBuffer,
                    segment,
                    allUniformValues,
                    allAttributeBindings,
                    textureBindings,
                    this->getID() + "/" + suffix
                );
            },
            [&](const std::reference_wrapper<SegmentVector<SymbolTextAttributes>>& segmentVector) {
                programInstance.draw(
                    parameters.context,
                    *parameters.renderPass,
                    gfx::Triangles(),
                    parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly),
                    gfx::StencilMode::disabled(),
                    parameters.colorModeForRenderPass(),
                    gfx::CullFaceMode::disabled(),
                    *buffers.indexBuffer,
                    segmentVector,
                    allUniformValues,
                    allAttributeBindings,
                    textureBindings,
                    this->getID() + "/" + suffix
                );
            }
        );
    };

    for (const RenderTile& tile : *renderTiles) {
        const LayerRenderData* renderData = getRenderDataForPass(tile, parameters.pass);
        if (!renderData) {
            continue;
        }

        auto& bucket = static_cast<SymbolBucket&>(*renderData->bucket);
        assert(bucket.paintProperties.find(getID()) != bucket.paintProperties.end());
        const auto& bucketPaintProperties = bucket.paintProperties.at(getID());

        auto addRenderables = [&renderableSegments, &tile, renderData, &bucketPaintProperties, it = renderableSegments.begin()] (auto& segments, const SymbolType type) mutable {
            for (auto& segment : segments) {
                it = renderableSegments.emplace_hint(it, SegmentWrapper{std::ref(segment)}, tile, *renderData, bucketPaintProperties, segment.sortKey, type);
            }
        };

        if (bucket.hasIconData()) {
            if (sortFeaturesByKey) {
                addRenderables(bucket.icon.segments, SymbolType::IconRGBA);
            } else {
                drawIcon(draw, tile, *renderData, std::ref(bucket.icon.segments), bucketPaintProperties, parameters, false /*sdfIcon*/);
            }
        }
        
        if (bucket.hasSdfIconData()) {
            if (sortFeaturesByKey) {
                addRenderables(bucket.sdfIcon.segments, SymbolType::IconSDF);
            } else {
                drawIcon(draw, tile, *renderData, std::ref(bucket.sdfIcon.segments), bucketPaintProperties, parameters, true /*sdfIcon*/);
            }
        }

        if (bucket.hasTextData()) {
            if (sortFeaturesByKey) {
                addRenderables(bucket.text.segments, SymbolType::Text);
            } else {
                drawText(draw, tile, *renderData, std::ref(bucket.text.segments), bucketPaintProperties, parameters);
            }
        }

        const auto drawCollisonData = [&](const bool isText, const bool hasCollisionBox, const bool hasCollisionCircle) {
            if (!hasCollisionBox && !hasCollisionCircle) return;

            static const style::Properties<>::PossiblyEvaluated properties{};
            static const CollisionBoxProgram::Binders paintAttributeData(properties, 0);
            auto pixelRatio = tile.id.pixelsToTileUnits(1, parameters.state.getZoom());
            const float scale = std::pow(2, parameters.state.getZoom() - tile.getOverscaledTileID().overscaledZ);
            std::array<float,2> extrudeScale =
                {{
                    parameters.pixelsToGLUnits[0] / (pixelRatio * scale),
                    parameters.pixelsToGLUnits[1] / (pixelRatio * scale)
                }};
            const auto& evaluated = getEvaluated<SymbolLayerProperties>(renderData->layerProperties);
            const auto& layout = *bucket.layout;
            const auto values = isText ? textPropertyValues(evaluated, layout): iconPropertyValues(evaluated, layout);
            const bool needTranslate = values.translate[0] != 0 || values.translate[1] != 0;

            if (hasCollisionBox) {
                const auto& collisionBox = isText ? bucket.textCollisionBox : bucket.iconCollisionBox;
                parameters.programs.getSymbolLayerPrograms().collisionBox.draw(
                    parameters.context,
                    *parameters.renderPass,
                    gfx::Lines{ 1.0f },
                    gfx::DepthMode::disabled(),
                    gfx::StencilMode::disabled(),
                    parameters.colorModeForRenderPass(),
                    gfx::CullFaceMode::disabled(),
                    CollisionBoxProgram::LayoutUniformValues {
                        uniforms::matrix::Value((needTranslate
                                 ? tile.translatedMatrix(values.translate, values.translateAnchor, parameters.state)
                                 : tile.matrix)),
                        uniforms::extrude_scale::Value(extrudeScale),
                        uniforms::camera_to_center_distance::Value(parameters.state.getCameraToCenterDistance())
                    },
                    *collisionBox->vertexBuffer,
                    *collisionBox->dynamicVertexBuffer,
                    *collisionBox->indexBuffer,
                    collisionBox->segments,
                    paintAttributeData,
                    properties,
                    CollisionBoxProgram::TextureBindings{},
                    parameters.state.getZoom(),
                    getID());
            }
            if (hasCollisionCircle) {
                const auto& collisionCircle = isText ? bucket.textCollisionCircle : bucket.iconCollisionCircle;
                parameters.programs.getSymbolLayerPrograms().collisionCircle.draw(
                    parameters.context,
                    *parameters.renderPass,
                    gfx::Triangles(),
                    gfx::DepthMode::disabled(),
                    gfx::StencilMode::disabled(),
                    parameters.colorModeForRenderPass(),
                    gfx::CullFaceMode::disabled(),
                    CollisionCircleProgram::LayoutUniformValues {
                        uniforms::matrix::Value((needTranslate
                                 ? tile.translatedMatrix(values.translate, values.translateAnchor, parameters.state)
                                 : tile.matrix)),
                        uniforms::extrude_scale::Value(extrudeScale),
                        uniforms::overscale_factor::Value(float(tile.getOverscaledTileID().overscaleFactor())),
                        uniforms::camera_to_center_distance::Value(parameters.state.getCameraToCenterDistance())
                    },
                    *collisionCircle->vertexBuffer,
                    *collisionCircle->dynamicVertexBuffer,
                    *collisionCircle->indexBuffer,
                    collisionCircle->segments,
                    paintAttributeData,
                    properties,
                    CollisionCircleProgram::TextureBindings{},
                    parameters.state.getZoom(),
                    getID());
            }
        };
        drawCollisonData(false /*isText*/, bucket.hasIconCollisionBoxData(), bucket.hasIconCollisionCircleData());
        drawCollisonData(true /*isText*/, bucket.hasTextCollisionBoxData(), bucket.hasTextCollisionCircleData());
    }

    if (sortFeaturesByKey) {
        for (auto& renderable : renderableSegments) {
            if (renderable.type == SymbolType::Text) {
                drawText(draw, renderable.tile, renderable.renderData, renderable.segment, renderable.bucketPaintProperties, parameters);
            } else {
                drawIcon(draw, renderable.tile, renderable.renderData, renderable.segment, renderable.bucketPaintProperties, parameters, renderable.type == SymbolType::IconSDF);
            }
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

void RenderSymbolLayer::prepare(const LayerPrepareParameters& params) {
    renderTiles = params.source->getRenderTilesSortedByYPosition();
    addRenderPassesFromTiles();

    placementData.clear();
    for (const RenderTile& renderTile : *renderTiles) {
        auto* bucket = static_cast<SymbolBucket*>(renderTile.getBucket(*baseImpl));
        if (bucket && bucket->bucketLeaderID == getID()) {
            // Only place this layer if it's the "group leader" for the bucket
            const Tile* tile = params.source->getRenderedTile(renderTile.id);
            assert(tile);
            assert(tile->kind == Tile::Kind::Geometry);
            placementData.push_back({*bucket, renderTile, static_cast<const GeometryTile*>(tile)->getFeatureIndex()});
        }
    }
}

} // namespace mbgl
