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
#include <mbgl/util/mat3.hpp>

namespace mbgl {

using namespace style;

void Painter::renderFillExtrusion(PaintParameters& parameters,
                         FillExtrusionBucket& bucket,
                         const FillExtrusionLayer& layer,
                         const RenderTile& tile,
                         const Style& style) {
    const FillExtrusionPaintProperties::Evaluated& properties = layer.impl->paint.evaluated;

    if (pass == RenderPass::Opaque) {
        return;
    }

    auto lightpos = style.light.getPosition();
    vec3f lightvec{ lightpos };
    mat3 lightmat;
    matrix::identity(lightmat);
    if (style.light.getAnchor() == LightAnchorType::Viewport) {
        matrix::rotate(lightmat, lightmat, -state.getAngle());
    }
    matrix::transformMat3f(lightvec, lightvec, lightmat);

    if (!properties.get<FillExtrusionPattern>().from.empty()) {

    } else {
        auto draw = [&] (auto& program) {
            program.draw(
                         context,
                         gl::Triangles(),
                         depthModeForSublayer(0, gl::DepthMode::ReadWrite),
                         gl::StencilMode::disabled(),
                         colorModeForRenderPass(),
                         FillExtrusionProgram::UniformValues {
                             uniforms::u_matrix::Value{
                                 tile.translatedMatrix(properties.get<FillExtrusionTranslate>(),
                                                       properties.get<FillExtrusionTranslateAnchor>(),
                                                       state)
                             },
                             uniforms::u_lightcolor::Value{ style.light.getColor() },
                             uniforms::u_lightpos::Value{ lightvec },
                             uniforms::u_lightintensity::Value{ style.light.getIntensity() }
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
