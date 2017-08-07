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
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

using namespace style;

RenderLineLayer::RenderLineLayer(Immutable<style::LineLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Line, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::LineLayer::Impl& RenderLineLayer::impl() const {
    return static_cast<const style::LineLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderLineLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<LineBucket>(parameters, layers, impl().layout);
}

void RenderLineLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderLineLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    style::Properties<LineFloorwidth>::Unevaluated extra(unevaluated.get<style::LineWidth>());

    auto dashArrayParams = parameters;
    dashArrayParams.useIntegerZoom = true;

    evaluated = RenderLinePaintProperties::PossiblyEvaluated(
        unevaluated.evaluate(parameters).concat(extra.evaluate(dashArrayParams)));

    passes = (evaluated.get<style::LineOpacity>().constantOr(1.0) > 0
              && evaluated.get<style::LineColor>().constantOr(Color::black()).a > 0
              && evaluated.get<style::LineWidth>().constantOr(1.0) > 0)
             ? RenderPass::Translucent : RenderPass::None;
}

bool RenderLineLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderLineLayer::render(PaintParameters& parameters, RenderSource*) {
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    for (const RenderTile& tile : renderTiles) {
        assert(dynamic_cast<LineBucket*>(tile.tile.getBucket(*baseImpl)));
        LineBucket& bucket = *reinterpret_cast<LineBucket*>(tile.tile.getBucket(*baseImpl));

        auto draw = [&] (auto& program, auto&& uniformValues) {
            program.get(evaluated).draw(
                parameters.context,
                gl::Triangles(),
                parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                parameters.stencilModeForClipping(tile.clip),
                parameters.colorModeForRenderPass(),
                std::move(uniformValues),
                *bucket.vertexBuffer,
                *bucket.indexBuffer,
                bucket.segments,
                bucket.paintPropertyBinders.at(getID()),
                evaluated,
                parameters.state.getZoom(),
                getID()
            );
        };

        if (!evaluated.get<LineDasharray>().from.empty()) {
            const LinePatternCap cap = bucket.layout.get<LineCap>() == LineCapType::Round
                ? LinePatternCap::Round : LinePatternCap::Square;
            LinePatternPos posA = parameters.lineAtlas.getDashPosition(evaluated.get<LineDasharray>().from, cap);
            LinePatternPos posB = parameters.lineAtlas.getDashPosition(evaluated.get<LineDasharray>().to, cap);

            parameters.lineAtlas.bind(parameters.context, 0);

            draw(parameters.programs.lineSDF,
                 LineSDFProgram::uniformValues(
                     evaluated,
                     parameters.pixelRatio,
                     tile,
                     parameters.state,
                     parameters.pixelsToGLUnits,
                     posA,
                     posB,
                     parameters.lineAtlas.getSize().width));

        } else if (!evaluated.get<LinePattern>().from.empty()) {
            optional<ImagePosition> posA = parameters.imageManager.getPattern(evaluated.get<LinePattern>().from);
            optional<ImagePosition> posB = parameters.imageManager.getPattern(evaluated.get<LinePattern>().to);

            if (!posA || !posB)
                return;

            parameters.imageManager.bind(parameters.context, 0);

            draw(parameters.programs.linePattern,
                 LinePatternProgram::uniformValues(
                     evaluated,
                     tile,
                     parameters.state,
                     parameters.pixelsToGLUnits,
                     parameters.imageManager.getPixelSize(),
                     *posA,
                     *posB));

        } else {
            draw(parameters.programs.line,
                 LineProgram::uniformValues(
                     evaluated,
                     tile,
                     parameters.state,
                     parameters.pixelsToGLUnits));
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
        const float bearing,
        const float pixelsToTileUnits) const {

    // Translate query geometry
    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::LineTranslate>(),
            evaluated.get<style::LineTranslateAnchor>(),
            bearing,
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


} // namespace mbgl
