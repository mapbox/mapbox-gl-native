#include <mbgl/gl/value.hpp>

namespace mbgl {
namespace gl {
namespace value {

const constexpr StencilFunc::Type StencilFunc::Default;
const constexpr StencilMask::Type StencilMask::Default;
const constexpr StencilTest::Type StencilTest::Default;
const constexpr StencilOp::Type StencilOp::Default;
const constexpr DepthRange::Type DepthRange::Default;
const constexpr DepthMask::Type DepthMask::Default;
const constexpr DepthTest::Type DepthTest::Default;
const constexpr DepthFunc::Type DepthFunc::Default;
const constexpr Blend::Type Blend::Default;
const constexpr BlendFunc::Type BlendFunc::Default;
const constexpr BlendColor::Type BlendColor::Default;
const constexpr ColorMask::Type ColorMask::Default;
const constexpr ClearDepth::Type ClearDepth::Default;
const constexpr ClearColor::Type ClearColor::Default;
const constexpr ClearStencil::Type ClearStencil::Default;
const constexpr Program::Type Program::Default;
const constexpr LineWidth::Type LineWidth::Default;
const constexpr ActiveTexture::Type ActiveTexture::Default;
const constexpr BindTexture::Type BindTexture::Default;
const constexpr BindVertexArray::Type BindVertexArray::Default;

#ifndef GL_ES_VERSION_2_0
const constexpr PixelZoom::Type PixelZoom::Default;
const constexpr RasterPos::Type RasterPos::Default;
#endif // GL_ES_VERSION_2_0

} // namespace value
} // namespace gl
} // namespace mbgl
