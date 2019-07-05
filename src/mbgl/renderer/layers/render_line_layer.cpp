#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/buckets/line_bucket.hpp>
#include <mbgl/renderer/color_ramp_render_data.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/upload_parameters.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/line_program.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>
#include <mbgl/tile/geometry_tile.hpp>

namespace mbgl {

using namespace style;

inline const LineLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    return static_cast<const LineLayer::Impl&>(*impl);
}

RenderLineLayer::RenderLineLayer(Immutable<style::LineLayer::Impl> _impl)
    : RenderLayer(makeMutable<LineLayerProperties>(std::move(_impl))),
      unevaluated(impl(baseImpl).paint.untransitioned()) {
}

RenderLineLayer::~RenderLineLayer() = default;

LayerRenderer RenderLineLayer::createRenderer() {
    const unsigned char kDrawPattern = 1 << 0;
    const unsigned char kDrawGradient = 1 << 1;
    // Minimize size of the catured parameters in order to
    // avoid heap allocations at std::function construction.
    unsigned char drawFlags = 0;

    if (!unevaluated.get<LinePattern>().isUndefined()) {
        drawFlags |= kDrawPattern;
    } else if (!unevaluated.get<LineGradient>().getValue().isUndefined()) {
        drawFlags |= kDrawGradient;
    }

    return [drawFlags, colorRampData = colorRamp](PaintParameters& parameters, const LayerRenderItem& renderItem) {
        if (parameters.pass == RenderPass::Opaque) {
            return;
        }
        const auto& renderTiles = renderItem.renderTiles;
        const auto layerId = renderItem.evaluatedProperties->baseImpl->id;

        parameters.renderTileClippingMasks(renderTiles);

        for (const RenderTile& tile : renderTiles) {
            const LayerRenderData* renderData = tile.getLayerRenderData(*renderItem.evaluatedProperties->baseImpl);
            if (!renderData) {
                continue;
            }
            auto& bucket = static_cast<LineBucket&>(*renderData->bucket);
            const auto& evaluated = getEvaluated<LineLayerProperties>(renderData->layerProperties);
            const auto& crossfade = getCrossfade<LineLayerProperties>(renderData->layerProperties);

            const bool drawDash = !evaluated.get<LineDasharray>().from.empty();

            auto draw = [&](auto& programInstance,
                            auto&& uniformValues,
                            const optional<ImagePosition>& patternPositionA,
                            const optional<ImagePosition>& patternPositionB, auto&& textureBindings) {
                const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(layerId);

                paintPropertyBinders.setPatternParameters(patternPositionA, patternPositionB, crossfade);

                const auto allUniformValues = programInstance.computeAllUniformValues(
                    std::move(uniformValues),
                    paintPropertyBinders,
                    evaluated,
                    parameters.state.getZoom()
                );
                const auto allAttributeBindings = programInstance.computeAllAttributeBindings(
                    *bucket.vertexBuffer,
                    paintPropertyBinders,
                    evaluated
                );

                renderItem.checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

                programInstance.draw(
                    parameters.context,
                    *parameters.renderPass,
                    gfx::Triangles(),
                    parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly),
                    parameters.stencilModeForClipping(tile.id),
                    parameters.colorModeForRenderPass(),
                    gfx::CullFaceMode::disabled(),
                    *bucket.indexBuffer,
                    bucket.segments,
                    allUniformValues,
                    allAttributeBindings,
                    std::move(textureBindings),
                    layerId
                );
            };

            if (drawDash) {
                const LinePatternCap cap = bucket.layout.get<LineCap>() == LineCapType::Round
                    ? LinePatternCap::Round : LinePatternCap::Square;
                LinePatternPos posA = parameters.lineAtlas.getDashPosition(evaluated.get<LineDasharray>().from, cap);
                LinePatternPos posB = parameters.lineAtlas.getDashPosition(evaluated.get<LineDasharray>().to, cap);

                draw(
                    parameters.programs.getLineLayerPrograms().lineSDF,
                    LineSDFProgram::layoutUniformValues(
                        evaluated,
                        parameters.pixelRatio,
                        tile,
                        parameters.state,
                        parameters.pixelsToGLUnits,
                        posA,
                        posB,
                        crossfade,
                        parameters.lineAtlas.getSize().width),
                        {},
                        {},
                        LineSDFProgram::TextureBindings{
                            parameters.lineAtlas.textureBinding(),
                        });

            } else if (drawFlags & kDrawPattern) {
                const auto& linePatternValue = evaluated.get<LinePattern>().constantOr(Faded<std::basic_string<char>>{ "", ""});
                const Size& texsize = tile.getIconAtlasTexture().size;

                optional<ImagePosition> posA = tile.getPattern(linePatternValue.from);
                optional<ImagePosition> posB = tile.getPattern(linePatternValue.to);

                draw(parameters.programs.getLineLayerPrograms().linePattern,
                    LinePatternProgram::layoutUniformValues(
                        evaluated,
                        tile,
                        parameters.state,
                        parameters.pixelsToGLUnits,
                        parameters.pixelRatio,
                        texsize,
                        crossfade),
                        posA,
                        posB,
                        LinePatternProgram::TextureBindings{
                            textures::image::Value{ tile.getIconAtlasTexture().getResource(), gfx::TextureFilterType::Linear },
                        });
            } else if (drawFlags & kDrawGradient) {
                assert(colorRampData);
                assert(colorRampData->texture);
                draw(parameters.programs.getLineLayerPrograms().lineGradient,
                    LineGradientProgram::layoutUniformValues(
                        evaluated,
                        tile,
                        parameters.state,
                        parameters.pixelsToGLUnits,
                        parameters.pixelRatio),
                        {},
                        {},
                        LineGradientProgram::TextureBindings{
                            textures::image::Value{ colorRampData->texture->getResource(), gfx::TextureFilterType::Linear },
                        });
            } else {
                draw(parameters.programs.getLineLayerPrograms().line,
                    LineProgram::layoutUniformValues(
                        evaluated,
                        tile,
                        parameters.state,
                        parameters.pixelsToGLUnits,
                        parameters.pixelRatio),
                    {},
                    {},
                    LineProgram::TextureBindings{});
            }
        }
    };
}

