#include "gl_config.hpp"

namespace mbgl {
namespace gl {

const ClearDepth::Type ClearDepth::Default = 0;
const ClearColor::Type ClearColor::Default = { 0, 0, 0, 0 };
const ClearStencil::Type ClearStencil::Default = 0;
const StencilMask::Type StencilMask::Default = ~0u;
const DepthMask::Type DepthMask::Default = GL_TRUE;
const ColorMask::Type ColorMask::Default = { false, false, false, false };
const StencilFunc::Type StencilFunc::Default = { GL_ALWAYS, 0, ~0u };
const StencilTest::Type StencilTest::Default = false;
const DepthRange::Type DepthRange::Default = { 0, 1 };
const DepthTest::Type DepthTest::Default = false;
const Blend::Type Blend::Default = false;

}
}
