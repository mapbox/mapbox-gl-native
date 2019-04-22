#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/gfx/context_impl.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/mat3.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<FillExtrusionProgram>> gfx::Context::createProgram(const ProgramParameters&);
template std::unique_ptr<gfx::Program<FillExtrusionPatternProgram>> gfx::Context::createProgram(const ProgramParameters&);

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
        matrix::rotate(lightMat, lightMat, -state.getBearing());
    }
    matrix::transformMat3f(lightPos, lightPos, lightMat);
    return lightPos;
}

float lightIntensity(const EvaluatedLight& light) {
    return light.get<LightIntensity>();
}

FillExtrusionProgram::LayoutUniformValues FillExtrusionProgram::layoutUniformValues(
    mat4 matrix, const TransformState& state, const EvaluatedLight& light) {
    return {
        uniforms::matrix::Value( matrix ),
        uniforms::lightcolor::Value( lightColor(light) ),
        uniforms::lightpos::Value( lightPosition(light, state) ),
        uniforms::lightintensity::Value( lightIntensity(light) )
    };
}

FillExtrusionPatternProgram::LayoutUniformValues
FillExtrusionPatternProgram::layoutUniformValues(mat4 matrix,
                                           Size atlasSize,
                                           const CrossfadeParameters& crossfade,
                                           const UnwrappedTileID& tileID,
                                           const TransformState& state,
                                           const float heightFactor,
                                           const float pixelRatio,
                                           const EvaluatedLight& light) {
    const auto tileRatio = 1 / tileID.pixelsToTileUnits(1, state.getIntegerZoom());
    int32_t tileSizeAtNearestZoom = util::tileSize * state.zoomScale(state.getIntegerZoom() - tileID.canonical.z);
    int32_t pixelX = tileSizeAtNearestZoom * (tileID.canonical.x + tileID.wrap * state.zoomScale(tileID.canonical.z));
    int32_t pixelY = tileSizeAtNearestZoom * tileID.canonical.y;

    return {
        uniforms::matrix::Value( matrix ),
        uniforms::scale::Value( {{pixelRatio, tileRatio, crossfade.fromScale, crossfade.toScale}} ),
        uniforms::texsize::Value( atlasSize ),
        uniforms::fade::Value( crossfade.t ),
        uniforms::pixel_coord_upper::Value( std::array<float, 2>{{ float(pixelX >> 16), float(pixelY >> 16) }} ),
        uniforms::pixel_coord_lower::Value( std::array<float, 2>{{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }} ),
        uniforms::height_factor::Value( heightFactor ),
        uniforms::lightcolor::Value( lightColor(light) ),
        uniforms::lightpos::Value( lightPosition(light, state) ),
        uniforms::lightintensity::Value( lightIntensity(light) ),
    };
}

} // namespace mbgl
