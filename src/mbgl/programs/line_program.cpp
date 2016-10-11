#include <mbgl/programs/line_program.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/mat2.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>

namespace mbgl {

static_assert(sizeof(LineAttributes::Vertex) == 8, "expected LineVertex size");

template <class Values, class...Args>
Values makeValues(const style::LinePaintProperties& properties,
                  const RenderTile& tile,
                  const TransformState& state,
                  Args&&... args) {

    mat2 antialiasingMatrix;
    matrix::identity(antialiasingMatrix);
    matrix::scale(antialiasingMatrix, antialiasingMatrix, 1.0, std::cos(state.getPitch()));
    matrix::rotate(antialiasingMatrix, antialiasingMatrix, state.getAngle());

    // calculate how much longer the real world distance is at the top of the screen
    // than at the middle of the screen.
    float topedgelength = std::sqrt(std::pow(state.getSize().height, 2.0f) / 4.0f  * (1.0f + std::pow(state.getAltitude(), 2.0f)));
    float x = state.getSize().height / 2.0f * std::tan(state.getPitch());

    return Values {
        uniforms::u_matrix::Value{ tile.translatedMatrix(properties.lineTranslate.value,
                                   properties.lineTranslateAnchor.value,
                                   state) },
        uniforms::u_opacity::Value{ properties.lineOpacity.value },
        uniforms::u_width::Value{ properties.lineWidth.value / 2 },
        uniforms::u_gapwidth::Value{ properties.lineGapWidth.value / 2 },
        uniforms::u_blur::Value{ properties.lineBlur.value },
        uniforms::u_offset::Value{ -properties.lineOffset.value },
        uniforms::u_antialiasingmatrix::Value{ antialiasingMatrix },
        uniforms::u_ratio::Value{ 1.0f / tile.id.pixelsToTileUnits(1.0, state.getZoom()) },
        uniforms::u_extra::Value{ (topedgelength + x) / topedgelength - 1.0f },
        std::forward<Args>(args)...
    };
}

LineProgram::UniformValues
LineProgram::uniformValues(const style::LinePaintProperties& properties,
                           const RenderTile& tile,
                           const TransformState& state) {
    return makeValues<LineProgram::UniformValues>(
        properties,
        tile,
        state,
        uniforms::u_color::Value{ properties.lineColor.value }
    );
}

LineSDFProgram::UniformValues
LineSDFProgram::uniformValues(const style::LinePaintProperties& properties,
                              float pixelRatio,
                              const RenderTile& tile,
                              const TransformState& state,
                              const LinePatternPos& posA,
                              const LinePatternPos& posB,
                              float dashLineWidth,
                              float atlasWidth) {
    const float widthA = posA.width * properties.lineDasharray.value.fromScale * dashLineWidth;
    const float widthB = posB.width * properties.lineDasharray.value.toScale * dashLineWidth;

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
        uniforms::u_color::Value{ properties.lineColor.value },
        uniforms::u_patternscale_a::Value{ scaleA },
        uniforms::u_patternscale_b::Value{ scaleB },
        uniforms::u_tex_y_a::Value{ posA.y },
        uniforms::u_tex_y_b::Value{ posB.y },
        uniforms::u_mix::Value{ properties.lineDasharray.value.t },
        uniforms::u_sdfgamma::Value{ atlasWidth / (std::min(widthA, widthB) * 256.0f * pixelRatio) / 2.0f },
        uniforms::u_image::Value{ 0 }
    );
}

LinePatternProgram::UniformValues
LinePatternProgram::uniformValues(const style::LinePaintProperties& properties,
                                  const RenderTile& tile,
                                  const TransformState& state,
                                  const SpriteAtlasPosition& posA,
                                  const SpriteAtlasPosition& posB) {
     std::array<float, 2> sizeA {{
         tile.id.pixelsToTileUnits(posA.size[0] * properties.linePattern.value.fromScale, state.getIntegerZoom()),
         posA.size[1]
     }};

     std::array<float, 2> sizeB {{
         tile.id.pixelsToTileUnits(posB.size[0] * properties.linePattern.value.toScale, state.getIntegerZoom()),
         posB.size[1]
     }};

    return makeValues<LinePatternProgram::UniformValues>(
        properties,
        tile,
        state,
        uniforms::u_pattern_tl_a::Value{ posA.tl },
        uniforms::u_pattern_br_a::Value{ posA.br },
        uniforms::u_pattern_tl_b::Value{ posB.tl },
        uniforms::u_pattern_br_b::Value{ posB.br },
        uniforms::u_pattern_size_a::Value{ sizeA },
        uniforms::u_pattern_size_b::Value{ sizeB },
        uniforms::u_fade::Value{ properties.linePattern.value.t },
        uniforms::u_image::Value{ 0 }
    );
}

} // namespace mbgl
