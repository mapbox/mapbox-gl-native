#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/layout/symbol_projection.hpp>
#include <mbgl/tile/tile.hpp>
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
                  const bool alongLine,
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

    const float pixelsToTileUnits = tile.id.pixelsToTileUnits(1.0, state.getZoom());
    const bool pitchWithMap = values.pitchAlignment == style::AlignmentType::Map;
    const bool rotateWithMap = values.rotationAlignment == style::AlignmentType::Map;
    
    // Line label rotation happens in `updateLineLabels`
    // Pitched point labels are automatically rotated by the labelPlaneMatrix projection
    // Unpitched point labels need to have their rotation applied after projection
    const bool rotateInShader = rotateWithMap && !pitchWithMap && !alongLine;

    mat4 labelPlaneMatrix;
    if (alongLine) {
        // For labels that follow lines the first part of the projection is handled on the cpu.
        // Pass an identity matrix because no transformation needs to be done in the vertex shader.
        matrix::identity(labelPlaneMatrix);
    } else {
        labelPlaneMatrix = getLabelPlaneMatrix(tile.matrix, pitchWithMap, rotateWithMap, state, pixelsToTileUnits);
    }

    mat4 glCoordMatrix = getGlCoordMatrix(tile.matrix, pitchWithMap, rotateWithMap, state, pixelsToTileUnits);
        
    return Values {
        uniforms::u_matrix::Value{ tile.translatedMatrix(values.translate,
                                   values.translateAnchor,
                                   state) },
        uniforms::u_label_plane_matrix::Value{labelPlaneMatrix},
        uniforms::u_gl_coord_matrix::Value{ tile.translateVtxMatrix(glCoordMatrix,
                                            values.translate,
                                            values.translateAnchor,
                                            state,
                                            true) },
        uniforms::u_extrude_scale::Value{ extrudeScale },
        uniforms::u_texsize::Value{ texsize },
        uniforms::u_texture::Value{ 0 },
        uniforms::u_fadetexture::Value{ 1 },
        uniforms::u_is_text::Value{ isText },
        uniforms::u_collision_y_stretch::Value{ tile.tile.yStretch() },
        uniforms::u_camera_to_center_distance::Value{ state.getCameraToCenterDistance() },
        uniforms::u_pitch::Value{ state.getPitch() },
        uniforms::u_pitch_with_map::Value{ pitchWithMap },
        uniforms::u_max_camera_distance::Value{ values.maxCameraDistance },
        uniforms::u_rotate_symbol::Value{ rotateInShader },
        uniforms::u_aspect_ratio::Value{ state.getSize().aspectRatio() },
        std::forward<Args>(args)...
    };
}

SymbolIconProgram::UniformValues
SymbolIconProgram::uniformValues(const bool isText,
                                 const style::SymbolPropertyValues& values,
                                 const Size& texsize,
                                 const std::array<float, 2>& pixelsToGLUnits,
                                 const bool alongLine,
                                 const RenderTile& tile,
                                 const TransformState& state)
{
    return makeValues<SymbolIconProgram::UniformValues>(
        isText,
        values,
        texsize,
        pixelsToGLUnits,
        alongLine,
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
      const bool alongLine,
      const RenderTile& tile,
      const TransformState& state,
      const SymbolSDFPart part)
{
    const float gammaScale = (values.pitchAlignment == AlignmentType::Map
                              ? std::cos(state.getPitch()) * state.getCameraToCenterDistance()
                              : 1.0);
    
    return makeValues<SymbolSDFProgram<PaintProperties>::UniformValues>(
        isText,
        values,
        texsize,
        pixelsToGLUnits,
        alongLine,
        tile,
        state,
        uniforms::u_gamma_scale::Value{ gammaScale },
        uniforms::u_is_halo::Value{ part == SymbolSDFPart::Halo }
    );
}

template class SymbolSDFProgram<style::IconPaintProperties>;
template class SymbolSDFProgram<style::TextPaintProperties>;

} // namespace mbgl
