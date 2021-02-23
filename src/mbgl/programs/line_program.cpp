#include <mbgl/programs/line_program.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/mat2.hpp>
#include <mbgl/geometry/line_atlas.hpp>

namespace mbgl {

using namespace style;

static_assert(sizeof(LineLayoutVertex) == 8, "expected LineLayoutVertex size");

template <class Values, class...Args>
Values makeValues(const style::LinePaintProperties::PossiblyEvaluated& properties,
                  const RenderTile& tile,
                  const TransformState& state,
                  const std::array<float, 2>& pixelsToGLUnits,
                  const float pixelRatio,
                  Args&&... args) {

    return Values {
        uniforms::matrix::Value(
            tile.translatedMatrix(properties.get<LineTranslate>(),
                                  properties.get<LineTranslateAnchor>(),
                                  state)
        ),
        uniforms::ratio::Value( 1.0f / tile.id.pixelsToTileUnits(1.0, state.getZoom()) ),
        uniforms::units_to_pixels::Value({ {1.0f / pixelsToGLUnits[0], 1.0f / pixelsToGLUnits[1]} }),
        uniforms::device_pixel_ratio::Value( pixelRatio ),
        std::forward<Args>(args)...
    };
}

LineProgram::LayoutUniformValues
LineProgram::layoutUniformValues(const style::LinePaintProperties::PossiblyEvaluated& properties,
                                 const RenderTile& tile,
                                 const TransformState& state,
                                 const std::array<float, 2>& pixelsToGLUnits,
                                 const float pixelRatio) {
    return makeValues<LineProgram::LayoutUniformValues>(
        properties,
        tile,
        state,
        pixelsToGLUnits,
        pixelRatio
    );
}

LineSDFProgram::LayoutUniformValues
LineSDFProgram::layoutUniformValues(const style::LinePaintProperties::PossiblyEvaluated& properties,
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

    return makeValues<LineSDFProgram::LayoutUniformValues>(
        properties,
        tile,
        state,
        pixelsToGLUnits,
        pixelRatio,
        uniforms::patternscale_a::Value( scaleA ),
        uniforms::patternscale_b::Value( scaleB ),
        uniforms::tex_y_a::Value( posA.y ),
        uniforms::tex_y_b::Value( posB.y ),
        uniforms::mix::Value( crossfade.t ),
        uniforms::sdfgamma::Value( atlasWidth / (std::min(widthA, widthB) * 256.0f * pixelRatio) / 2.0f )
    );
}

LinePatternProgram::LayoutUniformValues LinePatternProgram::layoutUniformValues(
    const style::LinePaintProperties::PossiblyEvaluated& properties,
    const RenderTile& tile,
    const TransformState& state,
    const std::array<float, 2>& pixelsToGLUnits,
    const float pixelRatio,
    const Size atlasSize,
    const CrossfadeParameters& crossfade) {

    const auto tileRatio = 1 / tile.id.pixelsToTileUnits(1, state.getIntegerZoom());

    return makeValues<LinePatternProgram::LayoutUniformValues>(
        properties,
        tile,
        state,
        pixelsToGLUnits,
        pixelRatio,
        uniforms::scale::Value ({ {pixelRatio, tileRatio, crossfade.fromScale, crossfade.toScale} }),
        uniforms::texsize::Value( atlasSize ),
        uniforms::fade::Value( crossfade.t )
    );
}

LineGradientProgram::LayoutUniformValues LineGradientProgram::layoutUniformValues(
    const style::LinePaintProperties::PossiblyEvaluated& properties,
    const RenderTile& tile,
    const TransformState& state,
    const std::array<float, 2>& pixelsToGLUnits,
    const float pixelRatio) {
    return makeValues<LineGradientProgram::LayoutUniformValues>(
        properties,
        tile,
        state,
        pixelsToGLUnits,
        pixelRatio
    );
}

} // namespace mbgl
