#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/programs/line_program.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/renderer/buckets/line_bucket.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/upload_parameters.hpp>
#include <mbgl/style/expression/image.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/intersection_tests.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

using namespace style;

namespace {

inline const LineLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == LineLayer::Impl::staticTypeInfo());
    return static_cast<const LineLayer::Impl&>(*impl);
}

} // namespace

RenderLineLayer::RenderLineLayer(Immutable<style::LineLayer::Impl> _impl)
    : RenderLayer(makeMutable<LineLayerProperties>(std::move(_impl))),
      unevaluated(impl(baseImpl).paint.untransitioned()),
      colorRamp({256, 1}) {
}

RenderLineLayer::~RenderLineLayer() = default;

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
    properties->renderPasses = mbgl::underlying_type(passes);
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
    for (const RenderTile& tile : *renderTiles) {
        const LayerRenderData* renderData = tile.getLayerRenderData(*baseImpl);
        if (!renderData) continue;

        const auto& evaluated = getEvaluated<LineLayerProperties>(renderData->layerProperties);
        if (evaluated.get<LineDasharray>().from.empty()) continue;

        auto& bucket = static_cast<LineBucket&>(*renderData->bucket);
        const LinePatternCap cap = bucket.layout.get<LineCap>() == LineCapType::Round
            ? LinePatternCap::Round : LinePatternCap::Square;
        // Ensures that the dash data gets added to the atlas.
        params.lineAtlas.getDashPatternTexture(
            evaluated.get<LineDasharray>().from, evaluated.get<LineDasharray>().to, cap);
    }
}

void RenderLineLayer::upload(gfx::UploadPass& uploadPass) {
    if (!unevaluated.get<LineGradient>().getValue().isUndefined() && !colorRampTexture) {
        colorRampTexture = uploadPass.createTexture(colorRamp);
    }
}

void RenderLineLayer::render(PaintParameters& parameters) {
    assert(renderTiles);
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    parameters.renderTileClippingMasks(renderTiles);

    for (const RenderTile& tile : *renderTiles) {
        const LayerRenderData* renderData = getRenderDataForPass(tile, parameters.pass);
        if (!renderData) {
            continue;
        }
        auto& bucket = static_cast<LineBucket&>(*renderData->bucket);
        const auto& evaluated = getEvaluated<LineLayerProperties>(renderData->layerProperties);
        const auto& crossfade = getCrossfade<LineLayerProperties>(renderData->layerProperties);

        auto draw = [&](auto& programInstance,
                        auto&& uniformValues,
                        const optional<ImagePosition>& patternPositionA,
                        const optional<ImagePosition>& patternPositionB, auto&& textureBindings) {
            const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());

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

            checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

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
                getID()
            );
        };

        if (!evaluated.get<LineDasharray>().from.empty()) {
            const LinePatternCap cap =
                bucket.layout.get<LineCap>() == LineCapType::Round ? LinePatternCap::Round : LinePatternCap::Square;
            const auto& dashPatternTexture = parameters.lineAtlas.getDashPatternTexture(
                evaluated.get<LineDasharray>().from, evaluated.get<LineDasharray>().to, cap);

            draw(parameters.programs.getLineLayerPrograms().lineSDF,
                 LineSDFProgram::layoutUniformValues(evaluated,
                                                     parameters.pixelRatio,
                                                     tile,
                                                     parameters.state,
                                                     parameters.pixelsToGLUnits,
                                                     dashPatternTexture.getFrom(),
                                                     dashPatternTexture.getTo(),
                                                     crossfade,
                                                     dashPatternTexture.getSize().width),
                 {},
                 {},
                 LineSDFProgram::TextureBindings{
                     dashPatternTexture.textureBinding(),
                 });

        } else if (!unevaluated.get<LinePattern>().isUndefined()) {
            const auto& linePatternValue = evaluated.get<LinePattern>().constantOr(Faded<expression::Image>{"", ""});
            const Size& texsize = tile.getIconAtlasTexture().size;

            optional<ImagePosition> posA = tile.getPattern(linePatternValue.from.id());
            optional<ImagePosition> posB = tile.getPattern(linePatternValue.to.id());

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
        } else if (!unevaluated.get<LineGradient>().getValue().isUndefined()) {
            assert(colorRampTexture);

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
                        textures::image::Value{ colorRampTexture->getResource(), gfx::TextureFilterType::Linear },
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
}

