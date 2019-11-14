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
                                                    const style::PropertyValue<float>& sizeProperty,
                                                    const float defaultValue) {
    return sizeProperty.match(
        [&] (const Undefined& value) -> std::unique_ptr<SymbolSizeBinder> {
            return std::make_unique<ConstantSymbolSizeBinder>(tileZoom, value, defaultValue);
        },
        [&] (float value) -> std::unique_ptr<SymbolSizeBinder> {
            return std::make_unique<ConstantSymbolSizeBinder>(tileZoom, value, defaultValue);
        },
        [&] (const style::PropertyExpression<float>& expression) -> std::unique_ptr<SymbolSizeBinder> {
            if (expression.isFeatureConstant()) {
                return std::make_unique<ConstantSymbolSizeBinder>(tileZoom, expression, defaultValue);
            } else if (expression.isZoomConstant()) {
                return std::make_unique<SourceFunctionSymbolSizeBinder>(tileZoom, expression, defaultValue);
            } else {
                return std::make_unique<CompositeFunctionSymbolSizeBinder>(tileZoom, expression, defaultValue);
            }
        }
    );
}

template <class Values, class...Args>
Values makeValues(const bool isText,
                  const bool hasVariablePacement,
                  const style::SymbolPropertyValues& values,
                  const Size& texsize,
                  const std::array<float, 2>& pixelsToGLUnits,
                  const bool alongLine,
                  const RenderTile& tile,
                  const TransformState& state,
                  const float symbolFadeChange,
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

    // Line label rotation happens in `updateLineLabels`/`reprojectLineLabels``
    // Pitched point labels are automatically rotated by the labelPlaneMatrix projection
    // Unpitched point labels need to have their rotation applied after projection
    const bool rotateInShader = rotateWithMap && !pitchWithMap && !alongLine;

    mat4 labelPlaneMatrix;
    if (alongLine || hasVariablePacement) {
        // For labels that follow lines the first part of the projection is handled on the cpu.
        // Pass an identity matrix because no transformation needs to be done in the vertex shader.
        matrix::identity(labelPlaneMatrix);
    } else {
        labelPlaneMatrix = getLabelPlaneMatrix(tile.matrix, pitchWithMap, rotateWithMap, state, pixelsToTileUnits);
    }

    mat4 glCoordMatrix = getGlCoordMatrix(tile.matrix, pitchWithMap, rotateWithMap, state, pixelsToTileUnits);

    return Values {
        uniforms::matrix::Value( tile.translatedMatrix(values.translate,
                                   values.translateAnchor,
                                   state) ),
        uniforms::label_plane_matrix::Value(labelPlaneMatrix),
        uniforms::coord_matrix::Value( tile.translateVtxMatrix(glCoordMatrix,
                                            values.translate,
                                            values.translateAnchor,
                                            state,
                                            true) ),
        uniforms::extrude_scale::Value( extrudeScale ),
        uniforms::texsize::Value( texsize ),
        uniforms::fade_change::Value( symbolFadeChange ),
        uniforms::is_text::Value( isText ),
        uniforms::camera_to_center_distance::Value( state.getCameraToCenterDistance() ),
        uniforms::pitch::Value( state.getPitch() ),
        uniforms::pitch_with_map::Value( pitchWithMap ),
        uniforms::rotate_symbol::Value( rotateInShader ),
        uniforms::aspect_ratio::Value( state.getSize().aspectRatio() ),
        std::forward<Args>(args)...
    };
}

SymbolIconProgram::LayoutUniformValues
SymbolIconProgram::layoutUniformValues(const bool isText,
                                       const bool hasVariablePacement,
                                       const style::SymbolPropertyValues& values,
                                       const Size& texsize,
                                       const std::array<float, 2>& pixelsToGLUnits,
                                       const bool alongLine,
                                       const RenderTile& tile,
                                       const TransformState& state,
                                       const float symbolFadeChange) {
    return makeValues<SymbolIconProgram::LayoutUniformValues>(
        isText,
        hasVariablePacement,
        values,
        texsize,
        pixelsToGLUnits,
        alongLine,
        tile,
        state,
        symbolFadeChange
    );
}

template <class Name, class PaintProperties>
typename SymbolSDFProgram<Name, PaintProperties>::LayoutUniformValues
SymbolSDFProgram<Name, PaintProperties>::layoutUniformValues(const bool isText,
                                                       const bool hasVariablePacement,
                                                       const style::SymbolPropertyValues& values,
                                                       const Size& texsize,
                                                       const std::array<float, 2>& pixelsToGLUnits,
                                                       const float pixelRatio,
                                                       const bool alongLine,
                                                       const RenderTile& tile,
                                                       const TransformState& state,
                                                       const float symbolFadeChange,
                                                       const SymbolSDFPart part) {
    const float gammaScale = (values.pitchAlignment == AlignmentType::Map
                              ? std::cos(state.getPitch()) * state.getCameraToCenterDistance()
                              : 1.0);

    return makeValues<SymbolSDFProgram<Name, PaintProperties>::LayoutUniformValues>(
        isText,
        hasVariablePacement,
        values,
        texsize,
        pixelsToGLUnits,
        alongLine,
        tile,
        state,
        symbolFadeChange,
        uniforms::gamma_scale::Value( gammaScale ),
        uniforms::device_pixel_ratio::Value( pixelRatio ),
        uniforms::is_halo::Value( part == SymbolSDFPart::Halo )
    );
}

SymbolTextAndIconProgram::LayoutUniformValues SymbolTextAndIconProgram::layoutUniformValues(
    const bool hasVariablePacement,
    const style::SymbolPropertyValues& values,
    const Size& texsize,
    const Size& texsize_icon,
    const std::array<float, 2>& pixelsToGLUnits,
    const float pixelRatio,
    const bool alongLine,
    const RenderTile& tile,
    const TransformState& state,
    const float symbolFadeChange,
    const SymbolSDFPart part) {
    return SymbolTextAndIconProgram::LayoutUniformValues(
        SymbolSDFProgram<SymbolSDFTextProgram, style::TextPaintProperties>::layoutUniformValues(true,
                                                                                                hasVariablePacement,
                                                                                                values,
                                                                                                texsize,
                                                                                                pixelsToGLUnits,
                                                                                                pixelRatio,
                                                                                                alongLine,
                                                                                                tile,
                                                                                                state,
                                                                                                symbolFadeChange,
                                                                                                part)
            .concat(gfx::UniformValues<SymbolTextAndIconProgramUniforms>(uniforms::texsize::Value(texsize_icon))));
}

template class SymbolSDFProgram<SymbolSDFIconProgram, style::IconPaintProperties>;
template class SymbolSDFProgram<SymbolSDFTextProgram, style::TextPaintProperties>;

} // namespace mbgl
