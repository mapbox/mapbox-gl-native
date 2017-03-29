#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/fill_extrusion_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/util/constants.hpp>
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

    const auto light = style.light.evaluated;
    const auto lightColor = light.get<LightColor>();
    std::array<float, 3> color3f = { { lightColor.r, lightColor.g, lightColor.b } };
    const auto lightPos =
        light.get<LightPosition>().get(light.get<LightAnchor>(), state.getAngle());
    const auto lightIntensity = light.get<LightIntensity>();

    if (!properties.get<FillExtrusionPattern>().from.empty()) {
        optional<SpriteAtlasElement> imagePosA =
            spriteAtlas->getPattern(properties.get<FillExtrusionPattern>().from);
        optional<SpriteAtlasElement> imagePosB =
            spriteAtlas->getPattern(properties.get<FillExtrusionPattern>().to);

        if (!imagePosA || !imagePosB) {
            return;
        }

        spriteAtlas->bind(true, context, 0);

        parameters.programs.fillExtrusionPattern.draw(
            context, gl::Triangles(), depthModeForSublayer(0, gl::DepthMode::ReadWrite),
            gl::StencilMode::disabled(), colorModeForRenderPass(),
            FillExtrusionPatternUniforms::values(
                tile.translatedMatrix(properties.get<FillExtrusionTranslate>(),
                                      properties.get<FillExtrusionTranslateAnchor>(), state),
                *imagePosA, *imagePosB, properties.get<FillExtrusionPattern>(), tile.id, state,
                -std::pow(2, tile.id.canonical.z) / util::tileSize / 8.0f, color3f, lightPos,
                lightIntensity),
            *bucket.vertexBuffer, *bucket.indexBuffer, bucket.triangleSegments,
            bucket.paintPropertyBinders.at(layer.getID()), properties, state.getZoom());

    } else {
        parameters.programs.fillExtrusion.draw(
            context, gl::Triangles(), depthModeForSublayer(0, gl::DepthMode::ReadWrite),
            gl::StencilMode::disabled(), colorModeForRenderPass(),
            FillExtrusionProgram::UniformValues{
                uniforms::u_matrix::Value{
                    tile.translatedMatrix(properties.get<FillExtrusionTranslate>(),
                                          properties.get<FillExtrusionTranslateAnchor>(), state) },
                uniforms::u_lightcolor::Value{ color3f }, uniforms::u_lightpos::Value{ lightPos },
                uniforms::u_lightintensity::Value{ lightIntensity } },
            *bucket.vertexBuffer, *bucket.indexBuffer, bucket.triangleSegments,
            bucket.paintPropertyBinders.at(layer.getID()), properties, state.getZoom());
    };
}

} // namespace mbgl
