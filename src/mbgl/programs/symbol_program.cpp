#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/util/enum.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {

using namespace style;

static_assert(sizeof(SymbolLayoutVertex) == 16, "expected SymbolLayoutVertex size");

std::unique_ptr<SymbolSizeBinder> SymbolSizeBinder::create(const float tileZoom,
                                                    const style::DataDrivenPropertyValue<float>& sizeProperty,
                                                    const float defaultValue) {
    return sizeProperty.match(
        [&] (const style::CompositeFunction<float>& function) -> std::unique_ptr<SymbolSizeBinder> {
            return std::make_unique<CompositeFunctionSymbolSizeBinder>(tileZoom, function, defaultValue);
        },
        [&] (const style::SourceFunction<float>& function) {
            return std::make_unique<SourceFunctionSymbolSizeBinder>(tileZoom, function, defaultValue);
        },
        [&] (const auto& value) -> std::unique_ptr<SymbolSizeBinder> {
            return std::make_unique<ConstantSymbolSizeBinder>(tileZoom, value, defaultValue);
        }
    );
}

template <class Values, class...Args>
Values makeValues(const bool isText,
                  const style::SymbolPropertyValues& values,
                  const Size& texsize,
                  const std::array<float, 2>& pixelsToGLUnits,
                  const RenderTile& tile,
                  const TransformState& state,
                  Args&&... args) {
    std::array<float, 2> extrudeScale;
    
    if (values.pitchAlignment == AlignmentType::Map) {
        extrudeScale.fill(tile.id.pixelsToTileUnits(1, state.getZoom()));
    } else {
        extrudeScale = {{
            pixelsToGLUnits[0] * state.getCameraToCenterDistance(),
            pixelsToGLUnits[1] * state.getCameraToCenterDistance()
        }};
    }
        
    return Values {
        uniforms::u_matrix::Value{ tile.translatedMatrix(values.translate,
                                   values.translateAnchor,
                                   state) },
        uniforms::u_extrude_scale::Value{ extrudeScale },
        uniforms::u_texsize::Value{ std::array<float, 2> {{ float(texsize.width) / 4, float(texsize.height) / 4 }} },
        uniforms::u_zoom::Value{ float(state.getZoom()) },
        uniforms::u_rotate_with_map::Value{ values.rotationAlignment == AlignmentType::Map },
        uniforms::u_texture::Value{ 0 },
        uniforms::u_fadetexture::Value{ 1 },
        uniforms::u_is_text::Value{ isText },
        std::forward<Args>(args)...
    };
}

SymbolIconProgram::UniformValues
SymbolIconProgram::uniformValues(const bool isText,
                                 const style::SymbolPropertyValues& values,
                                 const Size& texsize,
                                 const std::array<float, 2>& pixelsToGLUnits,
                                 const RenderTile& tile,
                                 const TransformState& state)
{
    return makeValues<SymbolIconProgram::UniformValues>(
        isText,
        values,
        texsize,
        pixelsToGLUnits,
        tile,
        state
    );
}

template <class PaintProperties>
typename SymbolSDFProgram<PaintProperties>::UniformValues SymbolSDFProgram<PaintProperties>::uniformValues(
      const bool isText,
      const style::SymbolPropertyValues& values,
      const Size& texsize,
      const std::array<float, 2>& pixelsToGLUnits,
      const RenderTile& tile,
      const TransformState& state,
      const SymbolSDFPart part)
{
    const float gammaScale = (values.pitchAlignment == AlignmentType::Map
                              ? std::cos(state.getPitch())
                              : 1.0) * state.getCameraToCenterDistance();
    
    return makeValues<SymbolSDFProgram<PaintProperties>::UniformValues>(
        isText,
        values,
        texsize,
        pixelsToGLUnits,
        tile,
        state,
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
