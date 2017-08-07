#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/mat3.hpp>

namespace mbgl {

using namespace style;

static_assert(sizeof(FillExtrusionLayoutVertex) == 12, "expected FillExtrusionLayoutVertex size");

std::array<float, 3> lightColor(const EvaluatedLight& light) {
    const auto color = light.get<LightColor>();
    return {{ color.r, color.g, color.b }};
}

std::array<float, 3> lightPosition(const EvaluatedLight& light, const TransformState& state) {
    auto lightPos = light.get<LightPosition>().getCartesian();
    mat3 lightMat;
    matrix::identity(lightMat);
    if (light.get<LightAnchor>() == LightAnchorType::Viewport) {
        matrix::rotate(lightMat, lightMat, -state.getAngle());
    }
    matrix::transformMat3f(lightPos, lightPos, lightMat);
    return lightPos;
}

float lightIntensity(const EvaluatedLight& light) {
    return light.get<LightIntensity>();
}

FillExtrusionUniforms::Values
FillExtrusionUniforms::values(mat4 matrix,
                              const TransformState& state,
                              const EvaluatedLight& light) {
    return FillExtrusionUniforms::Values{
        uniforms::u_matrix::Value{ matrix },
        uniforms::u_lightcolor::Value{ lightColor(light) },
        uniforms::u_lightpos::Value{ lightPosition(light, state) },
        uniforms::u_lightintensity::Value{ lightIntensity(light) }
    };
}

FillExtrusionPatternUniforms::Values
FillExtrusionPatternUniforms::values(mat4 matrix,
                                     Size atlasSize,
                                     const ImagePosition& a,
                                     const ImagePosition& b,
                                     const Faded<std::string>& fading,
                                     const UnwrappedTileID& tileID,
                                     const TransformState& state,
                                     const float heightFactor,
                                     const EvaluatedLight& light) {
    int32_t tileSizeAtNearestZoom = util::tileSize * state.zoomScale(state.getIntegerZoom() - tileID.canonical.z);
    int32_t pixelX = tileSizeAtNearestZoom * (tileID.canonical.x + tileID.wrap * state.zoomScale(tileID.canonical.z));
    int32_t pixelY = tileSizeAtNearestZoom * tileID.canonical.y;

    return FillExtrusionPatternUniforms::Values{
        uniforms::u_matrix::Value{ matrix },
        uniforms::u_pattern_tl_a::Value{ a.tl() },
        uniforms::u_pattern_br_a::Value{ a.br() },
        uniforms::u_pattern_tl_b::Value{ b.tl() },
        uniforms::u_pattern_br_b::Value{ b.br() },
        uniforms::u_pattern_size_a::Value{ a.displaySize() },
        uniforms::u_pattern_size_b::Value{ b.displaySize() },
        uniforms::u_scale_a::Value{ fading.fromScale },
        uniforms::u_scale_b::Value{ fading.toScale },
        uniforms::u_texsize::Value{ atlasSize },
        uniforms::u_mix::Value{ fading.t },
        uniforms::u_image::Value{ 0 },
        uniforms::u_pixel_coord_upper::Value{ std::array<float, 2>{{ float(pixelX >> 16), float(pixelY >> 16) }} },
        uniforms::u_pixel_coord_lower::Value{ std::array<float, 2>{{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }} },
        uniforms::u_tile_units_to_pixels::Value{ 1.0f / tileID.pixelsToTileUnits(1.0f, state.getIntegerZoom()) },
        uniforms::u_height_factor::Value{ heightFactor },
        uniforms::u_lightcolor::Value{ lightColor(light) },
        uniforms::u_lightpos::Value{ lightPosition(light, state) },
        uniforms::u_lightintensity::Value{ lightIntensity(light) },
    };
}

} // namespace mbgl
