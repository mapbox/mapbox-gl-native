#include <mbgl/renderer/layers/render_hillshade_layer.hpp>
#include <mbgl/renderer/buckets/hillshade_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/hillshade_program.hpp>
#include <mbgl/programs/hillshade_prepare_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/hillshade_layer_impl.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/offscreen_texture.hpp>

namespace mbgl {

using namespace style;
RenderHillshadeLayer::RenderHillshadeLayer(Immutable<style::HillshadeLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Hillshade, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::HillshadeLayer::Impl& RenderHillshadeLayer::impl() const {
    return static_cast<const style::HillshadeLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderHillshadeLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false);
    return nullptr;
}

const std::array<float, 2> RenderHillshadeLayer::getLatRange(const UnwrappedTileID& id) {
   const LatLng latlng0 = LatLng(id);
   const LatLng latlng1 = LatLng(UnwrappedTileID(id.canonical.z, id.canonical.x, id.canonical.y + 1));
   return {{ (float)latlng0.latitude(), (float)latlng1.latitude() }};
}

const std::array<float, 3> RenderHillshadeLayer::getLight(const PaintParameters& parameters){
    float azimuthal = evaluated.get<HillshadeIlluminationDirection>() * util::DEG2RAD;
    const float zenith = 30.0f * util::DEG2RAD;
    if (evaluated.get<HillshadeIlluminationAnchor>() == Viewport) azimuthal = azimuthal - parameters.state.getAngle();
    return {{evaluated.get<HillshadeExaggeration>(), azimuthal, zenith}};
}

void RenderHillshadeLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderHillshadeLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);
    passes = RenderPass::Translucent;
}

bool RenderHillshadeLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderHillshadeLayer::render(PaintParameters& parameters, RenderSource*) {
    if (parameters.pass != RenderPass::Translucent)
        return;
    
    auto draw = [&] (const mat4& matrix,
                     const auto& vertexBuffer,
                     const auto& indexBuffer,
                     const auto& segments,
                     const UnwrappedTileID& id) {
        parameters.programs.hillshade.draw(
            parameters.context,
            gl::Triangles(),
            parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
            gl::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            HillshadeProgram::UniformValues {
                uniforms::u_matrix::Value{ matrix },
                uniforms::u_image::Value{ 0 },
                uniforms::u_highlight::Value{ evaluated.get<HillshadeHighlightColor>() },
                uniforms::u_shadow::Value{ evaluated.get<HillshadeShadowColor>() },
                uniforms::u_accent::Value{ evaluated.get<HillshadeAccentColor>() },
                uniforms::u_light::Value{ getLight(parameters) },
                uniforms::u_latrange::Value{ getLatRange(id) },
            },
            vertexBuffer,
            indexBuffer,
            segments,
            HillshadeProgram::PaintPropertyBinders { evaluated, 0 },
            evaluated,
            parameters.state.getZoom(),
            getID()
        );
    };


    for (const RenderTile& tile : renderTiles) {
        assert(dynamic_cast<HillshadeBucket*>(tile.tile.getBucket(*baseImpl)));
        HillshadeBucket& bucket = *reinterpret_cast<HillshadeBucket*>(tile.tile.getBucket(*baseImpl));
        

        if (!bucket.hasData())
            continue;
        if (!bucket.prepared) {
            OffscreenTexture view(parameters.context, { 256, 256 });
            view.bind();
            
            parameters.context.bindTexture(*bucket.dem, 0, gl::TextureFilter::Linear, gl::TextureMipMap::Yes);
            const Properties<>::PossiblyEvaluated properties;

            mat4 mat;
            matrix::ortho(mat, 0, util::EXTENT, -util::EXTENT, 0, 0, 1);
            matrix::translate(mat, mat, 0, -util::EXTENT, 0);
            
            parameters.programs.hillshadePrepare.draw(
                parameters.context,
                gl::Triangles(),
                parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                gl::StencilMode::disabled(),
                parameters.colorModeForRenderPass(),
                HillshadePrepareProgram::UniformValues {
                    uniforms::u_matrix::Value { mat },
                    uniforms::u_dimension::Value { {{512, 512 }} },
                    uniforms::u_zoom::Value{ float(tile.id.canonical.z) },
                    uniforms::u_image::Value{ 0 }
                },
                parameters.staticData.rasterVertexBuffer,
                parameters.staticData.quadTriangleIndexBuffer,
                parameters.staticData.rasterSegments,
                HillshadePrepareProgram::PaintPropertyBinders { properties, 0 },
                properties,
                parameters.state.getZoom(),
                getID()
            );
            bucket.texture = std::move(view.getTexture());
            bucket.prepared = true;
        }
        
        assert(bucket.texture);
        parameters.context.bindTexture(*bucket.texture, 0, gl::TextureFilter::Linear);

        if (bucket.vertexBuffer && bucket.indexBuffer && !bucket.segments.empty()) {
            // Draw only the parts of the tile that aren't drawn by another tile in the layer.
            draw(tile.matrix,
                 *bucket.vertexBuffer,
                 *bucket.indexBuffer,
                 bucket.segments,
                 tile.id);
        } else {
            // Draw the full tile.
            draw(tile.matrix,
                 parameters.staticData.rasterVertexBuffer,
                 parameters.staticData.quadTriangleIndexBuffer,
                 parameters.staticData.rasterSegments,
                 tile.id);
        }
    }
}

} // namespace mbgl
