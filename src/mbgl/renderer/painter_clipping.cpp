#include <mbgl/renderer/painter.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/util/clip_id.hpp>

namespace mbgl {

void Painter::renderClippingMask(const UnwrappedTileID& tileID, const ClipID& clip) {
    static const style::FillPaintProperties::Evaluated properties {};
    static const FillProgram::PaintPropertyBinders paintAttibuteData(properties, 0);
    programs->fill.get(properties).draw(
        context,
        gl::Triangles(),
        gl::DepthMode::disabled(),
        gl::StencilMode {
            gl::StencilMode::Always(),
            static_cast<int32_t>(clip.reference.to_ulong()),
            0b11111111,
            gl::StencilMode::Keep,
            gl::StencilMode::Keep,
            gl::StencilMode::Replace
        },
        gl::ColorMode::disabled(),
        FillProgram::UniformValues {
            uniforms::u_matrix::Value{ matrixForTile(tileID) },
            uniforms::u_world::Value{ context.viewport.getCurrentValue().size },
        },
        tileVertexBuffer,
        quadTriangleIndexBuffer,
        tileTriangleSegments,
        paintAttibuteData,
        properties,
        state.getZoom(),
        "clipping"
    );
}

} // namespace mbgl
