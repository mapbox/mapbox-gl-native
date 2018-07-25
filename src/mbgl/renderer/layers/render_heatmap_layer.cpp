#include <mbgl/renderer/layers/render_heatmap_layer.hpp>
#include <mbgl/renderer/buckets/heatmap_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/heatmap_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

using namespace style;

RenderHeatmapLayer::RenderHeatmapLayer(Immutable<style::HeatmapLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Heatmap, _impl),
    unevaluated(impl().paint.untransitioned()), colorRamp({256, 1}) {
}

const style::HeatmapLayer::Impl& RenderHeatmapLayer::impl() const {
    return static_cast<const style::HeatmapLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderHeatmapLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<HeatmapBucket>(parameters, layers);
}

void RenderHeatmapLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderHeatmapLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = (evaluated.get<style::HeatmapOpacity>() > 0)
            ? (RenderPass::Translucent | RenderPass::Pass3D)
            : RenderPass::None;
}

bool RenderHeatmapLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderHeatmapLayer::render(PaintParameters& parameters, RenderSource*) {
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    if (parameters.pass == RenderPass::Pass3D) {
        const auto& viewportSize = parameters.staticData.backendSize;
        const auto size = Size{viewportSize.width / 4, viewportSize.height / 4};

        if (!renderTexture || renderTexture->getSize() != size) {
            if (parameters.context.supportsHalfFloatTextures) {
                renderTexture = OffscreenTexture(parameters.context, size, gl::TextureType::HalfFloat);

                try {
                    renderTexture->bind();
                } catch (const std::runtime_error& ex) {
                    // can't render to a half-float texture; falling back to unsigned byte one
                    renderTexture = nullopt;
                    parameters.context.supportsHalfFloatTextures = false;
                }
            }

            if (!parameters.context.supportsHalfFloatTextures || !renderTexture) {
                renderTexture = OffscreenTexture(parameters.context, size, gl::TextureType::UnsignedByte);
                renderTexture->bind();
            }

        } else {
            renderTexture->bind();
        }

        if (!colorRampTexture) {
            colorRampTexture = parameters.context.createTexture(colorRamp, 1, gl::TextureType::UnsignedByte);
        }

        parameters.context.clear(Color{ 0.0f, 0.0f, 0.0f, 1.0f }, {}, {});

        for (const RenderTile& tile : renderTiles) {
            auto bucket_ = tile.tile.getBucket<HeatmapBucket>(*baseImpl);
            if (!bucket_) {
                continue;
            }
            HeatmapBucket& bucket = *bucket_;

            const auto extrudeScale = tile.id.pixelsToTileUnits(1, parameters.state.getZoom());

            const auto stencilMode = parameters.mapMode != MapMode::Continuous
                ? parameters.stencilModeForClipping(tile.clip)
                : gl::StencilMode::disabled();

            const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());

            auto& programInstance = parameters.programs.heatmap.get(evaluated);
       
            const auto allUniformValues = programInstance.computeAllUniformValues(
                HeatmapProgram::UniformValues {
                    uniforms::u_intensity::Value{ evaluated.get<style::HeatmapIntensity>() },
                    uniforms::u_matrix::Value{ tile.matrix },
                    uniforms::heatmap::u_extrude_scale::Value{ extrudeScale }
                },
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
                gl::Triangles(),
                parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                stencilMode,
                gl::ColorMode::additive(),
                *bucket.indexBuffer,
                bucket.segments,
                allUniformValues,
                allAttributeBindings,
                getID()
            );
        }

    } else if (parameters.pass == RenderPass::Translucent) {
        parameters.context.bindTexture(renderTexture->getTexture(), 0, gl::TextureFilter::Linear);
        parameters.context.bindTexture(*colorRampTexture, 1, gl::TextureFilter::Linear);

        const auto& size = parameters.staticData.backendSize;

        mat4 viewportMat;
        matrix::ortho(viewportMat, 0, size.width, size.height, 0, 0, 1);

        const Properties<>::PossiblyEvaluated properties;
        const HeatmapTextureProgram::PaintPropertyBinders paintAttributeData{ properties, 0 };

        auto& programInstance = parameters.programs.heatmapTexture;

        const auto allUniformValues = programInstance.computeAllUniformValues(
            HeatmapTextureProgram::UniformValues{
                uniforms::u_matrix::Value{ viewportMat }, uniforms::u_world::Value{ size },
                uniforms::u_image::Value{ 0 },
                uniforms::u_color_ramp::Value{ 1 },
                uniforms::u_opacity::Value{ evaluated.get<HeatmapOpacity>() }
            },
            paintAttributeData,
            properties,
            parameters.state.getZoom()
        );
        const auto allAttributeBindings = programInstance.computeAllAttributeBindings(
            parameters.staticData.extrusionTextureVertexBuffer,
            paintAttributeData,
            properties
        );

        checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

        programInstance.draw(
            parameters.context,
            gl::Triangles(),
            gl::DepthMode::disabled(),
            gl::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            parameters.staticData.quadTriangleIndexBuffer,
            parameters.staticData.extrusionTextureSegments,
            allUniformValues,
            allAttributeBindings,
            getID()
        );
    }
}

void RenderHeatmapLayer::updateColorRamp() {
    auto colorValue = unevaluated.get<HeatmapColor>().getValue();
    if (colorValue.isUndefined()) {
        colorValue = HeatmapLayer::getDefaultHeatmapColor();
    }

    const auto length = colorRamp.bytes();

    for (uint32_t i = 0; i < length; i += 4) {
        const auto color = colorValue.evaluate(static_cast<double>(i) / length);
        colorRamp.data[i + 0] = std::floor(color.r * 255);
        colorRamp.data[i + 1] = std::floor(color.g * 255);
        colorRamp.data[i + 2] = std::floor(color.b * 255);
        colorRamp.data[i + 3] = std::floor(color.a * 255);
    }

    if (colorRampTexture) {
        colorRampTexture = nullopt;
    }
}

bool RenderHeatmapLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float zoom,
        const TransformState&,
        const float pixelsToTileUnits,
        const mat4&) const {
    (void) queryGeometry;
    (void) feature;
    (void) zoom;
    (void) pixelsToTileUnits;
    return false;
}

} // namespace mbgl
