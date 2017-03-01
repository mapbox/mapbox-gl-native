#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/util/enum.hpp>

namespace mbgl {

using namespace style;

static_assert(sizeof(SymbolLayoutVertex) == 16, "expected SymbolLayoutVertex size");

template <class Values, class...Args>
Values makeValues(const style::SymbolPropertyValues& values,
                  const Size& texsize,
                  const std::array<float, 2>& pixelsToGLUnits,
                  const RenderTile& tile,
                  const TransformState& state,
                  Args&&... args) {
    std::array<float, 2> extrudeScale;

    const float scale = values.paintSize / values.sdfScale;
    
    if (values.pitchAlignment == AlignmentType::Map) {
        extrudeScale.fill(tile.id.pixelsToTileUnits(1, state.getZoom()) * scale);
    } else {
        extrudeScale = {{
            pixelsToGLUnits[0] * scale * state.getCameraToCenterDistance(),
            pixelsToGLUnits[1] * scale * state.getCameraToCenterDistance()
        }};
    }
    
    // adjust min/max zooms for variable font sies
    float zoomAdjust = std::log(values.paintSize / values.layoutSize) / std::log(2);

    return Values {
        uniforms::u_matrix::Value{ tile.translatedMatrix(values.translate,
                                   values.translateAnchor,
                                   state) },
        uniforms::u_extrude_scale::Value{ extrudeScale },
        uniforms::u_texsize::Value{ std::array<float, 2> {{ float(texsize.width) / 4, float(texsize.height) / 4 }} },
        uniforms::u_zoom::Value{ float((state.getZoom() - zoomAdjust) * 10) },
        uniforms::u_rotate_with_map::Value{ values.rotationAlignment == AlignmentType::Map },
        uniforms::u_texture::Value{ 0 },
        uniforms::u_fadetexture::Value{ 1 },
        std::forward<Args>(args)...
    };
}

SymbolIconProgram::UniformValues
SymbolIconProgram::uniformValues(const style::SymbolPropertyValues& values,
                                 const Size& texsize,
                                 const std::array<float, 2>& pixelsToGLUnits,
                                 const RenderTile& tile,
                                 const TransformState& state)
{
    return makeValues<SymbolIconProgram::UniformValues>(
        values,
        texsize,
        pixelsToGLUnits,
        tile,
        state
    );
}

template <class PaintProperties>
typename SymbolSDFProgram<PaintProperties>::UniformValues SymbolSDFProgram<PaintProperties>::uniformValues(const style::SymbolPropertyValues& values,
                              const Size& texsize,
                              const std::array<float, 2>& pixelsToGLUnits,
                              const RenderTile& tile,
                              const TransformState& state,
                              const SymbolSDFPart part)
{
    const float scale = values.paintSize / values.sdfScale;
    
    const float gammaScale = scale * (values.pitchAlignment == AlignmentType::Map
                              ? std::cos(state.getPitch())
                              : 1.0) * state.getCameraToCenterDistance();
    
    return makeValues<SymbolSDFProgram<PaintProperties>::UniformValues>(
        values,
        texsize,
        pixelsToGLUnits,
        tile,
        state,
        uniforms::u_font_scale::Value{ scale },
        uniforms::u_gamma_scale::Value{ gammaScale },
        uniforms::u_pitch::Value{ state.getPitch() },
        uniforms::u_bearing::Value{ -1.0f * state.getAngle() },
        uniforms::u_aspect_ratio::Value{ (state.getSize().width * 1.0f) / (state.getSize().height * 1.0f) },
        uniforms::u_pitch_with_map::Value{ values.pitchAlignment == AlignmentType::Map },
        uniforms::u_is_halo::Value{ part == SymbolSDFPart::Halo }
    );
}

template class SymbolSDFProgram<style::IconPaintProperties>;
template class SymbolSDFProgram<style::TextPaintProperties>;

} // namespace mbgl
