#pragma once

#include <cstddef>

namespace mbgl {
namespace gfx {

class Context;
class DrawMode;
class DepthMode;
class StencilMode;
class ColorMode;
class CullFaceMode;
class DrawScope;
class IndexBuffer;
template <class> class UniformValues;
template <class> class AttributeBindings;
template <class> class TextureBindings;

template <class AttributeList, class UniformList, class TextureList>
class Program {
protected:
    Program() = default;

public:
    virtual ~Program() = default;

    Program(Program&&) = delete;
    Program(const Program&) = delete;
    Program& operator=(Program&&) = delete;
    Program& operator=(const Program&) = delete;

    virtual void draw(Context&,
                      const DrawMode&,
                      const DepthMode&,
                      const StencilMode&,
                      const ColorMode&,
                      const CullFaceMode&,
                      const UniformValues<UniformList>&,
                      DrawScope&,
                      const AttributeBindings<AttributeList>&,
                      const TextureBindings<TextureList>&,
                      const IndexBuffer&,
                      std::size_t indexOffset,
                      std::size_t indexLength) = 0;
};

} // namespace gfx
} // namespace mbgl
