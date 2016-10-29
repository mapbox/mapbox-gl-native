#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/gl/context.hpp>

#include <string>

namespace mbgl {
namespace gl {

template <class As, class Us>
class Program {
public:
    using Attributes = As;
    using Vertex = typename Attributes::Vertex;

    using Uniforms = Us;
    using UniformValues = typename Uniforms::Values;

    Program(Context& context, const std::string& vertexSource, const std::string& fragmentSource)
        : vertexShader(context.createShader(ShaderType::Vertex, vertexSource)),
          fragmentShader(context.createShader(ShaderType::Fragment, fragmentSource)),
          program(context.createProgram(vertexShader, fragmentShader)),
          attributesState(Attributes::state(program)),
          uniformsState(Uniforms::state(program)) {}

    UniqueShader vertexShader;
    UniqueShader fragmentShader;
    UniqueProgram program;

    typename Attributes::State attributesState;
    typename Uniforms::State uniformsState;
};

} // namespace gl
} // namespace mbgl
