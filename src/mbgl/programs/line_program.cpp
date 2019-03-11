#include <mbgl/programs/line_program.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/mat2.hpp>
#include <mbgl/geometry/line_atlas.hpp>

namespace mbgl {

using namespace style;

static_assert(sizeof(LineLayoutVertex) == 12, "expected LineLayoutVertex size");

template <class Values, class...Args>
Values makeValues(const RenderLinePaintProperties::PossiblyEvaluated& properties,
                  const RenderTile& tile,
                  const TransformState& state,
                  const std::array<float, 2>& pixelsToGLUnits,
                  Args&&... args) {

    return Values {
        uniforms::u_matrix::Value(
            tile.translatedMatrix(properties.get<LineTranslate>(),
                                  properties.get<LineTranslateAnchor>(),
                                  state)
        ),
        uniforms::u_ratio::Value( 1.0f / tile.id.pixelsToTileUnits(1.0, state.getZoom()) ),
        uniforms::u_gl_units_to_pixels::Value({ {1.0f / pixelsToGLUnits[0], 1.0f / pixelsToGLUnits[1]} }),
        std::forward<Args>(args)...
    };
}

LineProgram::UniformValues
LineProgram::uniformValues(const RenderLinePaintProperties::PossiblyEvaluated& properties,
                           const RenderTile& tile,
                           const TransformState& state,
                           const std::array<float, 2>& pixelsToGLUnits) {
    return makeValues<LineProgram::UniformValues>(
        properties,
        tile,
        state,
        pixelsToGLUnits
    );
}

LineSDFProgram::UniformValues
LineSDFProgram::uniformValues(const RenderLinePaintProperties::PossiblyEvaluated& properties,
                              float pixelRatio,
                              const RenderTile& tile,
                              const TransformState& state,
                              const std::array<float, 2>& pixelsToGLUnits,
                              const LinePatternPos& posA,
                              const LinePatternPos& posB,
                              const CrossfadeParameters& crossfade,
                              float atlasWidth) {
    const float widthA = posA.width * crossfade.fromScale;
    const float widthB = posB.width * crossfade.toScale;

    std::array<float, 2> scaleA {{
        1.0f / tile.id.pixelsToTileUnits(widthA, state.getIntegerZoom()),
        -posA.height / 2.0f
    }};

    std::array<float, 2> scaleB {{
        1.0f / tile.id.pixelsToTileUnits(widthB, state.getIntegerZoom()),
        -posB.height / 2.0f
    }};

    return makeValues<LineSDFProgram::UniformValues>(
        properties,
        tile,
        state,
        pixelsToGLUnits,
        uniforms::u_patternscale_a::Value( scaleA ),
        uniforms::u_patternscale_b::Value( scaleB ),
        uniforms::u_tex_y_a::Value( posA.y ),
        uniforms::u_tex_y_b::Value( posB.y ),
        uniforms::u_mix::Value( crossfade.t ),
        uniforms::u_sdfgamma::Value( atlasWidth / (std::min(widthA, widthB) * 256.0f * pixelRatio) / 2.0f )
    );
}

LinePatternProgram::UniformValues
LinePatternProgram::uniformValues(const RenderLinePaintProperties::PossiblyEvaluated& properties,
                                  const RenderTile& tile,
                                  const TransformState& state,
                                  const std::array<float, 2>& pixelsToGLUnits,
                                  const Size atlasSize,
                                  const CrossfadeParameters& crossfade,
                                  const float pixelRatio) {
    
    const auto tileRatio = 1 / tile.id.pixelsToTileUnits(1, state.getIntegerZoom());

    return makeValues<LinePatternProgram::UniformValues>(
        properties,
        tile,
        state,
        pixelsToGLUnits,
        uniforms::u_scale::Value ({ {pixelRatio, tileRatio, crossfade.fromScale, crossfade.toScale} }),
        uniforms::u_texsize::Value( atlasSize ),
        uniforms::u_fade::Value( crossfade.t )
    );
}

LineGradientProgram::UniformValues
LineGradientProgram::uniformValues(const RenderLinePaintProperties::PossiblyEvaluated& properties,
                           const RenderTile& tile,
                           const TransformState& state,
                           const std::array<float, 2>& pixelsToGLUnits) {
    return makeValues<LineGradientProgram::UniformValues>(
        properties,
        tile,
        state,
        pixelsToGLUnits
    );
}

} // namespace mbgl
