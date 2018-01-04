#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>

#include <mbgl/util/io.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/programs/binary_program.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/shaders/shaders.hpp>

#include <string>

namespace mbgl {
namespace gl {

template <class P, class As, class Us>
class Program {
public:
    using Primitive = P;
    using Attributes = As;
    using Uniforms = Us;

    using UniformValues = typename Uniforms::Values;
    using AttributeBindings = typename Attributes::Bindings;

    Program(Context& context, const std::string& vertexSource, const std::string& fragmentSource)
        : program(
              context.createProgram(context.createShader(ShaderType::Vertex, vertexSource),
                                    context.createShader(ShaderType::Fragment, fragmentSource))),
          uniformsState((context.linkProgram(program), Uniforms::bindLocations(program))),
          attributeLocations(Attributes::bindLocations(program)) {
        // Re-link program after manually binding only active attributes in Attributes::bindLocations
        context.linkProgram(program);
    }

    template <class BinaryProgram>
    Program(Context& context, const BinaryProgram& binaryProgram)
        : program(context.createProgram(binaryProgram.format(), binaryProgram.code())),
          uniformsState(Uniforms::loadNamedLocations(binaryProgram)),
          attributeLocations(Attributes::loadNamedLocations(binaryProgram)) {
    }
    
    static Program createProgram(gl::Context& context,
                                 const ProgramParameters& programParameters,
                                 const char* name,
                                 const char* vertexSource_,
                                 const char* fragmentSource_) {
        const std::string vertexSource = shaders::vertexSource(programParameters, vertexSource_);
        const std::string fragmentSource = shaders::fragmentSource(programParameters, fragmentSource_);

#if MBGL_HAS_BINARY_PROGRAMS
        optional<std::string> cachePath = programParameters.cachePath(name);
        if (cachePath && context.supportsProgramBinaries()) {
            const std::string identifier = shaders::programIdentifier(vertexSource, fragmentSource);

            try {
                if (auto cachedBinaryProgram = util::readFile(*cachePath)) {
                    const BinaryProgram binaryProgram(std::move(*cachedBinaryProgram));
                    if (binaryProgram.identifier() == identifier) {
                        return Program { context, binaryProgram };
                    } else {
                        Log::Warning(Event::OpenGL,
                                     "Cached program %s changed. Recompilation required.",
                                     name);
                    }
                }
            } catch (std::runtime_error& error) {
                Log::Warning(Event::OpenGL, "Could not load cached program: %s",
                             error.what());
            }

            // Compile the shader
            Program result{ context, vertexSource, fragmentSource };

            try {
                if (const auto binaryProgram =
                        result.template get<BinaryProgram>(context, identifier)) {
                    util::write_file(*cachePath, binaryProgram->serialize());
                    Log::Warning(Event::OpenGL, "Caching program in: %s", (*cachePath).c_str());
                }
            } catch (std::runtime_error& error) {
                Log::Warning(Event::OpenGL, "Failed to cache program: %s", error.what());
            }

            return std::move(result);
        }
#endif

        (void)name;
        return Program { context, vertexSource, fragmentSource };
    }

    template <class BinaryProgram>
    optional<BinaryProgram> get(Context& context, const std::string& identifier) const {
        if (auto binaryProgram = context.getBinaryProgram(program)) {
            return BinaryProgram{ binaryProgram->first, std::move(binaryProgram->second),
                                  identifier, Attributes::getNamedLocations(attributeLocations),
                                  Uniforms::getNamedLocations(uniformsState) };
        }
        return {};
    }

    template <class DrawMode>
    void draw(Context& context,
              DrawMode drawMode,
              DepthMode depthMode,
              StencilMode stencilMode,
              ColorMode colorMode,
              const UniformValues& uniformValues,
              VertexArray& vertexArray,
              const AttributeBindings& attributeBindings,
              const IndexBuffer<DrawMode>& indexBuffer,
              std::size_t indexOffset,
              std::size_t indexLength) {
        static_assert(std::is_same<Primitive, typename DrawMode::Primitive>::value, "incompatible draw mode");

        context.setDrawMode(drawMode);
        context.setDepthMode(depthMode);
        context.setStencilMode(stencilMode);
        context.setColorMode(colorMode);

        context.program = program;

        Uniforms::bind(uniformsState, uniformValues);

        vertexArray.bind(context,
                        indexBuffer.buffer,
                        Attributes::toBindingArray(attributeLocations, attributeBindings));

        context.draw(drawMode.primitiveType,
                     indexOffset,
                     indexLength);
    }

private:
    UniqueProgram program;

    typename Uniforms::State uniformsState;
    typename Attributes::Locations attributeLocations;
};

} // namespace gl
} // namespace mbgl
