#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/buckets/line_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/line_program.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>
#include <mbgl/tile/geometry_tile.hpp>

namespace mbgl {

using namespace style;

RenderLineLayer::RenderLineLayer(Immutable<style::LineLayer::Impl> _impl)
    : RenderLayer(std::move(_impl)),
      unevaluated(impl().paint.untransitioned()),
      colorRamp({256, 1}) {
}

const style::LineLayer::Impl& RenderLineLayer::impl() const {
    return static_cast<const style::LineLayer::Impl&>(*baseImpl);
}

void RenderLineLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderLineLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    style::Properties<LineFloorwidth>::Unevaluated extra(unevaluated.get<style::LineWidth>());
    evaluated = RenderLinePaintProperties::PossiblyEvaluated(
        unevaluated.evaluate(parameters).concat(extra.evaluate(parameters)));

    crossfade = parameters.getCrossfadeParameters();

    passes = (evaluated.get<style::LineOpacity>().constantOr(1.0) > 0
              && evaluated.get<style::LineColor>().constantOr(Color::black()).a > 0
              && evaluated.get<style::LineWidth>().constantOr(1.0) > 0)
             ? RenderPass::Translucent : RenderPass::None;
}

bool RenderLineLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderLineLayer::hasCrossfade() const {
    return crossfade.t != 1;
}

void RenderLineLayer::render(PaintParameters& parameters, RenderSource*) {
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    for (const RenderTile& tile : renderTiles) {
        auto bucket_ = tile.tile.getBucket<LineBucket>(*baseImpl);
        if (!bucket_) {
            continue;
        }
        LineBucket& bucket = *bucket_;

        auto draw = [&](auto& program, auto&& uniformValues,
                        const optional<ImagePosition>& patternPositionA,
                        const optional<ImagePosition>& patternPositionB, auto&& textureBindings) {
            auto& programInstance = program.get(evaluated);

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
                gfx::Triangles(),
                parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly),
                parameters.stencilModeForClipping(tile.clip),
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
            const LinePatternCap cap = bucket.layout.get<LineCap>() == LineCapType::Round
                ? LinePatternCap::Round : LinePatternCap::Square;
            LinePatternPos posA = parameters.lineAtlas.getDashPosition(evaluated.get<LineDasharray>().from, cap);
            LinePatternPos posB = parameters.lineAtlas.getDashPosition(evaluated.get<LineDasharray>().to, cap);

            draw(parameters.programs.getLineLayerPrograms().lineSDF,
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
                         parameters.lineAtlas.textureBinding(parameters.context),
                     });

        } else if (!unevaluated.get<LinePattern>().isUndefined()) {
            const auto linePatternValue =  evaluated.get<LinePattern>().constantOr(Faded<std::basic_string<char>>{ "", ""});
            GeometryTile& geometryTile = static_cast<GeometryTile&>(tile.tile);            
            const Size texsize = geometryTile.iconAtlasTexture->size;

            optional<ImagePosition> posA = geometryTile.getPattern(linePatternValue.from);
            optional<ImagePosition> posB = geometryTile.getPattern(linePatternValue.to);

            draw(parameters.programs.getLineLayerPrograms().linePattern,
                 LinePatternProgram::layoutUniformValues(
                     evaluated,
                     tile,
                     parameters.state,
                     parameters.pixelsToGLUnits,
                     texsize,
                     crossfade,
                     parameters.pixelRatio),
                     posA,
                     posB,
                     LinePatternProgram::TextureBindings{
                         textures::u_image::Value{ *geometryTile.iconAtlasTexture->resource, gfx::TextureFilterType::Linear },
                     });
        } else if (!unevaluated.get<LineGradient>().getValue().isUndefined()) {
            if (!colorRampTexture) {
                colorRampTexture = parameters.context.createTexture(colorRamp);
            }

            draw(parameters.programs.getLineLayerPrograms().lineGradient,
                 LineGradientProgram::layoutUniformValues(
                    evaluated,
                    tile,
                    parameters.state,
                    parameters.pixelsToGLUnits),
                    {},
                    {},
                    LineGradientProgram::TextureBindings{
                        textures::u_image::Value{ *colorRampTexture->resource, gfx::TextureFilterType::Linear },
                    });
        } else {
            draw(parameters.programs.getLineLayerPrograms().line,
                 LineProgram::layoutUniformValues(
                     evaluated,
                     tile,
                     parameters.state,
                     parameters.pixelsToGLUnits),
                 {},
                 {},
                 LineProgram::TextureBindings{});
        }
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

float RenderLineLayer::getLineWidth(const GeometryTileFeature& feature, const float zoom) const {
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

void RenderLineLayer::update() {
    updateColorRamp();
}

} // namespace mbgl