namespace {

GeometryCollection offsetLine(const GeometryCollection& rings, double offset) {
    assert(offset != 0.0f);
    assert(!rings.empty());

    GeometryCollection newRings;
    newRings.reserve(rings.size());

    const Point<double> zero(0, 0);
    for (const auto& ring : rings) {
        newRings.emplace_back();
        auto& newRing = newRings.back();
        newRing.reserve(ring.size());

        for (auto i = ring.begin(); i != ring.end(); ++i) {
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

            newRing.emplace_back(convertPoint<int16_t>(extrude * offset) + p);
        }
    }

    return newRings;
}

} // namespace

bool RenderLineLayer::queryIntersectsFeature(const GeometryCoordinates& queryGeometry,
                                             const GeometryTileFeature& feature, const float zoom,
                                             const TransformState& transformState, const float pixelsToTileUnits,
                                             const mat4&, const FeatureState& featureState) const {
    const auto& evaluated = static_cast<const LineLayerProperties&>(*evaluatedProperties).evaluated;
    // Translate query geometry
    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::LineTranslate>(),
            evaluated.get<style::LineTranslateAnchor>(),
            transformState.getBearing(),
            pixelsToTileUnits);

    // Evaluate function
    auto offset =
        evaluated.get<style::LineOffset>().evaluate(feature, zoom, featureState, style::LineOffset::defaultValue()) *
        pixelsToTileUnits;
    // Test intersection
    const auto halfWidth = static_cast<float>(getLineWidth(feature, zoom, featureState) / 2.0 * pixelsToTileUnits);

    // Apply offset to geometry
    if (offset != 0.0f && !feature.getGeometries().empty()) {
        return util::polygonIntersectsBufferedMultiLine(
                translatedQueryGeometry.value_or(queryGeometry),
                offsetLine(feature.getGeometries(), offset),
                halfWidth);
    }

    return util::polygonIntersectsBufferedMultiLine(
        translatedQueryGeometry.value_or(queryGeometry),
        feature.getGeometries(),
        halfWidth);
}

void RenderLineLayer::updateColorRamp() {
    auto colorValue = unevaluated.get<LineGradient>().getValue();
    if (colorValue.isUndefined()) {
        return;
    }

    const auto length = colorRamp.bytes();

    for (uint32_t i = 0; i < length; i += 4) {
        const auto color = colorValue.evaluate(static_cast<double>(i) / length);
        colorRamp.data[i] = std::floor(color.r * 255);
        colorRamp.data[i + 1] = std::floor(color.g * 255);
        colorRamp.data[i + 2] = std::floor(color.b * 255);
        colorRamp.data[i + 3] = std::floor(color.a * 255);
    }

    if (colorRampTexture) {
        colorRampTexture = nullopt;
    }
}

float RenderLineLayer::getLineWidth(const GeometryTileFeature& feature, const float zoom,
                                    const FeatureState& featureState) const {
    const auto& evaluated = static_cast<const LineLayerProperties&>(*evaluatedProperties).evaluated;
    float lineWidth =
        evaluated.get<style::LineWidth>().evaluate(feature, zoom, featureState, style::LineWidth::defaultValue());
    float gapWidth =
        evaluated.get<style::LineGapWidth>().evaluate(feature, zoom, featureState, style::LineGapWidth::defaultValue());
    if (gapWidth) {
        return gapWidth + 2 * lineWidth;
    } else {
        return lineWidth;
    }
}

} // namespace mbgl
