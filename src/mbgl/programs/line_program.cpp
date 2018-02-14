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
        uniforms::u_matrix::Value{
            tile.translatedMatrix(properties.get<LineTranslate>(),
                                  properties.get<LineTranslateAnchor>(),
                                  state)
        },
        uniforms::u_ratio::Value{ 1.0f / tile.id.pixelsToTileUnits(1.0, state.getZoom()) },
        uniforms::u_gl_units_to_pixels::Value{{{ 1.0f / pixelsToGLUnits[0], 1.0f / pixelsToGLUnits[1] }}},
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

LineGradientProgram::UniformValues
LineGradientProgram::uniformValues(const RenderLinePaintProperties::PossiblyEvaluated& properties,
                           const RenderTile& tile,
                           const TransformState& state,
                           const std::array<float, 2>& pixelsToGLUnits) {
    return makeValues<LineGradientProgram::UniformValues>(
        properties,
        tile,
        state,
        pixelsToGLUnits,
        uniforms::u_image::Value{ 0 }
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
                              float atlasWidth) {
    const float widthA = posA.width * properties.get<LineDasharray>().fromScale;
    const float widthB = posB.width * properties.get<LineDasharray>().toScale;

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
        uniforms::u_patternscale_a::Value{ scaleA },
        uniforms::u_patternscale_b::Value{ scaleB },
        uniforms::u_tex_y_a::Value{ posA.y },
        uniforms::u_tex_y_b::Value{ posB.y },
        uniforms::u_mix::Value{ properties.get<LineDasharray>().t },
        uniforms::u_sdfgamma::Value{ atlasWidth / (std::min(widthA, widthB) * 256.0f * pixelRatio) / 2.0f },
        uniforms::u_image::Value{ 0 }
    );
}

LinePatternProgram::UniformValues
LinePatternProgram::uniformValues(const RenderLinePaintProperties::PossiblyEvaluated& properties,
                                  const RenderTile& tile,
                                  const TransformState& state,
                                  const std::array<float, 2>& pixelsToGLUnits,
                                  const Size atlasSize,
                                  const ImagePosition& posA,
                                  const ImagePosition& posB) {
     std::array<float, 2> sizeA {{
         tile.id.pixelsToTileUnits(posA.displaySize()[0] * properties.get<LinePattern>().fromScale, state.getIntegerZoom()),
         posA.displaySize()[1]
     }};

     std::array<float, 2> sizeB {{
         tile.id.pixelsToTileUnits(posB.displaySize()[0] * properties.get<LinePattern>().toScale, state.getIntegerZoom()),
         posB.displaySize()[1]
     }};

    return makeValues<LinePatternProgram::UniformValues>(
        properties,
        tile,
        state,
        pixelsToGLUnits,
        uniforms::u_pattern_tl_a::Value{ posA.tl() },
        uniforms::u_pattern_br_a::Value{ posA.br() },
        uniforms::u_pattern_tl_b::Value{ posB.tl() },
        uniforms::u_pattern_br_b::Value{ posB.br() },
        uniforms::u_pattern_size_a::Value{ sizeA },
        uniforms::u_pattern_size_b::Value{ sizeB },
        uniforms::u_texsize::Value{ atlasSize },
        uniforms::u_fade::Value{ properties.get<LinePattern>().t },
        uniforms::u_image::Value{ 0 }
    );
}

} // namespace mbgl
