#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/fill_extrusion_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/util/convert.hpp>

namespace mbgl {

using namespace style;

void Painter::renderFillExtrusion(PaintParameters& parameters,
                         FillExtrusionBucket& bucket,
                         const FillExtrusionLayer& layer,
                         const RenderTile& tile) {
    const FillExtrusionPaintProperties::Evaluated& properties = layer.impl->paint.evaluated;

    if (pass == RenderPass::Opaque) {
        return;
    }

    // TODO implement texture

    if (!properties.get<FillExtrusionPattern>().from.empty()) {

    } else {
        auto draw = [&] (auto& program) {
            program.draw(
                         context,
                         gl::Triangles(),
                         depthModeForSublayer(0, gl::DepthMode::ReadWrite),
                         stencilModeForClipping(tile.clip),
                         colorModeForRenderPass(),
                         FillExtrusionProgram::UniformValues {
                             uniforms::u_matrix::Value{
                                 tile.translatedMatrix(properties.get<FillExtrusionTranslate>(),
                                                       properties.get<FillExtrusionTranslateAnchor>(),
                                                       state)
                             },
                             uniforms::u_lightcolor::Value{ state.getLightColor() },    // TODO these are all placeholder getters
                             uniforms::u_lightpos::Value{ state.getLightPosition() },
                             uniforms::u_lightintensity::Value{ state.getLightIntensity() }
                         },
                         *bucket.vertexBuffer,
                         *bucket.indexBuffer,
                         bucket.triangleSegments,
                         bucket.paintPropertyBinders.at(layer.getID()),
                         properties,
                         state.getZoom()
                         );
        };

        draw(parameters.programs.fillExtrusion);
    }
}

} // namespace mbgl
