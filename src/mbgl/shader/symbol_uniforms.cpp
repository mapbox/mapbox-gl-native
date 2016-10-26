#include <mbgl/shader/symbol_uniforms.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>

namespace mbgl {

using namespace style;

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
            pixelsToGLUnits[0] * scale * state.getAltitude(),
            pixelsToGLUnits[1] * scale * state.getAltitude()
        }};
    }

    // adjust min/max zooms for variable font sies
    float zoomAdjust = std::log(values.paintSize / values.layoutSize) / std::log(2);

    return Values {
        tile.translatedMatrix(values.translate,
                              values.translateAnchor,
                              state),
        values.opacity,
        extrudeScale,
        std::array<float, 2> {{ float(texsize.width) / 4, float(texsize.height) / 4 }},
        (state.getZoom() - zoomAdjust) * 10.0f,
        values.rotationAlignment == AlignmentType::Map,
        0,
        1,
        std::forward<Args>(args)...
    };
}

SymbolIconUniforms::Values
SymbolIconUniforms::values(const style::SymbolPropertyValues& values,
                           const Size& texsize,
                           const std::array<float, 2>& pixelsToGLUnits,
                           const RenderTile& tile,
                           const TransformState& state)
{
    return makeValues<SymbolIconUniforms::Values>(
        values,
        texsize,
        pixelsToGLUnits,
        tile,
        state
    );
}

static SymbolSDFUniforms::Values makeSDFValues(const style::SymbolPropertyValues& values,
                                               const Size& texsize,
                                               const std::array<float, 2>& pixelsToGLUnits,
                                               const RenderTile& tile,
                                               const TransformState& state,
                                               float pixelRatio,
                                               Color color,
                                               float buffer,
                                               float gammaAdjust)
{
    // The default gamma value has to be adjust for the current pixelratio so that we're not
    // drawing blurry font on retina screens.
    const float gammaBase = 0.105 * values.sdfScale / values.paintSize / pixelRatio;
    const float gammaScale = values.pitchAlignment == AlignmentType::Map
        ? 1.0 / std::cos(state.getPitch())
        : 1.0;

    return makeValues<SymbolSDFUniforms::Values>(
        values,
        texsize,
        pixelsToGLUnits,
        tile,
        state,
        color,
        buffer,
        (gammaBase + gammaAdjust) * gammaScale,
        state.getPitch(),
        -1.0f * state.getAngle(),
        (state.getSize().width * 1.0f) / (state.getSize().height * 1.0f),
        values.pitchAlignment == AlignmentType::Map
    );
}

SymbolSDFUniforms::Values
SymbolSDFUniforms::haloValues(const style::SymbolPropertyValues& values,
                              const Size& texsize,
                              const std::array<float, 2>& pixelsToGLUnits,
                              const RenderTile& tile,
                              const TransformState& state,
                              float pixelRatio)
{
    const float scale = values.paintSize / values.sdfScale;
    const float sdfPx = 8.0f;
    const float blurOffset = 1.19f;
    const float haloOffset = 6.0f;

    return makeSDFValues(
        values,
        texsize,
        pixelsToGLUnits,
        tile,
        state,
        pixelRatio,
        values.haloColor,
        (haloOffset - values.haloWidth / scale) / sdfPx,
        values.haloBlur * blurOffset / scale / sdfPx
    );
}

SymbolSDFUniforms::Values
SymbolSDFUniforms::foregroundValues(const style::SymbolPropertyValues& values,
                                    const Size& texsize,
                                    const std::array<float, 2>& pixelsToGLUnits,
                                    const RenderTile& tile,
                                    const TransformState& state,
                                    float pixelRatio)
{
    return makeSDFValues(
        values,
        texsize,
        pixelsToGLUnits,
        tile,
        state,
        pixelRatio,
        values.color,
        (256.0f - 64.0f) / 256.0f,
        0
    );
}

} // namespace mbgl
