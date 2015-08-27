#include "gl_config.hpp"

#include <algorithm>

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


void Config::enableVertexAttributes(const std::set<int32_t>& attributes) {
    std::set<int32_t> difference;
    std::set_difference(enabledVertexAttributes.begin(), enabledVertexAttributes.end(),
                        attributes.begin(), attributes.end(),
                        std::inserter(difference, difference.begin()));
    for (int32_t attribute : difference) {
        MBGL_CHECK_ERROR(glDisableVertexAttribArray(attribute));
    }

    difference.clear();
    std::set_difference(attributes.begin(), attributes.end(),
                        enabledVertexAttributes.begin(), enabledVertexAttributes.end(),
                        std::inserter(difference, difference.begin()));
    for (int32_t attribute : difference) {
        MBGL_CHECK_ERROR(glEnableVertexAttribArray(attribute));
    }

    enabledVertexAttributes = attributes;
}

void Config::resetVertexAttributes() {
    enabledVertexAttributes.clear();
}

}
}