LayerUploader RenderLineLayer::createUploader() {
    if (!unevaluated.get<LineGradient>().getValue().isUndefined()) {
        return [colorRampData = colorRamp] (gfx::UploadPass& uploadPass) {
            if (colorRampData) colorRampData->upload(uploadPass);
        };
    }
    return {};
}

void RenderLineLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
    updateColorRamp();
}

void RenderLineLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    auto properties = makeMutable<LineLayerProperties>(
        staticImmutableCast<LineLayer::Impl>(baseImpl),
        parameters.getCrossfadeParameters(),
        unevaluated.evaluate(parameters));
    auto& evaluated = properties->evaluated;

    passes = (evaluated.get<style::LineOpacity>().constantOr(1.0) > 0
              && evaluated.get<style::LineColor>().constantOr(Color::black()).a > 0
              && evaluated.get<style::LineWidth>().constantOr(1.0) > 0)
             ? RenderPass::Translucent : RenderPass::None;
    evaluatedProperties = std::move(properties);
}

bool RenderLineLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderLineLayer::hasCrossfade() const {
    return getCrossfade<LineLayerProperties>(evaluatedProperties).t != 1;
}

void RenderLineLayer::prepare(const LayerPrepareParameters& params) {
    RenderLayer::prepare(params);
    for (const RenderTile& tile : renderTiles) {
        const LayerRenderData* renderData = tile.getLayerRenderData(*baseImpl);
        if (!renderData) continue;

        const auto& evaluated = getEvaluated<LineLayerProperties>(renderData->layerProperties);
        if (evaluated.get<LineDasharray>().from.empty()) continue;

        auto& bucket = static_cast<LineBucket&>(*renderData->bucket);
        const LinePatternCap cap = bucket.layout.get<LineCap>() == LineCapType::Round
            ? LinePatternCap::Round : LinePatternCap::Square;
        // Ensures that the dash data gets added to the atlas.
        params.lineAtlas.getDashPosition(evaluated.get<LineDasharray>().from, cap);
        params.lineAtlas.getDashPosition(evaluated.get<LineDasharray>().to, cap);
    }
}

optional<GeometryCollection> offsetLine(const GeometryCollection& rings, const double offset) {
    if (offset == 0) return {};

    GeometryCollection newRings;
    Point<double> zero(0, 0);
    for (const auto& ring : rings) {
        newRings.emplace_back();
        auto& newRing = newRings.back();

        for (auto i = ring.begin(); i != ring.end(); i++) {
            auto& p = *i;

            Point<double> aToB = i == ring.begin() ?
                                 zero :
                                 util::perp(util::unit(convertPoint<double>(p - *(i - 1))));
            Point<double> bToC = i + 1 == ring.end() ?
                                 zero :
                                 util::perp(util::unit(convertPoint<double>(*(i + 1) - p)));
            Point<double> extrude = util::unit(aToB + bToC);

            const double cosHalfAngle = extrude.x * bToC.x + extrude.y * bToC.y;
            extrude *= (1.0 / cosHalfAngle);

            newRing.push_back(convertPoint<int16_t>(extrude * offset) + p);
        }
    }

    return newRings;
}

bool RenderLineLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float zoom,
        const TransformState& transformState,
        const float pixelsToTileUnits,
        const mat4&) const {
    const auto& evaluated = static_cast<const LineLayerProperties&>(*evaluatedProperties).evaluated;
    // Translate query geometry
    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::LineTranslate>(),
            evaluated.get<style::LineTranslateAnchor>(),
            transformState.getBearing(),
            pixelsToTileUnits);

    // Evaluate function
    auto offset = evaluated.get<style::LineOffset>()
                          .evaluate(feature, zoom, style::LineOffset::defaultValue()) * pixelsToTileUnits;

    // Apply offset to geometry
    auto offsetGeometry = offsetLine(feature.getGeometries(), offset);

    // Test intersection
    const float halfWidth = getLineWidth(feature, zoom) / 2.0 * pixelsToTileUnits;
    return util::polygonIntersectsBufferedMultiLine(
            translatedQueryGeometry.value_or(queryGeometry),
            offsetGeometry.value_or(feature.getGeometries()),
            halfWidth);
}

void RenderLineLayer::updateColorRamp() {
    auto colorValue = unevaluated.get<LineGradient>().getValue();
    if (colorValue.isUndefined()) {
        return;
    }
    colorRamp = std::make_shared<ColorRampRenderData>(colorValue);
}

float RenderLineLayer::getLineWidth(const GeometryTileFeature& feature, const float zoom) const {
    const auto& evaluated = static_cast<const LineLayerProperties&>(*evaluatedProperties).evaluated;
    float lineWidth = evaluated.get<style::LineWidth>()
            .evaluate(feature, zoom, style::LineWidth::defaultValue());
    float gapWidth = evaluated.get<style::LineGapWidth>()
            .evaluate(feature, zoom, style::LineGapWidth::defaultValue());
    if (gapWidth) {
        return gapWidth + 2 * lineWidth;
    } else {
        return lineWidth;
    }
}

} // namespace mbgl
