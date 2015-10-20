#include "gl_config.hpp"

namespace mbgl {
namespace gl {

const StencilFunc::Type StencilFunc::Default = { GL_ALWAYS, 0, ~0u };
const StencilMask::Type StencilMask::Default = ~0u;
const StencilTest::Type StencilTest::Default = GL_FALSE;
const StencilOp::Type StencilOp::Default = { GL_KEEP, GL_KEEP, GL_REPLACE };
const DepthRange::Type DepthRange::Default = { 0, 1 };
const DepthMask::Type DepthMask::Default = GL_TRUE;
const DepthTest::Type DepthTest::Default = GL_FALSE;
const DepthFunc::Type DepthFunc::Default = GL_LEQUAL;
const Blend::Type Blend::Default = GL_TRUE;
const BlendFunc::Type BlendFunc::Default = { GL_ONE, GL_ONE_MINUS_SRC_ALPHA };
const ColorMask::Type ColorMask::Default = { GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE };
const ClearDepth::Type ClearDepth::Default = 1;
const ClearColor::Type ClearColor::Default = { 0, 0, 0, 0 };
const ClearStencil::Type ClearStencil::Default = 0;

}
}
